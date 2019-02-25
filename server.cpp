#include <unistd.h>
#include <iostream>
#include <vector>
#include <chrono>

#include "server.hpp"
#include "protocol.hpp"


tmdb::server::server(size_t pool_size, size_t max_connections): 
pool_size_(pool_size), max_connections_(max_connections), pool_(pool_size)
{

    
}
 
tmdb::server::~server()
{
    
}

bool foo(tmdb::provider* socket, int i)
{
    std::cout << "--- worker thread #" << i << " is running.. ---\n";
    tmdb::protocol server(*socket);  
    while(server.is_ready_)
    {
        char type;
        std::string message;
        server.recv(message, type);
        if(!server.is_ready_)
        { break; }
        std::cout << "[ recv: " << socket->get_socket_id() << " ]: " << message << "\n";

        char reply[32] = {0};
        sprintf(reply, "ok, #%d",i );
        server.send(reply, 'b');
        std::cout << "[ send: " << socket->get_socket_id() << " ]: " << reply << "\n";
    } 
    
    socket->close();
    return true;
}


void tmdb::server::run()
{
    int pool_size = 2;
    ThreadPool pool(pool_size);
    tmdb::provider socket[pool_size*2];
    tmdb::listener listener(8080);
    
    std::cout << "=== " << sizeof(socket) << " ===\n";
    
    
    int i = 0;
    while(true)
    {
        tmdb::provider accepted = listener.accept();
        
        accepted.swap(socket[i]);
        pool.enqueue(foo, &socket[i], i);
        
        i++;
                
    }
}
