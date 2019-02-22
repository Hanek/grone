#include <cerrno>
#include <iostream>

#ifndef _SERVER_H
#define _SERVER_H

namespace tmdb
{
  class time;
  class serializer;
  
  class server
  {
  public:
    server();
    ~server();
    void run();
  };
}

#endif