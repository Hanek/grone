#include <cerrno>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#ifndef _DB_SOCKET_H
#define _DB_SOCKET_H

namespace tmdb
{
  class time;
  class serializer;


    class socket
    {
    private:
        int socket_id_;
        int errno_;
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
        socket(socket&& move)              noexcept;
        socket& operator = (socket&& move) noexcept;
        void swap(socket& other)           noexcept;
        socket(socket const&)              = delete;
        socket& operator=(socket const&)   = delete;
        void close();
    };


    class provider: public socket
    {
    public:
        provider(int id): socket(id) { std::cout << "provider\n"; }
        bool read(char* buffer, std::size_t size);
        template <class T> bool read(T* var) { return read((char*)var, sizeof(T)); }
        bool write(const char* buffer, std::size_t size);
        bool write(const std::string& msg);
        template <class T> bool write(T var) { return write((const char*)&var, sizeof(T)); }
        bool close();
    };
    
    class connector: public provider
    {
    public:
        connector(std::string const& host, int port);
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


