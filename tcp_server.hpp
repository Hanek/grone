#include <cerrno>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#ifndef _DB_TCP_SERVER_H
#define _DB_TCP_SERVER_H

namespace tmdb
{
  class time;
  class serializer;


    class socket
    {
        int socket_id_;
    protected:
        static constexpr int is_invalid_ = -1;

        int get_socket_id() const
        {
            return socket_id_;
        }
    public:
        virtual ~socket();
        socket(int id);
        /* allow move,  no copies */
        socket(socket&& move) noexcept;
        socket& operator = (socket&& move) noexcept;
        void swap(socket& other) noexcept;
        socket(socket const&) = delete;
        socket& operator=(socket const&) = delete;
        void close();
    };

    class provider: public socket
    {
    public:
        provider(int id): socket(id) {}
        template<typename F>
        std::size_t message_get(char* buffer, std::size_t size, F scanForEnd = [](std::size_t) { return false; });
        void message_put(char const* buffer, std::size_t size);
        void message_put_close();
    };
    
    class listener: public socket
    {
        static constexpr int max_connection_ = 5;
    public:
        listener(int port);
        provider accept();
    };
}
  

#endif


