#include "db_core.hpp"
#include "time.hpp"
#include "serializer.hpp"



namespace tmdb
{
  bool core::instance_ = false;
  std::map<time,void*> core::bmap_;
  std::map<std::string,std::vector<std::pair<void*,time> > >core::dmap_;
  
  core::core()
  {
  }
  
  core::~core()
  {
  }
  
  void core::cacheIt(serializer& s)
  {
    time t;
    get_time(t);
    char str[4096] = {0};
    
    s.reset();
    
    for(void* block = s.get_block(str); block; block = s.get_block(str))
    {
      std::string device_id(str);
      memset(str, 0x00, 4096);
      std::cout << device_id << std::endl; 
      std::pair<void*,time> device = std::make_pair(block, t);
      
      dm_it_ it = dmap_.find(device_id);
      if(it != dmap_.end())
      {/* update */
        it->second.push_back(device);
      }
      else
      {/* insert */
        std::vector<std::pair<void*,time> >v_dev;
        v_dev.push_back(device);
        dmap_.insert(std::pair<std::string,std::vector<std::pair<void*,time> > >(device_id, v_dev));
      }
    }
    
      
    
    /* detach and insert into bare map */
    void* buf = s.detach_buffer();
    bmap_[t] = buf;
    
    
  }
  
  
  void core::bm_walk()
  {
    std::cout << "==================== bm_walk ====================\n"; 
    for(bm_cit_ cit = bmap_.begin(); cit != bmap_.end(); cit++)
    {
      std::cout << cit->first << std::endl;
    }
  }
  
  void core::dm_walk()
  {
    std::cout << "==================== dm_walk ====================\n";
    for(dm_cit_ cit = dmap_.begin(); cit != dmap_.end(); cit++)
    {
      std::vector<std::pair<void*,time> >:: const_iterator vit;
      std::cout << cit->first << std::endl;
      for(vit = cit->second.begin(); vit != cit->second.end(); vit++)
      {
        int a = 0;
        char c = 0;
        serializer s(8);
        s.deserialize_ext<int>((char*)vit->first, &a);
        s.deserialize_ext<char>((char*)vit->first + sizeof(a), &c);
        std::cout << "\t" << vit->second << ": " << a << ": " << c << std::endl;
      }
    }
  }
  
  
}
