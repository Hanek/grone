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

void tmdb::protocol::send(const std::string& message, const char& type)
{
    const_request req(message, type);
    return send(req);
}
  
void tmdb::protocol::send(const_request& req)
{
    socket_.write<char>(req.type_);
    socket_.write<size_t>(req.len_);
    socket_.write(req.val_);
    
    if(provider::state::eof == socket_.state_)
    {
        is_ready_ = false;
    }  
}


void tmdb::protocol::recv(std::string& message, char& type)
{
    size_t len;
    char buffer[4096] = {0};
    socket_.read<char>(&type);
    socket_.read<size_t>(&len);
    socket_.read(buffer, len);
    
    if(provider::state::eof == socket_.state_)
    {
        is_ready_ = false;
        return;
    }
    message = buffer;
}

void tmdb::protocol::recv(request& req)
{
    char buffer[4096] = {0};
    socket_.read<char>(&req.type_);
    socket_.read<size_t>(&req.len_);
    socket_.read(buffer, req.len_);
    req.val_ = buffer;
    if(provider::state::eof == socket_.state_)
    {
        is_ready_ = false;
        return;
    }
}
