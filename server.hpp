#include <cerrno>
#include <iostream>
#include "socket.hpp"

#ifndef _SERVER_H
#define _SERVER_H

namespace tmdb
{
    class time;
    class serializer;

    class server
    {
    private:
        size_t pool_size_;
        size_t max_connections_;
    public:
        server(size_t, size_t);
        ~server();
        static void worker(int id, tmdb::provider&);
        void run();
    };
}

#endif