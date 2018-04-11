#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <iostream>
#include <fstream>
#include <string.h>
#include <map>

#ifndef _DEVICES_H
#define _DEVICES_H

namespace tmdb
{
  class serializer;

  class device
  {
    public:
      static serializer* ins;
      std::string device_id_;
      device(const char* id): device_id_(id) {}
      virtual ~device() {}

      virtual void* get_data()               = 0;
      virtual void print_data()              = 0;
      virtual void serialize()               = 0;
      virtual void deserialize(void*)        = 0;
      virtual void serialize(void*)          = 0;
      virtual void deserialize(void*, void*) = 0;
  };

  /************************* user's classes ****************************/

  class test_device1: public device
  {
    public:
     struct data
     {
       int x_;
       int y_;
       float val_;
       double sp_;
       std::string descr_;
       char mode_;
       data(): x_(0), y_(0), val_(0), sp_(0), mode_(0) {}
     };

     data data_unit_;
     test_device1(const char* id): device(id) {}
     test_device1(const char*, void*);
     
     void* get_data() { return &data_unit_; }
     void print_data();
     void serialize();
     void deserialize(void*);
     void serialize(void*);
     void deserialize(void*, void*);
  };


  struct test_device2: public device
  {
    public:
      struct data
      {
        int x_;
        double temp_;
        std::string descr_;
        data(): x_(0), temp_(0) {}
      };

      data data_unit_;
      test_device2(const char* id): device(id) {}
      test_device2(const char*, void*);

      void* get_data() { return &data_unit_; }
      void print_data();
      void serialize();
      void deserialize(void*);
      void serialize(void*);
      void deserialize(void*, void*);
  };


  /************************       factory         ***************************/


  class device_factory
  {
    public:
      device* create(const char* device_id)
      {
        std::map<std::string,pCreate>::iterator it;
        it = mapCreate_.find(std::string(device_id));
        if(it != mapCreate_.end())
        { return it->second(device_id); }
        return 0;
      }

      device* create2(const char* device_id, void* data)
      {
        std::map<std::string,pCreate2>::iterator it;
        it = mapCreate2_.find(std::string(device_id));
        if(it != mapCreate2_.end())
        { return it->second(device_id, data); }
        return 0;
      }

      template <typename T>
        void register_device(const char* device_id)
        { mapCreate_[device_id] = &instantiate<T>; }

      template <typename T>
        void register_device2(const char* device_id)
        { mapCreate2_[device_id] = &instantiate2<T>; }


    private:
      template <typename T>
        static device* instantiate(const char* id)
        { return new T(id); }
      
      template <typename T>
        static device* instantiate2(const char* id,void* val)
        { return new T(id, val); }


    private:
      typedef device* (*pCreate)(const char*);
      std::map<std::string,pCreate> mapCreate_;
      typedef device* (*pCreate2)(const char*, void*);
      std::map<std::string,pCreate2> mapCreate2_;
  };
}
#endif
