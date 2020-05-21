#include <iostream>
#include <string>
#include <tuple>
#include <vector>


#ifndef _DB_PROTOCOL_H
#define _DB_PROTOCOL_H

namespace grone
{
    class provider;
    
    class const_request
    {
    public:
        const char   type_;
        const size_t len_;
        const std::string& val_;
        const_request(const std::string& msg, char type): type_(type), len_(msg.length()), val_(msg) {}
    };
    
    class request
    {
    public:
        char   type_;
        size_t len_;
        std::vector<unsigned char> val_;
        
        /* total number of bytes in incoming request */
        size_t  inlen_;
        /* total number of bytes in outcoming request */
        size_t outlen_;
        std::vector<std::tuple<std::string,std::vector<unsigned char> > > inreq_;
        std::vector<std::tuple<std::string,std::vector<unsigned char> > > outreq_;
    };
        
    
    class protocol
    {
    private:
        provider& socket_;
    public:
        bool is_ready_;
        protocol(provider& socket): socket_(socket), is_ready_(true) {}
        void send(const std::string& message, const char& type);
        void send(const_request& req);
        void send(request& req);
        void recv(std::string& message, char& type);
        void recv(request& req);
    };
    

}

#endif