#include  <plog/Log.h>
#include <cstring>
#include <cstdio>
#include "protocol.hpp"
#include "socket.hpp"





/*
 * blocking send - receive utilizing type-length-value standard
 * --------------------------------------
 * | 1 byte | 4 bytes | variable length |
 * --------------------------------------
 * |  type  | length  |     message     |
 * --------------------------------------
 */

bool tmdb::protocol::send(const std::string& message, const char& type)
{
    const request req(message, type);
    return send(req);
}

bool tmdb::protocol::send(const request& req)
{
    socket_.write<char>(req.type_);
    socket_.write<size_t>(req.len_);
    socket_.write(req.val_);
    
//    if(tmdb::socket::state::undef == socket_.state_)
//    {
//        socket_.close();
//        return false;
//    }
    
    return true;
}


bool tmdb::protocol::recv(std::string& message, char& type)
{
    size_t len;
    char buffer[4096] = {0};
    socket_.read<char>(&type);
    socket_.read<size_t>(&len);
    socket_.read(buffer, len);
    
//    if(tmdb::socket::state::undef == socket_.state_)
//    {
//        socket_.close();
//        return false;
//    }
    
    message = buffer;
    
    return true;
}
