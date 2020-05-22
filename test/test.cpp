#include <iostream>
#include <string>
#include <sstream>

#include <plog/Log.h>
#include "serializer_test.hpp"



int main()
{
    plog::init(plog::error, "test.log");
    LOG_NONE << "autotests running... ";

    test::serializer_test st;
    st.ctor_test();


}