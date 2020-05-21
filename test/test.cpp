#include <iostream>
#include <string>
#include <sstream>

#include <plog/Log.h> 



int main() 
{
  plog::init(plog::error, "tmdb.log");
  LOG_NONE << "autotests running... ";
  
}