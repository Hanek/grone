#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include  <plog/Log.h>

#include "tcp_client.hpp"



tmdb::connector::connector(std::string const& host, int port): socket(::socket(PF_INET, SOCK_STREAM, 0))
{
    struct sockaddr_in server_addr{};
    server_addr.sin_family       = AF_INET;
    server_addr.sin_port         = htons(port);
    server_addr.sin_addr.s_addr  = inet_addr(host.c_str());

    if(::connect(get_socket_id(), (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0)
    {
        close();
        LOG_ERROR << __PRETTY_FUNCTION__ << ": connection failed: " << strerror(errno);
        exit(0);
    }
}