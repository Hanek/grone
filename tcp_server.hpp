#include <cerrno>
#include <iostream>

#ifndef _DB_TCP_SERVER_H
#define _DB_TCP_SERVER_H

namespace tmdb
{
  class time;
  class serializer;
  
  class tcp_server 
  {
  public:
    tcp_server();
    ~tcp_server();
  };
}



#endif


