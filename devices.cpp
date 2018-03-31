#include "devices.hpp"
#include "serializer.hpp"


namespace tmdb
{
 serializer*  device::is = new serializer(32);
  
  device::device(const char* id): device_id_(std::string(id))
  {
    
  }



  void test_device1::serialize(const data& unit)
  {
    is->sign_block(device_id_.c_str());
    is->serialize<int>(unit.x_);
    is->serialize<int>(unit.y_);
    is->serialize<float>(unit.val_);
    is->serialize<double>(unit.sp_);
    is->serialize_cstring(unit.descr_.c_str());
    is->serialize<char>(unit.mode_);
    is->finalize_block();
  }

  void test_device1::deserialize(void* mem, data& unit)
  {
    serializer os(static_cast<char*>(mem));
  }



  void test_device2::serialize(const data& unit)
  {
    is->sign_block(device_id_.c_str());
    
    is->finalize_block();
  }

  void test_device2::deserialize(void* mem, data& unit)
  {
    serializer os(static_cast<char*>(mem));
  }

}
