#include "devices.hpp"
#include "serializer.hpp"


namespace tmdb
{
  serializer*  device::ins = new serializer(33);

  test_device1::test_device1(void* mem)
  {


  }

  void test_device1::serialize()
  {
    std::cout << "serialize1\n";
  }

  void test_device1::deserialize(void* block)
  {
  }

  void test_device1::serialize(void* mem)
  {
    std::cout << "serialize1\n";
    data& unit = *(static_cast<data*>(mem));
    //   ins->sign_block(device_id_.c_str());
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
    data& unit = *(static_cast<data*>(mem));
    serializer exs(static_cast<char*>(block));
    exs.deserialize<int>(&unit.x_);
    exs.deserialize<int>(&unit.y_);
    exs.deserialize<float>(&unit.val_);
    exs.deserialize<double>(&unit.sp_);
    exs.deserialize_cstring(unit.descr_);
    exs.deserialize<char>(&unit.mode_);
  }

  test_device2::test_device2(void* mem)
  {


  }

  void test_device2::serialize()
  {
    std::cout << "serialize2\n";
  }

  void test_device2::deserialize(void* block)
  {
  }


  void test_device2::serialize(void* mem)
  {
    std::cout << "serialize2\n";
    data& unit = *(static_cast<data*>(mem));
    // ins->sign_block(device_id_.c_str());
    ins->serialize<int>(unit.x_);
    ins->serialize<double>(unit.temp_);
    ins->serialize_cstring(unit.descr_.c_str());
    ins->finalize_block();
  }

  void test_device2::deserialize(void* block, void* mem)
  {
    data& unit = *(static_cast<data*>(mem));
    serializer exs(static_cast<char*>(block));
    exs.deserialize<int>(&unit.x_);
    exs.deserialize<double>(&unit.temp_);
    exs.deserialize_cstring(unit.descr_);
  }

}
/**************************************************************************/

