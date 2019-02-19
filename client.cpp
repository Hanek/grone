#include <unistd.h>
#include "client.hpp"
#include "socket.hpp"
#include "protocol.hpp"


int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "usage: client <host> <Message>\n";
        std::exit(1);
    }

    tmdb::connector socket(argv[1], 8080);
    tmdb::protocol client(socket);
    while(true)
    {
        client.send("hello pizdabolskoe otrod'je..", 'a');

        char type;
        std::string message;
        client.recv(message, type);
        std::cout << "receive: " << message << "\n";
        sleep(3);
    }
}
