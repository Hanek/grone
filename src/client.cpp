#include <unistd.h>
#include "client.hpp"
#include "socket.hpp"
#include "protocol.hpp"
#include "devices.hpp"


int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "usage: client <host> <Message>\n";
        std::exit(1);
    }

    grone::connector socket(argv[1], 8080);
    grone::protocol client(socket);
    grone::request req;
      
    std::string hreq("some request..");
    std::vector<unsigned char> breq;
    req.type_   = 0x01;
    req.outlen_ = hreq.size() + breq.size();
    req.outreq_.push_back(std::tuple<std::string,std::vector<unsigned char> > (hreq, breq));
    
    int i = 0;
    while(i < 20)
    {
        client.send(req);

        client.recv(req);

        sleep(1);
        i++;
    }
}
