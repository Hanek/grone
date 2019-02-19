#include <iostream>
#include <string>


#ifndef _DB_PROTOCOL_H
#define _DB_PROTOCOL_H

namespace tmdb
{
    class provider;

    class proto
    {
    protected:
        provider& socket_;
    public:
        proto(provider& socket): socket_(socket) {} 
        virtual ~proto() {}

        virtual void sendMessage(std::string const& url, std::string const& message) = 0;
        virtual void recvMessage(std::string& message) = 0;
    };

    class protocol: public proto
    {
    public:
        using proto::proto;
        void sendMessage(std::string const& url, std::string const& message);
        void recvMessage(std::string& message);
    };

}

#endif