#include <cerrno>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "tcp_server.hpp"

#ifndef _DB_TCP_CLIENT_H
#define _DB_TCP_CLIENT_H

namespace tmdb
{
    class time;
    class serializer;
    class socket;

    class connector : public socket
    {
    public:
        connector(std::string const& host, int port);
    };
}

#endif