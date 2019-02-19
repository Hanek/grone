#include <iostream>
#include <string>


#ifndef _DB_PROTOCOL_H
#define _DB_PROTOCOL_H

namespace tmdb
{
    class provider;
    
    class request
    {
    public:
        char   type_;
        size_t len_;
        const  std::string& val_;
        request(const std::string& msg): val_(msg) {}
        request(const std::string& msg, char type): type_(type), len_(msg.length()), val_(msg) {}
    };
    
    
    class protocol
    {
    private:
        provider& socket_;
    public:
        protocol(provider& socket): socket_(socket) {}
        bool send(const std::string& message, const char& type);
        bool send(const request& req);
        bool recv(std::string& message, char& type);
        bool recv(request& req);
    };
    

}

#endif