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
		unsigned long long system_memory_size_;

	public:
		serializer_test();
		void ctor_test();


	};

};

#endif