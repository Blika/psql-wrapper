#include "pg_transaction.hpp"
#include <stdexcept>

PGTransaction::PGTransaction(PGconn* c): conn{c}{}

PGTransaction::~PGTransaction(){
    if(!commited) rollback();
}

void PGTransaction::begin(){
    PGresult* ptr = PQexec(conn, "BEGIN;");
    auto status = PQresultStatus(ptr);
    if(status == PGRES_FATAL_ERROR || status == PGRES_NONFATAL_ERROR || status == PGRES_BAD_RESPONSE || status == PGRES_PIPELINE_ABORTED || status == PGRES_EMPTY_QUERY){
        throw std::runtime_error(PQerrorMessage(conn));
    }
}

void PGTransaction::rollback(){
    PGresult* ptr = PQexec(conn, "ROLLBACK;");
    auto status = PQresultStatus(ptr);
    if(status == PGRES_FATAL_ERROR || status == PGRES_NONFATAL_ERROR || status == PGRES_BAD_RESPONSE || status == PGRES_PIPELINE_ABORTED || status == PGRES_EMPTY_QUERY){
        throw std::runtime_error(PQerrorMessage(conn));
    }
}

void PGTransaction::commit(){
    PGresult* ptr = PQexec(conn, "COMMIT;");
    auto status = PQresultStatus(ptr);
    if(status == PGRES_FATAL_ERROR || status == PGRES_NONFATAL_ERROR || status == PGRES_BAD_RESPONSE || status == PGRES_PIPELINE_ABORTED || status == PGRES_EMPTY_QUERY){
        throw std::runtime_error(PQerrorMessage(conn));
    }
    commited = true;
}

void PGTransaction::execute(const std::string& query){
    PGresult* ptr = PQexec(conn, query.c_str());
    auto status = PQresultStatus(ptr);
    if(status == PGRES_FATAL_ERROR || status == PGRES_NONFATAL_ERROR || status == PGRES_BAD_RESPONSE || status == PGRES_PIPELINE_ABORTED || status == PGRES_EMPTY_QUERY){
        throw std::runtime_error(PQerrorMessage(conn));
    }
}