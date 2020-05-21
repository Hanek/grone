#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <iostream>

#include "../src/serializer.hpp"

#ifndef _SERIALIZER_TEST_H
#define _SERIALIZER_TEST_H  


namespace test
{
	class serializer_test 
	{
	private:
		grone::serializer s_;
	public:
		void test1();


	};

};

#endif