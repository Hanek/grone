#include <cerrno>
#include <iostream>
#include "thread_pool.hpp"
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
        ThreadPool pool_;
        std::queue<provider> socket_queue_;
    public:
        server(size_t, size_t);
        ~server();
        void run();
    };
}

#endif