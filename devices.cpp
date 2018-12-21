#include "devices.hpp"
#include "serializer.hpp"




namespace tmdb
{
    serializer* device::ins = new serializer(1024);

    /**************************************   test_device1   **************************************/


    void test_device1::print_data()
    {
    std::cout << device_id_ << " : "
        << data_unit_.x_ << " : "
        << data_unit_.y_ << " : "
        << data_unit_.val_ << " : "
        << data_unit_.sp_ << " : "
        << data_unit_.descr_ << " : "
        << data_unit_.mode_ << " : "
        << std::endl;
    }
    
    void test_device1::serialize_sync()
    {
        ins->sign_block(device_id_.c_str());
        ins->serialize<int>(data_unit_.x_);
        ins->serialize<int>(data_unit_.y_);
        ins->serialize<float>(data_unit_.val_);
        ins->serialize<double>(data_unit_.sp_);
        ins->serialize(data_unit_.descr_);
        ins->serialize<char>(data_unit_.mode_);
        ins->finalize_block();
    }

    void test_device1::deserialize_sync(void* block)
    {
        serializer exs(static_cast<char*>(block));
        exs.deserialize<int>(&data_unit_.x_);
        exs.deserialize<int>(&data_unit_.y_);
        exs.deserialize<float>(&data_unit_.val_);
        exs.deserialize<double>(&data_unit_.sp_);
        exs.deserialize(data_unit_.descr_);
        exs.deserialize<char>(&data_unit_.mode_);
    }

    void test_device1::serialize(void* mem)
    {
        data& data_unit = *(static_cast<data*>(mem));
        ins->sign_block(device_id_.c_str());
        ins->serialize<int>(data_unit.x_);
        ins->serialize<int>(data_unit.y_);
        ins->serialize<float>(data_unit.val_);
        ins->serialize<double>(data_unit.sp_);
        ins->serialize(data_unit.descr_);
        ins->serialize<char>(data_unit.mode_);
        ins->finalize_block();
    }

    void test_device1::deserialize(void* block, void* mem)
    {
        data& data_unit = *(static_cast<data*>(mem));
        serializer exs(static_cast<char*>(block));
        exs.deserialize<int>(&data_unit.x_);
        exs.deserialize<int>(&data_unit.y_);
        exs.deserialize<float>(&data_unit.val_);
        exs.deserialize<double>(&data_unit.sp_);
        exs.deserialize(data_unit.descr_);
        exs.deserialize<char>(&data_unit.mode_);
    }


    /**************************************   test_device2   **************************************/


    void test_device2::print_data()
    {
    std::cout << device_id_ << " : "
        << data_unit_.x_ << " : "
        << data_unit_.temp_ << " : "
        << data_unit_.descr_ << " : "
        << std::endl;
    }
    
    void test_device2::serialize_sync()
    {
        ins->sign_block(device_id_.c_str());
        ins->serialize<int>(data_unit_.x_);
        ins->serialize<double>(data_unit_.temp_);
        ins->serialize(data_unit_.descr_);
        ins->finalize_block();
    }

    void test_device2::deserialize_sync(void* block)
    {
        serializer exs(static_cast<char*>(block));
        exs.deserialize<int>(&data_unit_.x_);
        exs.deserialize<double>(&data_unit_.temp_);
        exs.deserialize(data_unit_.descr_);
    }

    void test_device2::serialize(void* mem)
    {
        data& data_unit = *(static_cast<data*>(mem));
        ins->sign_block(device_id_.c_str());
        ins->serialize<int>(data_unit.x_);
        ins->serialize<double>(data_unit.temp_);
        ins->serialize(data_unit.descr_);
        ins->finalize_block();
    }

    void test_device2::deserialize(void* block, void* mem)
    {
        data& data_unit = *(static_cast<data*>(mem));
        serializer exs(static_cast<char*>(block));
        exs.deserialize<int>(&data_unit.x_);
        exs.deserialize<double>(&data_unit.temp_);
        exs.deserialize(data_unit.descr_);
    }


    /**************************************   test_device3   **************************************/


    void test_device3::print_data()
    {
    std::cout << device_id_ << " : "
        << data_unit_.x_ << " : "
        << data_unit_.descr_ << " : "
        << std::endl;
    }
    
    void test_device3::serialize_sync()
    {
        ins->sign_block(device_id_.c_str());
        ins->serialize<int>(data_unit_.x_);
        ins->serialize(data_unit_.descr_);
        ins->finalize_block();
    }

    void test_device3::deserialize_sync(void* block)
    {
        serializer exs(static_cast<char*>(block));
        exs.deserialize<int>(&data_unit_.x_);
        exs.deserialize(data_unit_.descr_);
    }

    void test_device3::serialize(void* mem)
    {
        data& data_unit = *(static_cast<data*>(mem));
        ins->sign_block(device_id_.c_str());
        ins->serialize<int>(data_unit.x_);
        ins->serialize(data_unit.descr_);
        ins->finalize_block();
    }

    void test_device3::deserialize(void* block, void* mem)
    {
        data& data_unit = *(static_cast<data*>(mem));
        serializer exs(static_cast<char*>(block));
        exs.deserialize<int>(&data_unit.x_);
        exs.deserialize(data_unit.descr_);
    }

}
