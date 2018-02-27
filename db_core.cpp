#include "db_core.hpp"
#include "time.hpp"
#include "serializer.hpp"



namespace tmdb
{
  bool core::instance_ = false;
  std::map<time,void*> core::bmap_;
  std::map<std::string,std::map<time,void*> > core::dmap_;
  
  core::core()
  {
  }
  
  core::~core()
  {
  }
  
  void core::cacheIt(serializer& s)
  {
    time t;
    char str[4096] = {0};
    
    s.reset();
    while(s.get_block(str))
    {
      std::cout << str << std::endl;
      memset(str, 0x00, 4096);
      /* insert current block into map */
    }
    
    
    
    get_time(t);
    /* return allocated memory from serializer */
    void* buf = s.detach_buffer();
    bmap_[t] = buf;
    
    
  }
  
  
  void core::bm_walk()
  {
    for(bm_cit_ cit = bmap_.begin(); cit != bmap_.end(); cit++)
    {
      std::cout << cit->first << std::endl;
    }
  }
  
  
}

