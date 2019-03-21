#include <unistd.h>
#include <iostream>
#include <vector>
#include <chrono>

#include <ctpl_stl.h>

#include "server.hpp"


tmdb::server::server(core& c, size_t pool_size, size_t max_connections): 
core_(c), pool_size_(pool_size), max_connections_(max_connections)
{

    
}
 
tmdb::server::~server()
{
    
}



void tmdb::server::worker(int id, tmdb::provider& socket, tmdb::server& thisObj)
{
    tmdb::protocol server(socket);  
    while(server.is_ready_)
    {
        tmdb::request req;
        tmdb::request rep;
        server.recv(req);
        if(!server.is_ready_)
        { break; }
        std::cout << "[" << id << ":" << socket.get_socket_id() <<  "] = " << req.val_ << "\n";
        
        /* handle client request */
        thisObj.dispatch(req, rep);
        
        server.send(rep);
    } 
    socket.close();
}


void tmdb::server::dispatch(tmdb::request& req, tmdb::request& rep)
{
  auto it = core_.dispatchMap_.find(req.type_);
  if(it == core_.dispatchMap_.end())
  { return; }
  
  it->second(req.val_, rep.val_);
  rep.type_ = req.type_;
  rep.len_  = rep.val_.size();
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
        pool.push(tmdb::server::worker, std::move(accepted), std::move(*this));
    }
}

