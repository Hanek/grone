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
    sleep(600);
}
