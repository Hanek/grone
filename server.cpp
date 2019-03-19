#include <unistd.h>
#include <iostream>
#include <vector>
#include <chrono>

#include <ctpl_stl.h>

#include "server.hpp"
#include "protocol.hpp"


tmdb::server::server(size_t pool_size, size_t max_connections): 
pool_size_(pool_size), max_connections_(max_connections)
{

    
}
 
tmdb::server::~server()
{
    
}


void tmdb::server::worker(int id, tmdb::provider& socket)
{
    tmdb::protocol server(socket);  
    while(server.is_ready_)
    {
        tmdb::request req;
        server.recv(req);
        if(!server.is_ready_)
        { break; }
        std::cout << "[ recv: " << socket.get_socket_id() << " ]: " << req.val_ << "\n";
        
        /* handle client request */
        
        char reply[32] = {0};
        sprintf(reply, "ok, says polite #%d", id);
        server.send(reply, 'b');
        std::cout << "[ send: " << socket.get_socket_id() << " ]: " << reply << "\n";
    } 
    
    socket.close();
}





void tmdb::server::run()
{
    int pool_size = 2;

    ctpl::thread_pool pool(pool_size);
    tmdb::listener listener(8080);
    while(true)
    {
        std::cout << "listening...\n";
        tmdb::provider accepted = listener.accept();

        pool.push(tmdb::server::worker, std::move(accepted));
    }
}

