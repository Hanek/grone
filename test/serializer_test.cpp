#include <unistd.h>
#include "serializer_test.hpp"

namespace test
{

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

};