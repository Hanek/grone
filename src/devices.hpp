/* this file is auto-generated with devgen */
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <iostream>
#include <fstream>
#include <string.h>
#include <map>

#ifndef _DEVICES_H
#define _DEVICES_H

namespace grone
{
    class serializer;

    class device
    {
    public:
        static serializer* ins;
        std::string device_id_;
        device(const char* id): device_id_(id) {}
        virtual ~device() {}

        virtual void* get_data()                                  = 0;
        virtual void print_data(std::ostream& os = std::cout)     = 0;
        virtual void serialize_sync()                             = 0;
        virtual void deserialize_sync(unsigned char*)             = 0;
        virtual void serialize(unsigned char*)                    = 0;
        virtual void deserialize(unsigned char*, unsigned char*)  = 0;
        virtual size_t size(unsigned char*)                       = 0;
    };

    /************************************   test_device1   ************************************/

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
        test_device1(const char* id, unsigned char* mem): device(id) { if(mem) {} }

        void* get_data() { return &data_unit_; }
        void print_data(std::ostream& os = std::cout);
        void serialize_sync();
        void deserialize_sync(unsigned char*);
        void serialize(unsigned char*);
        void deserialize(unsigned char*, unsigned char*);
        size_t size(unsigned char*);
    };

    /************************************   test_device2   ************************************/

    class test_device2: public device
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
        test_device2(const char* id, unsigned char* mem): device(id) { if(mem) {} }

        void* get_data() { return &data_unit_; }
        void print_data(std::ostream& os = std::cout);
        void serialize_sync();
        void deserialize_sync(unsigned char*);
        void serialize(unsigned char*);
        void deserialize(unsigned char*, unsigned char*);
        size_t size(unsigned char*);
    };

    /************************************   test_device3   ************************************/

    class test_device3: public device
    {
    public:
        struct data
        {
            int x_;
            std::string descr_;
            data(): x_(0) {}
        };
        data data_unit_;
        test_device3(const char* id): device(id) {}
        test_device3(const char* id, unsigned char* mem): device(id) { if(mem) {} }

        void* get_data() { return &data_unit_; }
        void print_data(std::ostream& os = std::cout);
        void serialize_sync();
        void deserialize_sync(unsigned char*);
        void serialize(unsigned char*);
        void deserialize(unsigned char*, unsigned char*);
        size_t size(unsigned char*);
    };

    /************************************   factory   ************************************/

    class device_factory
    {
    public:
        /* create empty device, supply data later.. */
        device* create(const char* device_id)
        {
            std::map<std::string,pCreate>::iterator it;
            it = mapCreate_.find(std::string(device_id));
            if(it != mapCreate_.end())
            { return it->second(device_id); }
            return 0;
        }

        /* create device with data */
        device* create_sync(const char* device_id, unsigned char* data)
        {
            std::map<std::string,pCreate_sync>::iterator it;
            it = mapCreate_sync_.find(std::string(device_id));
            if(it != mapCreate_sync_.end())
            { return it->second(device_id, data); }
            return 0;
        }

        template <typename T>
            void register_device(const char* device_id)
            { mapCreate_[device_id] = &instantiate<T>; }

        template <typename T>
            void register_device_sync(const char* device_id)
            { mapCreate_sync_[device_id] = &instantiate_sync<T>; }


    private:
        template <typename T>
            static device* instantiate(const char* id)
            { return new T(id); }

        template <typename T>
            static device* instantiate_sync(const char* id, unsigned char* val)
            { return new T(id, val); }


        typedef device* (*pCreate)(const char*);
        std::map<std::string,pCreate> mapCreate_;
        typedef device* (*pCreate_sync)(const char*, unsigned char*);
        std::map<std::string,pCreate_sync> mapCreate_sync_;
    };
}
#endif
