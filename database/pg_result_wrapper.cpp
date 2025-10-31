#include "pg_result_wrapper.hpp"

PGResultWrapper::PGResultWrapper(PGconn* c, PGresult* r): conn{c}, ptr{r}{
}

PGResultWrapper::~PGResultWrapper(){
    PQclear(ptr);
}

ExecStatusType PGResultWrapper::status(){
    return PQresultStatus(ptr);
}

bool PGResultWrapper::isOK(){
    ExecStatusType s = status();
    if(s == PGRES_FATAL_ERROR || s == PGRES_NONFATAL_ERROR || s == PGRES_BAD_RESPONSE || s == PGRES_PIPELINE_ABORTED || s == PGRES_EMPTY_QUERY){
        return false;
    }else{
        return true;
    }
}

int PGResultWrapper::rows(){
    return PQntuples(ptr);
}
char* PGResultWrapper::getValue(int row, int column){
    return PQgetvalue(ptr, row, column);
}
std::string PGResultWrapper::getString(int row, int column){
    char* v = PQgetvalue(ptr, row, column);
    return std::string(v);
}
int PGResultWrapper::getInt(int row, int column){
    char* v = PQgetvalue(ptr, row, column);
    return std::atoi(v);
}
float PGResultWrapper::getFloat(int row, int column){
    char* v = PQgetvalue(ptr, row, column);
    return std::atof(v);
}
double PGResultWrapper::getDouble(int row, int column){
    char* v = PQgetvalue(ptr, row, column);
    return std::strtod(v, NULL);
}
bool PGResultWrapper::getBool(int row, int column){
    char* v = PQgetvalue(ptr, row, column);
    return std::atoi(v) == 1;
}