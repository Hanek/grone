#include <unistd.h>
#include <iostream>
#include <vector>
#include <chrono>

#include <ctpl_stl.h>

#include "server.hpp"


grone::server::server(core& c, size_t pool_size, size_t max_connections): 
core_(c), pool_size_(pool_size), max_connections_(max_connections)
{

    
}
 
grone::server::~server()
{
    
}



void grone::server::worker(int id, grone::provider& socket, grone::server& obj)
{
    grone::protocol server(socket);  
    while(server.is_ready_)
    {
        grone::request req;
        grone::request rep;
        server.recv(req);
        if(!server.is_ready_)
        { break; }
        std::cout << "[" << id << ":" << socket.get_socket_id() <<  "] = type: " 
                         << req.type_ << ", length: " << req.len_ << "\n";
        
        /* handle client request */
        obj.dispatch(req, rep);
        
        server.send(rep);
    } 
    socket.close();
}
 

void grone::server::dispatch(grone::request& req, grone::request& rep)
{
  auto it = core_.dispatchMap_.find(req.type_);
  if(it == core_.dispatchMap_.end())
  { return; }
  
//  it->second(req.val_, rep.val_);
  rep.type_ = req.type_;
  rep.len_  = rep.val_.size();
}


void grone::server::run()
{
    int pool_size = 2;
    
    ctpl::thread_pool pool(pool_size);
    grone::listener listener(8080);
    while(true)
    {
        std::cout << "listening...\n";
        grone::provider accepted = listener.accept();      
        pool.push(grone::server::worker, std::move(accepted), std::move(*this));
    }
}

