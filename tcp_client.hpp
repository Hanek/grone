#include <cerrno>
#include <iostream>

#ifndef _DB_TCP_CLIENT_H
#define _DB_TCP_CLIENT_H

namespace tmdb
{
  class time;
  class serializer;
  
  class tcp_client 
  {
  public:
    tcp_client();
    ~tcp_client();
  };
}

#endif