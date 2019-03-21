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
        core&  core_;
        size_t pool_size_;
        size_t max_connections_;
    public:
        server(core&, size_t, size_t);
        ~server();
        static void worker(int id, tmdb::provider&, tmdb::server&);
        void dispatch(tmdb::request&, tmdb::request&);
        void run();
        
    };
}

#endif