#include <cerrno>
#include <iostream>
#include <map>

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
    std::map<std::string,const char> reqMap_;  
  };
}

#endif