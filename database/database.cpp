#include "database.hpp"
#include "pg_transaction.hpp"

Database::Database(const int& num_conns, const char* conn_data){
    poolSize = num_conns;
    conn = conn_data;
    for(int i = 0; i < num_conns; ++i){
        PGconn* c = PQconnectdb(conn);
        while(PQstatus(c) != CONNECTION_OK){
            PQfinish(c);
            c = PQconnectdb(conn);
        }
        pool.push_back(c);
    }
}

Database::~Database(){
    std::lock_guard<std::mutex> lock(poolMtx);
    for(auto& c: pool){
        PQfinish(c);
    }
}

PGResultWrapper Database::query(const std::string& query){
    PGconn* c = getConnection();
    return PGResultWrapper(c, PQexec(c, query.c_str()));
}

void Database::transaction(const std::vector<std::string>& queries){
    PGTransaction tr(getConnection());
    try{
        tr.begin();
        for(auto& q: queries){
            tr.execute(q);
        }
        tr.commit();
    }catch(const std::exception& e){
        tr.rollback();
    }
}

PGconn* Database::getConnection(){
    std::lock_guard<std::mutex> lock(poolMtx);
    PGconn* c = pool[next];
    while(!isHealthy(c)){
        PQfinish(c);
        pool.erase(pool.begin() + next);
        PGconn* new_c = PQconnectdb(conn);
        while(PQstatus(new_c) != CONNECTION_OK){
            PQfinish(new_c);
            new_c = PQconnectdb(conn);
        }
        pool.push_back(new_c);
        c = new_c;
    }
    next = (next + 1) % poolSize;
    return c;
}

bool Database::isHealthy(PGconn* c){
    if(!c) return false;
    PGresult* res = PQexec(c, "SELECT 1;");
    bool b = (PQresultStatus(res) == PGRES_TUPLES_OK);
    PQclear(res);
    return b;
}