#include <iostream>
#include <string>
#include <sstream>

#include <plog/Log.h>
#include "serializer_test.hpp"



int main()
{
    plog::init(plog::debug, "test.log");
    LOG_NONE << "---==[ autotests running... ]==---";

    test::serializer_test st;
    // st.ctor_test();
    st.serialization_test1();


}