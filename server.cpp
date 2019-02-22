#include <unistd.h>
#include <iostream>
#include <vector>
#include <chrono>

#include "server.hpp"
#include "socket.hpp"
#include "protocol.hpp"
#include "thread_pool.hpp"


tmdb::server::server()
{
    
    
}
 
tmdb::server::~server()
{
    
}

bool foo(tmdb::provider* socket, int i)
{
    bool res;
    std::cout << "--- sleeping... ---\n";
    tmdb::protocol server(*socket);  
    std::this_thread::sleep_for(std::chrono::seconds(1));
    while(true)
    {
        char type;
        std::string message;
        res = server.recv(message, type);
        std::cout << "------------- recv: " << res << " -----------\n";
        std::cout << "[" << socket->get_socket_id() << "]: " << message << "\n";

        char reply[32] = {0};
        sprintf(reply, "ok, #%d",i );
        res = server.send(reply, 'b');
        std::cout << "------------- send: " << res << " -----------\n";
    } 
    
    socket->close();
    return true;
}


void tmdb::server::run()
{
    int pool_size = 4;
    ThreadPool pool(pool_size);
    tmdb::provider socket[pool_size];
    tmdb::listener listener(8080);
    
    
    int i = 0;
    while(true)
    {
        tmdb::provider accepted = listener.accept();
        
        accepted.swap(socket[i]);
        auto result = pool.enqueue(foo, &socket[i], i);
        
        i++;
//        result.get();
        std::cout << "=== here ===\n";
        
        
    }
}
