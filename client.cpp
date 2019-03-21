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
    int i = 0;
    while(i < 20)
    {
        client.send("device1", 0x01);

        char type;
        std::string message;
        client.recv(message, type);
        std::cout << "receive: " << message << "\n";
        sleep(1);
        i++;
    }
}
