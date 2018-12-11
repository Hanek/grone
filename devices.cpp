#include "devices.hpp"
#include "serializer.hpp"


namespace tmdb
{
  serializer*  device::ins = new serializer(1024);

  test_device1::test_device1(const char* id, void* mem): device(id)
  {
      if(mem){}

  }

  void test_device1::print_data()
  {
    std::cout << device_id_ << " : "
      << data_unit_.x_ << " : "
      << data_unit_.y_ << " : "
      << data_unit_.val_ << " : "
      << data_unit_.sp_ << " : "
      << data_unit_.descr_ << " : "
      << data_unit_.mode_ << std::endl;;

  }

  void test_device1::serialize_sync()
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl; 
    ins->sign_block(device_id_.c_str());
    ins->serialize<int>(data_unit_.x_);
    ins->serialize<int>(data_unit_.y_);
    ins->serialize<float>(data_unit_.val_);
    ins->serialize<double>(data_unit_.sp_);
    ins->serialize_cstring(data_unit_.descr_.c_str());
    ins->serialize<char>(data_unit_.mode_);
    ins->finalize_block();
  }

  void test_device1::deserialize_sync(void* block)
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl; 
    serializer exs(static_cast<char*>(block));
    exs.deserialize<int>(&data_unit_.x_);
    exs.deserialize<int>(&data_unit_.y_);
    exs.deserialize<float>(&data_unit_.val_);
    exs.deserialize<double>(&data_unit_.sp_);
    exs.deserialize_cstring(data_unit_.descr_);
    exs.deserialize<char>(&data_unit_.mode_);
  }

  void test_device1::serialize(void* mem)
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl; 
    data& unit = *(static_cast<data*>(mem));
    ins->sign_block(device_id_.c_str());
    ins->serialize<int>(unit.x_);
    ins->serialize<int>(unit.y_);
    ins->serialize<float>(unit.val_);
    ins->serialize<double>(unit.sp_);
    ins->serialize_cstring(unit.descr_.c_str());
    ins->serialize<char>(unit.mode_);
    ins->finalize_block();

  }

  void test_device1::deserialize(void* block, void* mem)
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl; 
    data& unit = *(static_cast<data*>(mem));
    serializer exs(static_cast<char*>(block));
    exs.deserialize<int>(&unit.x_);
    exs.deserialize<int>(&unit.y_);
    exs.deserialize<float>(&unit.val_);
    exs.deserialize<double>(&unit.sp_);
    exs.deserialize_cstring(unit.descr_);
    exs.deserialize<char>(&unit.mode_);
  }

  test_device2::test_device2(const char* id, void* mem): device(id)
  {
    if(mem){}

  }

  void test_device2::print_data()
  {
    std::cout << device_id_ << " : "
      << data_unit_.x_ << " : "
      << data_unit_.temp_ << " : "
      << data_unit_.descr_ << std::endl;
  }


  void test_device2::serialize_sync()
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl; 
    ins->sign_block(device_id_.c_str());
    ins->serialize<int>(data_unit_.x_);
    ins->serialize<double>(data_unit_.temp_);
    ins->serialize_cstring(data_unit_.descr_.c_str());
    ins->finalize_block();
 }

  void test_device2::deserialize_sync(void* block)
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl; 
    serializer exs(static_cast<char*>(block));
    exs.deserialize<int>(&data_unit_.x_);
    exs.deserialize<double>(&data_unit_.temp_);
    exs.deserialize_cstring(data_unit_.descr_);
  }


  void test_device2::serialize(void* mem)
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl; 
    data& unit = *(static_cast<data*>(mem));
    ins->sign_block(device_id_.c_str());
    ins->serialize<int>(unit.x_);
    ins->serialize<double>(unit.temp_);
    ins->serialize_cstring(unit.descr_.c_str());
    ins->finalize_block();
  }

  void test_device2::deserialize(void* block, void* mem)
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl; 
    data& unit = *(static_cast<data*>(mem));
    serializer exs(static_cast<char*>(block));
    exs.deserialize<int>(&unit.x_);
    exs.deserialize<double>(&unit.temp_);
    exs.deserialize_cstring(unit.descr_);
  }

}
/**************************************************************************/

