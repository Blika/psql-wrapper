#pragma once

#include "pg_result_wrapper.hpp"
#include <libpq-fe.h>
#include <mutex>
#include <string>
#include <vector>

class Database{
    public:
        Database(const int& num_conns, const char* conn_data);
        ~Database();

        template<typename... Args>
        PGResultWrapper execParams(const std::string& query, Args&&... args){
            const int nParams = sizeof...(args);
            std::vector<const char*> paramValues;
            std::vector<int> paramLengths;
            std::vector<int> paramFormats;
            std::vector<std::string> stringStorage;
            processParams(paramValues, paramLengths, paramFormats, stringStorage, std::forward<Args>(args)...);
            PGconn* c = getConnection();
            return PGResultWrapper(c, PQexecParams(
                c,
                query.c_str(),
                nParams,
                NULL,
                paramValues.empty() ? NULL : paramValues.data(),
                paramLengths.empty() ? NULL : paramLengths.data(),
                paramFormats.empty() ? NULL : paramFormats.data(),
                0
            ));
        }
        PGResultWrapper query(const std::string& query);
        void transaction(const std::vector<std::string>& queries);

    private:
        PGconn* getConnection();
        bool isHealthy(PGconn* c);

        template<typename T, typename... Rest>
        void processParams(std::vector<const char*>& paramValues, std::vector<int>& paramLengths, std::vector<int>& paramFormats, std::vector<std::string>& stringStorage, T&& first, Rest&&... rest){
            if constexpr(std::is_same_v<std::decay_t<T>, const char*>){
                paramValues.push_back(first);
                paramLengths.push_back(static_cast<int>(strlen(first)));
            }else if constexpr(std::is_same_v<std::decay_t<T>, std::string>){
                stringStorage.push_back(first);
                paramValues.push_back(stringStorage.back().c_str());
                paramLengths.push_back(static_cast<int>(first.length()));
            }else if constexpr(std::is_integral_v<std::decay_t<T>> || std::is_floating_point_v<std::decay_t<T>>){
                stringStorage.push_back(std::to_string(first));
                paramValues.push_back(stringStorage.back().c_str());
                paramLengths.push_back(static_cast<int>(stringStorage.back().length()));
            }
            paramFormats.push_back(0);
            if constexpr(sizeof...(rest) > 0){
                processParams(paramValues, paramLengths, paramFormats, stringStorage, std::forward<Rest>(rest)...);
            }
        }
        void processParams(std::vector<const char*>&, std::vector<int>&, std::vector<int>&, std::vector<std::string>&){}

        std::mutex poolMtx;
        std::vector<PGconn*> pool;
        int poolSize;
        int next = 0;
        const char* conn;
};