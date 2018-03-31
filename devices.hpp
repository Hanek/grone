#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <iostream>
#include <fstream>
#include <string.h>


#ifndef _DEVICES_H
#define _DEVICES_H

namespace tmdb
{
  class serializer;

  class device
  {
    public:
      struct data {};
      std::string device_id_;
      static serializer* is;
      device(const char* id);
      virtual void serialize(const data&)          = 0;
      virtual void deserialize(const void*, data&) = 0;

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
     };

      test_device1(const char* id): device(id) {}
      void serialize(const data&);
      void deserialize(void*, data&);
  };


  struct test_device2: public device
  {
    public:
      struct data
      {
        int x_;
        double temp_;
        std::string descr_;
      };

      test_device2(const char* id): device(id) {}
      void serialize(const data&);
      void deserialize(void*, data&);
  };

}
#endif
