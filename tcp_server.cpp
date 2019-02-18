#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include  <plog/Log.h>
#include "tcp_server.hpp" 


tmdb::socket::socket(int socket_id) : socket_id_(socket_id)
{
    if(is_invalid_ == socket_id_)
    {
        LOG_ERROR << __PRETTY_FUNCTION__ << ": invalid socket: " << strerror(errno);
    }
}

tmdb::socket::~socket()
{
    if(is_invalid_ == socket_id_)
    {
        return;
    }
    close();
}

tmdb::socket::socket(socket&& move) noexcept: socket_id_(is_invalid_)
{
    move.swap(*this);
}

tmdb::socket& tmdb::socket::operator = (socket&& move) noexcept
{
    move.swap(*this);
    return *this;
}


void tmdb::socket::close()
{
    if(is_invalid_ == socket_id_)
    {
        LOG_ERROR << __PRETTY_FUNCTION__ << ": socket is already closed";
    }
    while(true)
    {
        int state = ::close(socket_id_);
        if(is_invalid_ == state)
        {
            break;
        }

        if(!errno || errno == EINTR)
        {
            /* all is good */
        }
        else
        {
            LOG_ERROR << __PRETTY_FUNCTION__ << ": " << strerror(errno);
            exit(0);
        }
    }
    socket_id_ = is_invalid_;
}

void tmdb::socket::swap(socket& other) noexcept
{
    using std::swap;
    swap(socket_id_, other.socket_id_);
}



tmdb::listener::listener(int port): socket(::socket(PF_INET, SOCK_STREAM, 0))
{
    struct sockaddr_in server_addr;
    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family       = AF_INET;
    server_addr.sin_port         = htons(port);
    server_addr.sin_addr.s_addr  = INADDR_ANY;

    if(::bind(get_socket_id(), (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0)
    {
        close();
        LOG_ERROR << __PRETTY_FUNCTION__ << ": bind failed: " << strerror(errno);
        exit(0);
    }

    if(::listen(get_socket_id(), max_connection_) != 0)
    {
        close();
        LOG_ERROR << __PRETTY_FUNCTION__ << ": listen failed: " << strerror(errno);
        exit(0);
    }
    std::cout << "tcp server is running, listening 8080..." << std::endl; 
}


tmdb::provider tmdb::listener::accept()
{
    if(get_socket_id() == is_invalid_)
    {
        LOG_ERROR << __PRETTY_FUNCTION__ << ": invalid socket id";
        exit(0);
    }

    struct  sockaddr_storage    server_data;
    socklen_t                   addr_size   = sizeof server_data;
    int socket = ::accept(get_socket_id(), (struct sockaddr*)&server_data, &addr_size);
    if(-1 == socket)
    {
        LOG_ERROR << __PRETTY_FUNCTION__ << ": listen failed: " << strerror(errno);
        exit(0);
    }
    LOG_INFO << __PRETTY_FUNCTION__ << "client accepted: " << get_socket_id();
    std::cout << "client accepted: " <<  get_socket_id() << std::endl;
    return provider(socket);
}


void tmdb::provider::message_put(const char* buffer, std::size_t len)
{
    std::size_t shift = 0;

    while(shift < len)
    {
        std::size_t res = write(get_socket_id(), buffer + shift, len - shift);
        if(res == static_cast<std::size_t>(-1))
        {
            if(errno == EINTR || errno == EAGAIN)
            { continue; }
            LOG_ERROR << __PRETTY_FUNCTION__ << ": failed to write to socket: " << strerror(errno);
            exit(0);
        }
        shift += res;
    }
    return;
}


void tmdb::provider::message_put_close()
{
    if(::shutdown(get_socket_id(), SHUT_WR) != 0)
    {
        LOG_ERROR << __PRETTY_FUNCTION__ << ": shutdown error: " << strerror(errno);
        exit(0);
    }
}