#include <unistd.h>
#include <iostream>
#include <vector>
#include <chrono>

#include <ctpl_stl.h>

#include "server.hpp"


tmdb::server::server(size_t pool_size, size_t max_connections): 
pool_size_(pool_size), max_connections_(max_connections)
{

    
}
 
tmdb::server::~server()
{
    
}

//
//std::function<void(void)> f = std::bind(&Foo::doSomething, this);
//If you want to bind a function with parameters, you need to specify placeholders:
//
//using namespace std::placeholders;
//std::function<void(int,int)> f = std::bind(&Foo::doSomethingArgs, this, _1, _2);
//Or, if your compiler supports C++11 lambdas:
//
//std::function<void(int,int)> f = [=](int a, int b) {
//    this->doSomethingArgs(a, b);
//}



void tmdb::server::init()
{    
  dispatchMap_ = {
    {0x01, [=](const std::string& a, std::string& b) { core_.device_list(a, b); } },
    {0x02,  [=](const std::string& a, std::string& b) { core_.device_list(a, b); } } };
  
}

void tmdb::server::worker(int id, tmdb::provider& socket, tmdb::server& thisObj)
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
        thisObj.dispatch(req);
        
        char reply[32] = {0};
        sprintf(reply, "ok, says polite #%d", id);
        server.send(reply, 'b');
        std::cout << "[ send: " << socket.get_socket_id() << " ]: " << reply << "\n";
    } 
    
    socket.close();
}


void tmdb::server::dispatch(tmdb::request& req)
{
  auto it = dispatchMap_.find(req.type_);
  if(it == dispatchMap_.end())
  { return; }
  
  std::string response;
  it->second(req.val_, response);
  
  std::cout << "server response: " << response << std::endl;
}


void tmdb::server::run()
{
    int pool_size = 2;
    init();
    tmdb::core c;
    
    ctpl::thread_pool pool(pool_size);
    tmdb::listener listener(8080);
    while(true)
    {
        std::cout << "listening...\n";
        tmdb::provider accepted = listener.accept();
//        std::function<void(void)> f = std::bind(&tmdb::server::worker, this, 0, std::move(accepted), std::move(c)); //, std::move(accepted), std::move(c));
        
        pool.push(tmdb::server::worker, std::move(accepted), std::move(*this));
    }
}

