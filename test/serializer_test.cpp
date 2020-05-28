#include <unistd.h>
#include "serializer_test.hpp"

namespace test
{
std::string device1::id_ = std::string("device1_id");
int device1::descr_len_max_ = 256;

void device1::fill_random_values()
{
    var1_ = rand();
    var2_ = rand();
    var3_ = rand();
    var4_ = static_cast <float>(rand()) + 1.0f / static_cast <float>(rand());
    var5_ = static_cast <double>(rand()) + 1.0f / static_cast <float>(rand());
    var6_ = rand() % 256;
    var7_ = rand() % 2;
    var8_ = rand();
    var9_ = rand();
    var10_ = rand();

    int descr_len = rand() % descr_len_max_;
    char str[descr_len + 1] = {0};

    for (int i = 0; i < descr_len; i++)
    { str[i] = (48 + rand() % 64); }

    descr_ = str;
}


void device1::print()
{
    std::cout << var1_ << std::endl
              << var2_ << std::endl
              << var3_ << std::endl
              << var4_ << std::endl
              << var5_ << std::endl
              << var6_ << std::endl
              << var7_ << std::endl
              << var8_ << std::endl
              << var9_ << std::endl
              << var10_ << std::endl
              << descr_ << std::endl
              << "--------------------------------" << std::endl;
}


bool device1::operator == (const device1& rhs) const
{
    return var1_ == rhs.var1_ &&
           var2_ == rhs.var2_ &&
           var3_ == rhs.var3_ &&
           var4_ == rhs.var4_ &&
           var5_ == rhs.var5_ &&
           var6_ == rhs.var6_ &&
           var7_ == rhs.var7_ &&
           var8_ == rhs.var8_ &&
           var9_ == rhs.var9_ &&
           var10_ == rhs.var10_ &&
           descr_ == rhs.descr_;
}







serializer_test::serializer_test()
{
    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    system_memory_size_ = pages * page_size;

    LOG_NONE << "system_memory_size_: " << system_memory_size_;
}


void serializer_test::ctor_test()
{
    unsigned long long size = 0;

    /* test memory allocation */
    while (size < system_memory_size_ / 4)
    {
        grone::serializer ins(size);
        assert(size == ins.get_size());
        LOG_NONE << "ctror_test: " << size << " bytes";

        size = (size) ? size * 2 : 1;
    }

    unsigned char* buf1 = NULL;
    grone::serializer exs1(buf1);
    assert(0 != exs1.get_size());
    LOG_NONE << "ctror_test: " << exs1.get_size() << " bytes";

    unsigned char buf2[64];
    grone::serializer exs2(buf2);
    assert(0 != exs2.get_size());
    LOG_NONE << "ctror_test: " << exs2.get_size() << " bytes";

    unsigned char* buf3 = NULL;
    grone::serializer exs3(buf3, 32);
    assert(0 != exs3.get_size());
    LOG_NONE << "ctror_test: " << exs3.get_size() << " bytes";

    unsigned char buf4[64];
    grone::serializer exs4(buf4, 64);
    assert(0 != exs4.get_size());
    LOG_NONE << "ctror_test: " << exs4.get_size() << " bytes";
}


void serializer_test::serialization_test1()
{
    size_t size = 4;

    srand(time(NULL));

    grone::serializer ins(size);
    device1 dev1;


    dev1.fill_random_values();

    ins.sign_block(dev1.id_.c_str());
    ins.serialize<int>(dev1.var1_);
    ins.serialize<size_t>(dev1.var2_);
    ins.serialize<long>(dev1.var3_);
    ins.serialize<float>(dev1.var4_);
    ins.serialize<double>(dev1.var5_);
    ins.serialize<char>(dev1.var6_);
    ins.serialize<bool>(dev1.var7_);
    ins.serialize<unsigned char>(dev1.var8_);
    ins.serialize<unsigned int>(dev1.var9_);
    ins.serialize<unsigned long long>(dev1.var10_);
    ins.serialize(dev1.descr_);
    ins.finalize_block();


    ins.dump();
    ins.reset();
    device1 dev1_ds;


    char id_ds[grone::serializer::dev_id_max()] = {0};
    ins.read_block(id_ds);
    ins.deserialize<int>(&dev1_ds.var1_);
    ins.deserialize<size_t>(&dev1_ds.var2_);
    ins.deserialize<long>(&dev1_ds.var3_);
    ins.deserialize<float>(&dev1_ds.var4_);
    ins.deserialize<double>(&dev1_ds.var5_);
    ins.deserialize<char>(&dev1_ds.var6_);
    ins.deserialize<bool>(&dev1_ds.var7_);
    ins.deserialize<unsigned char>(&dev1_ds.var8_);
    ins.deserialize<unsigned int>(&dev1_ds.var9_);
    ins.deserialize<unsigned long long>(&dev1_ds.var10_);
    ins.deserialize(dev1_ds.descr_);

    assert(dev1.id_ == std::string(id_ds));
    assert(dev1 == dev1_ds);
}

};