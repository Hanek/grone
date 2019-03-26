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
    public:
        virtual ~socket();
        socket(int id);
        /* allow move  */
        socket(socket&& move)              noexcept;
        socket& operator = (socket&& move) noexcept;
        void swap(socket& other)           noexcept;
        /* refuse copy */
        socket(const socket&)              = delete;
        socket& operator=(const socket&)   = delete;
        /* allow clone */
        socket& clone(const socket&);
        int get_socket_id() const { return socket_id_; }
        void close();
        
        
    };


    class provider: public socket
    {
    public:
        enum class state { connected, eof, error, undef };
        state state_;
        
        provider(): socket(is_invalid_), state_(state::undef) { std::cout << "provider empty\n"; }
        provider(int id): socket(id), state_(state::connected) { std::cout << "provider\n"; }
        bool read(unsigned char* buffer, std::size_t size);
        bool read(std::vector<unsigned char>& msg);
        template <class T> bool read(T* var) { return read(reinterpret_cast<unsigned char*>(var), sizeof(T)); }
        bool write(const char* buffer, std::size_t size);
        bool write(const std::vector<unsigned char>& msg);
        bool write(const std::string& msg);
        template <class T> bool write(T var) { return write((const char*)&var, sizeof(T)); }
        bool close();
        
        
    };
    
    class connector: public provider
    {
    public:
        connector(const std::string& host, int port);
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


