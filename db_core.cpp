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
    for(bm_cit_ cit = bmap_.begin(); cit != bmap_.end(); cit++)
    {
      free(cit->second);
      std::cout << cit->first << std::endl;
    }
 
  }
  
  void core::cacheIt(serializer& s)
  {
    time t;
    get_time(t);
    char str[4096] = {0};
    
    s.reset();
   
    /* get_block() return pointer to block and copy device_id to str */ 
    for(void* block = s.get_block(str); block; block = s.get_block(str))
    {
      /* device_id of current block */
      std::string device_id(str);
      memset(str, 0x00, 4096);
      std::cout << device_id << std::endl;
     /* block is a pointer to device data */
      std::pair<void*,time> device = std::make_pair(block, t);
      
      dm_it_ it = dmap_.find(device_id);
      if(it != dmap_.end())
      {/* update exsisting, by pushing we always keep data series in order */
        it->second.push_back(device);
      }
      else
      {/* insert new device in the map */
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
    char buf[256] = {0};
    std::cout << "==================== dm_walk ====================\n";
    for(dm_cit_ cit = dmap_.begin(); cit != dmap_.end(); cit++)
    {
      std::vector<std::pair<void*,time> >:: const_iterator vit;
      std::cout << cit->first << std::endl;
      for(vit = cit->second.begin(); vit != cit->second.end(); vit++)
      {
        /* callback to device serialization routine */
        int a;
        char c = 0;
        float f;
        /* use as external buffer */
        serializer s((char*)vit->first);
        memset(buf, 0x00, sizeof(buf));

        s.deserialize<int>(&a);
        s.deserialize<char>(&c);
        s.deserialize_cstring(buf);
        s.deserialize<float>(&f);

        std::cout << "\t" << vit->second 
                  << " : " << a 
                  << " : " << c 
                  << " : " << buf 
                  << " : " << f << std::endl;
      }
    }
  }
  
  
}
