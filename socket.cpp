#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <string.h>

#include  <plog/Log.h>
#include "socket.hpp"  


tmdb::socket::socket(int socket_id) : socket_id_(socket_id)
{
    std::cout << "ctor: " << socket_id_ << std::endl;
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
    std::cout << "move ctor: " << socket_id_ << std::endl;
    move.swap(*this);
}

tmdb::socket& tmdb::socket::operator = (socket&& move) noexcept
{
   std::cout << "operator =: " << socket_id_ << std::endl;
    move.swap(*this);
    return *this;
}


tmdb::socket& tmdb::socket::clone(const socket& obj)
{
    tmdb::socket& socket_clone = *(new socket(obj.socket_id_));
    int fd_clone = ::dup(obj.socket_id_);
    if(is_invalid_ == fd_clone)
    {
        LOG_ERROR << __PRETTY_FUNCTION__ << ": clone failed: " << strerror(errno);
        exit(0);
    }
    socket_clone.socket_id_ = fd_clone;
    
    return socket_clone;
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
        }
    }
    socket_id_ = is_invalid_;
}

void tmdb::socket::swap(socket& obj) noexcept
{
    std::swap(socket_id_, obj.socket_id_);
    std::cout << "swap: " << socket_id_ << " to: " << obj.socket_id_ << std::endl;
}


tmdb::connector::connector(const std::string& host, int port): provider(::socket(PF_INET, SOCK_STREAM, 0))
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
    std::cout << "connector\n";
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
 

bool tmdb::provider::write(const char* buffer, std::size_t len)
{
    std::size_t shift = 0;

    while(shift < len)
    {
        std::size_t res = ::write(get_socket_id(), buffer + shift, len - shift);
        if(res == static_cast<std::size_t>(-1))
        {
            if(EINTR == errno || EAGAIN == errno)
            { continue; }
            LOG_ERROR << __PRETTY_FUNCTION__ << ": failed to write to socket: " << strerror(errno);
            return false;
        }
        shift += res;
    }
    return true;
}

bool tmdb::provider::write(const std::vector<unsigned char>& msg)
{
    return write(reinterpret_cast<const char*>(msg.data()), msg.size());
}

bool tmdb::provider::write(const std::string& msg)
{
    return write(msg.c_str(), msg.length()); 
}


bool tmdb::provider::close()
{
    state_ = state::undef;
    if(::shutdown(get_socket_id(), SHUT_WR) != 0)
    {
        LOG_ERROR << __PRETTY_FUNCTION__ << ": shutdown error: " << strerror(errno);
        return false;
    }
    return true;
}



bool tmdb::provider::read(unsigned char* buffer, std::size_t len)
{
    std::size_t read = 0;
    while(read < len)
    {
        std::size_t res = ::read(get_socket_id(), buffer + read, len - read);
        if(static_cast<std::size_t>(-1) == res)
        {
            if(EINTR == errno || EAGAIN == errno || ETIMEDOUT == errno)
            { continue; }
            LOG_ERROR << __PRETTY_FUNCTION__ << ": failed to read to socket: " << strerror(errno);
            return false;
        }
        
        /* end of file */
        if(0 == res)
        {
            state_ = state::eof;
            LOG_ERROR << __PRETTY_FUNCTION__ << ": eof: " << strerror(errno);
            break;
        }
        
        read += res;
    }
    return true;
}
