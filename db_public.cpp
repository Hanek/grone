#include "db_core.hpp"
#include "time.hpp"
#include "serializer.hpp"
#include "devices.hpp"

namespace tmdb
{

 void core::get(tmdb::request& req, tmdb::request& resp)
  {
//    tmdb::device* pDev = 0;
//    std::vector<unsigned char> buf;
//    pDev = factory_->create(device_id.c_str());
//    auto it = dmap_.find(device_id);
//    if(!pDev || dmap_.end() == it)
//    {
////      resp.push_back("no device found: ");// + device_id;
//      return;
//    }
//    
//    std::vector<unsigned char>::iterator ot = buf.begin();
//    for(auto vit = it->second.begin(); vit != it->second.end(); vit++)
//    {
//      /*
//       * vit->second is a timestamp and vit->first is a pointer to raw data,
//       * that's what going to be send to client endpoint, just printing out at the current stage
//       */
////      resp += "[ " + vit->second.get_date() + "\t";
//      /* use as external buffer */
//      size_t dev_size = pDev->size(vit->first);
//      std::cout << "dev_size: " << dev_size << std::endl;
//       
//      resp.insert(ot, vit->first, vit->first + dev_size);
//      ot = resp.end();
////      resp.
////      pDev->deserialize_sync(vit->first);
////      pDev->print_data(ss);
//    }
//    delete pDev;

  }
 
 
 void core::device_list(tmdb::request& req, tmdb::request& resp)
 {
//     resp = factory_->get_device_list();
 }
  
}


