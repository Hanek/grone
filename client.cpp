#include <unistd.h>
#include "client.hpp"
#include "tcp_server.hpp"
#include "tcp_client.hpp"


int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "usage: client <host> <Message>\n";
        std::exit(1);
    }

    tmdb::connector client(argv[1], 8080);
    sleep(600);
}
