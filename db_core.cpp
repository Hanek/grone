#include "db_core.hpp"
#include "time.hpp"
#include "serializer.hpp"
#include "devices.hpp"

namespace tmdb
{
  bool core::instance_ = false;
  std::map<time,std::pair<unsigned char*,int> > core::bmap_;
  std::map<std::string,std::vector<std::pair<unsigned char*,time> > >core::dmap_;
  
  core::core()
  {
    /* create empty factory */
    factory_ = new device_factory();
    /* register devices */
    factory_->register_device<test_device1>("device1");
    factory_->register_device<test_device2>("device2");
  }
  
  core::~core()
  {
    for(auto cit = bmap_.begin(); cit != bmap_.end(); cit++)
    {
      free(cit->second.first);
    }
 
  }
  
  void core::init()
  {
    dispatchMap_ = 
    {
      {0x01, [=](tmdb::request& req, tmdb::request& rep) { this->get(req, rep); } },
      {0x02,  [=](tmdb::request& req, tmdb::request& rep) { this->device_list(req, rep); } } 
    };
      
  }
  
  
  void core::cacheIt() 
  {
    time t;
    get_time(t);
    char str[serializer::dev_id_max()] = {0};
    
    serializer* s = device::ins;
    /* 
     * called prior to detach, so we detaching exactly the same 
     * amount of bytes that we have serialized..
     */
    s->shrink_to_fit();
    s->reset();

    /* get_block() return pointer to block and copy device_id to str */ 
    for(unsigned char* block = s->get_block(str); block; block = s->get_block(str))
    {
      /* device_id of current block */
      std::string device_id(str);
      memset(str, 0x00, serializer::dev_id_max());
      std::cout << device_id << std::endl;
     /* block is a pointer to device data */
      std::pair<unsigned char*,time> device = std::make_pair(block, t);
      
      auto it = dmap_.find(device_id);
      if(it != dmap_.end())
      {/* update exsisting, by pushing we always keep data series in order */
        it->second.push_back(device);
      }
      else
      {/* insert new device in the map */
        std::vector<std::pair<unsigned char*,time> >v_dev;
        v_dev.push_back(device);
        dmap_.insert(std::pair<std::string,std::vector<std::pair<unsigned char*,time> > >(device_id, v_dev));
      }
    }
    
      
    
    /* detach and insert into bare map */
    int buffer_len = s->length();
    unsigned char* buf = s->detach_buffer();
    bmap_[t] = std::make_pair(buf, buffer_len);
  }

  void core::uncacheIt()
  {
    char str[serializer::dev_id_max()] = {0};

    for(auto rit = bmap_.rbegin(); rit != bmap_.rend(); rit++)
    {
      std::cout << __PRETTY_FUNCTION__ << ":time: " <<  rit->first.get_date() << std::endl;
      /* parse memory and remove corresponding devices in dmap_ */
      serializer s(rit->second.first, rit->second.second);
      for(unsigned char* block = s.get_block(str); block; block = s.get_block(str))
      {
        /* device_id of current block */
        std::string device_id(str);
        memset(str, 0x00, serializer::dev_id_max());

        auto it = dmap_.find(device_id);
        if(it != dmap_.end())
        { 
          std::cout << device_id << " is found" << std::endl;
          it->second.pop_back();
          if(it->second.empty())
          {
            std::cout << "empty..." << std::endl;
          }
        }
        else
        {}
      }
    }

    std::cout << "---------------------------\n";
  }


  void core::bm_walk()
  {
    std::cout << "==================== bm_walk ====================\n"; 
    for(auto cit = bmap_.begin(); cit != bmap_.end(); cit++)
    {
      std::cout << cit->first << std::endl;
    }
    std::cout << "================= bm_walk_end ===================\n"; 
  }  
  
  void core::dm_walk()
  {
    tmdb::device* pDev = 0;   
    std::cout << "==================== dm_walk ====================\n";
    
    for(auto cit = dmap_.begin(); cit != dmap_.end(); cit++)
    {
      /* device_id */
      std::cout << "device_id: " << cit->first << std::endl;
      pDev = factory_->create(cit->first.c_str());
      std::vector<std::pair<unsigned char*,time> >:: const_iterator vit;
      for(vit = cit->second.begin(); vit != cit->second.end(); vit++)
      {
        std::cout << "-time: " << vit->second.get_date() << "\t";
        /* use as external buffer */
        pDev->deserialize_sync(vit->first);
        pDev->print_data();
        std::cout << std::endl;
      }
      delete pDev;
    }
    std::cout << "================== dm_walk_end ==================\n"; 
  }

  void core::fetch_device_data(const std::string& device_id)
  {
    tmdb::device* pDev = 0;
    pDev = factory_->create(device_id.c_str());
    auto cit = dmap_.find(device_id);
    if(!pDev || dmap_.end() == cit)
    {
      std::cout << "no device found: " << device_id << std::endl;
      return;
    }

    for(auto vit = cit->second.begin(); vit != cit->second.end(); vit++)
    {
      std::cout << "--time: " << vit->second.get_date() << "\t";
      /* use as external buffer */
      pDev->deserialize_sync(vit->first);
      pDev->print_data();
      std::cout << std::endl;
    }
    delete pDev;

  }


}
