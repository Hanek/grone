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





tmdb::connect::connect(std::string const& host, int port): socket(::socket(PF_INET, SOCK_STREAM, 0))
{
    struct sockaddr_in server_addr{};
    server_addr.sin_family       = AF_INET;
    server_addr.sin_port         = htons(port);
    server_addr.sin_addr.s_addr  = inet_addr(host.c_str());

    if (::connect(get_socket_id(), (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0)
    {
        close();
        LOG_ERROR << __PRETTY_FUNCTION__ << ": connaction failed: " << strerror(errno);
        exit(0);
    }
}


//
//ServerSocket::ServerSocket(int port)
//    : BaseSocket(::socket(PF_INET, SOCK_STREAM, 0))
//{
//    struct sockaddr_in serverAddr;
//    bzero((char*)&serverAddr, sizeof(serverAddr));
//    serverAddr.sin_family       = AF_INET;
//    serverAddr.sin_port         = htons(port);
//    serverAddr.sin_addr.s_addr  = INADDR_ANY;
//
//    if (::bind(getSocketId(), (struct sockaddr *) &serverAddr, sizeof(serverAddr)) != 0)
//    {
//        close();
//        throw std::runtime_error(buildErrorMessage("ServerSocket::", __func__, ": bind: ", strerror(errno)));
//    }
//
//    if (::listen(getSocketId(), maxConnectionBacklog) != 0)
//    {
//        close();
//        throw std::runtime_error(buildErrorMessage("ServerSocket::", __func__, ": listen: ", strerror(errno)));
//    }
//}
//
//DataSocket ServerSocket::accept()
//{
//    if (getSocketId() == invalidSocketId)
//    {
//        throw std::logic_error(buildErrorMessage("ServerSocket::", __func__, ": accept called on a bad socket object (this object was moved)"));
//    }
//
//    struct  sockaddr_storage    serverStorage;
//    socklen_t                   addr_size   = sizeof serverStorage;
//    int newSocket = ::accept(getSocketId(), (struct sockaddr*)&serverStorage, &addr_size);
//    if (newSocket == -1)
//    {
//        throw std::runtime_error(buildErrorMessage("ServerSocket:", __func__, ": accept: ", strerror(errno)));
//    }
//    return DataSocket(newSocket);
//}
//
//void DataSocket::putMessageData(char const* buffer, std::size_t size)
//{
//    std::size_t     dataWritten = 0;
//
//    while(dataWritten < size)
//    {
//        std::size_t put = write(getSocketId(), buffer + dataWritten, size - dataWritten);
//        if (put == static_cast<std::size_t>(-1))
//        {
//            switch(errno)
//            {
//                case EINVAL:
//                case EBADF:
//                case ECONNRESET:
//                case ENXIO:
//                case EPIPE:
//                {
//                    // Fatal error. Programming bug
//                    throw std::domain_error(buildErrorMessage("DataSocket::", __func__, ": write: critical error: ", strerror(errno)));
//                }
//                case EDQUOT:
//                case EFBIG:
//                case EIO:
//                case ENETDOWN:
//                case ENETUNREACH:
//                case ENOSPC:
//                {
//                    // Resource acquisition failure or device error
//                    throw std::runtime_error(buildErrorMessage("DataSocket::", __func__, ": write: resource failure: ", strerror(errno)));
//                }
//                case EINTR:
//                        // TODO: Check for user interrupt flags.
//                        //       Beyond the scope of this project
//                        //       so continue normal operations.
//                case EAGAIN:
//                {
//                    // Temporary error.
//                    // Simply retry the read.
//                    continue;
//                }
//                default:
//                {
//                    throw std::runtime_error(buildErrorMessage("DataSocket::", __func__, ": write: returned -1: ", strerror(errno)));
//                }
//            }
//        }
//        dataWritten += put;
//    }
//    return;
//}
//
//void DataSocket::putMessageClose()
//{
//    if (::shutdown(getSocketId(), SHUT_WR) != 0)
//    {
//        throw std::domain_error(buildErrorMessage("HTTPProtocol::", __func__, ": shutdown: critical error: ", strerror(errno)));
//    }
//}