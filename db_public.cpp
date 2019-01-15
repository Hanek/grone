#include "db_core.hpp"
#include "time.hpp"
#include "serializer.hpp"
#include "devices.hpp"

namespace tmdb
{

 void core::get(const std::string& device_id, std::string& resp)
  {
    tmdb::device* pDev = 0;
    pDev = factory_->create(device_id.c_str());
    auto it = dmap_.find(device_id);
    if(!pDev || dmap_.end() == it)
    {
      resp = "no device found: " + device_id;
      return;
    }
    
    std::stringstream ss;
    

    for(auto vit = it->second.begin(); vit != it->second.end(); vit++)
    {
      /*
       * vit->second is a timestamp and vit->first is a pointer to raw data,
       * that's what going to be send to frontend, just printing out at the current stage
       */
      resp += "[ " + vit->second.get_date() + "\t";
      /* use as external buffer */
      pDev->deserialize_sync(vit->first);
      pDev->print_data(ss);
      resp += ss.str();
      ss.str("");
      resp += " ]";
    }
    delete pDev;

  }
 
}
