#include <cerrno>
#include <iostream>

#ifndef _DB_CLIENT_H
#define _DB_CLIENT_H

namespace tmdb
{
  class time;
  class serializer;
  
  class client 
  {
  public:
    client();
    ~client();
  };
}

#endif