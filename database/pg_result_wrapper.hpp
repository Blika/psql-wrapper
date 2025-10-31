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
        char* getValue(int row, int column);
        int getInt(int row, int column);
        float getFloat(int row, int column);
        double getDouble(int row, int column);
        bool getBool(int row, int column);
        std::string getString(int row, int column);
};