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
      device() {}
      virtual ~device() {}

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

     data data_unit;
     test_device1() {}
     test_device1(void*);
     
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

      data data_unit;
      test_device2() {}
      test_device2(void*);

      void serialize();
      void deserialize(void*);
      void serialize(void*);
      void deserialize(void*, void*);
  };


  /************************       factory         ***************************/


  class device_factory
  {
    public:
      device* create(const std::string& device_id)
      {
        std::map<std::string,pCreate>::iterator it;
        it = mapCreate_.find(device_id);
        if(it != mapCreate_.end())
        { return it->second(); }
        return 0;
      }

      device* create2(const std::string& device_id, void* data)
      {
        std::map<std::string,pCreate2>::iterator it;
        it = mapCreate2_.find(device_id);
        if(it != mapCreate2_.end())
        { return it->second(data); }
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
        static device* instantiate()
        { return new T; }
      
      template <typename T>
        static device* instantiate2(void* val)
        { return new T(val); }


    private:
      typedef device* (*pCreate)();
      std::map<std::string,pCreate> mapCreate_;
      typedef device* (*pCreate2)(void*);
      std::map<std::string,pCreate2> mapCreate2_;
  };
}
#endif
