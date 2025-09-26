#pragma once

#include <string>
#include <libpq-fe.h>

class PGResultWrapper{
    public:
        PGresult* ptr;
        PGconn* conn;
        PGResultWrapper(PGconn* c, PGresult* r);
        ~PGResultWrapper();

        ExecStatusType status();
        bool isOK();
        int rows();
        char* getValue(const int& row, const int& column);
        int getInt(const int& row, const int& column);
        float getFloat(const int& row, const int& column);
        double getDouble(const int& row, const int& column);
        bool getBool(const int& row, const int& column);
        std::string getString(const int& row, const int& column);
};