#pragma once

#include <string>
#include <libpq-fe.h>

class PGTransaction{
    public:
        PGTransaction(PGconn* c);
        ~PGTransaction();

        void begin();
        void rollback();
        void commit();
        void execute(const std::string& query);
    private:
        bool commited = false;
        PGconn* conn;
};