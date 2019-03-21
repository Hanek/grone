#include <cerrno>
#include <iostream>
#include <string>
#include <functional>
#include "socket.hpp"
#include "db_core.hpp"
#include "protocol.hpp"

#ifndef _SERVER_H
#define _SERVER_H

namespace tmdb
{
    class time;
    class serializer;

    class server
    {
    private:
        tmdb::core core_;
        size_t pool_size_;
        size_t max_connections_;
    public:
        server(size_t, size_t);
        ~server();
        void init();
        std::map<const char, std::function<void(const std::string&,std::string&)> > dispatchMap_;  
        static void worker(int id, tmdb::provider&, tmdb::server&);
        void dispatch(tmdb::request&);
        void run();
        
    };
}

#endif