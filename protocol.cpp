#include  <plog/Log.h>
#include "protocol.hpp"
#include "socket.hpp"


void tmdb::protocol::sendMessage(std::string const& url, std::string const& message)
{
    socket_.message_put(url.c_str(), url.size());
    socket_.message_put(message.c_str(), message.size());
    socket_.message_put_close();
}

// Utility class
// Used by getMessage() to open the string upto capacity size.
// Then on destruction resize to the actual size of the string.
class StringSizer
{
    std::string&    stringData;
    std::size_t&    currentSize;
    public:
        StringSizer(std::string& stringData, std::size_t& currentSize)
            : stringData(stringData)
            , currentSize(currentSize)
        {
            stringData.resize(stringData.capacity());
        }
        ~StringSizer()
        {
            stringData.resize(currentSize);
        }
        void incrementSize(std::size_t amount)
        {
            currentSize += amount;
        }
};

void tmdb::protocol::recvMessage(std::string& message)
{
    std::size_t dataRead = 0;
    message.clear();

    while(true)
    {
        // This outer loop handles resizing of the message when we run of space in the string.
        StringSizer        stringSizer(message, dataRead);
        std::size_t const  dataMax  = message.capacity() - 1;
        char*              buffer   = &message[0];

        std::size_t got = socket_.message_get(buffer + dataRead, dataMax - dataRead);
        dataRead    += got;
        if (got == 0)
        {
            break;
        }

        // Resize the string buffer
        // So that next time around we can read more data.
        message.reserve(message.capacity() * 1.5 + 10);
    }
}