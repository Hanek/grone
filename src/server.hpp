#include <cerrno>
#include <iostream>
#include <string>
#include <functional>
#include "socket.hpp"
#include "db_core.hpp"
#include "protocol.hpp"

#ifndef _SERVER_H
#define _SERVER_H

namespace grone
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
        static void worker(int id, grone::provider&, grone::server&);
        void dispatch(grone::request&, grone::request&);
        void run();
        
    };
}

#endif