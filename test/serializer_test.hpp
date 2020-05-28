#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <iostream>

#include "../src/serializer.hpp"

#ifndef _SERIALIZER_TEST_H
#define _SERIALIZER_TEST_H


namespace test
{
struct device1
{
    static std::string id_;
    static int descr_len_max_;

    int    var1_;
    size_t var2_;
    long   var3_;
    float  var4_;
    double var5_;
    char   var6_;
    bool   var7_;
    unsigned char var8_;
    unsigned int  var9_;
    unsigned long long var10_;
    std::string descr_;

    void print();
    void fill_random_values();
    bool operator == (const device1& rhs) const;
};


class serializer_test
{
private:
    unsigned long long system_memory_size_;

public:
    serializer_test();
    void ctor_test();
    void serialization_test1();


};

};

#endif