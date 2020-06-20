// Copyright (c) 2018-2020 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>
#if defined(BOOST_LEAF_NO_EXCEPTIONS)

#include <iostream>

int main()
{
	std::cout << "Unit test not applicable." << std::endl;
	return 0;
}

#else

#include <boost/leaf/boost_exception_support.hpp>
#include <boost/exception/info.hpp>
#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

struct test_ex: std::exception { };

typedef boost::error_info<struct test_info_, int> test_info;

int main()
{
	leaf::try_catch(
		[]
		{
			try
			{
				boost::throw_exception(test_ex());
			}
			catch( boost::exception & ex )
			{
				ex << test_info(42);
				throw;
			}
		},
		[]( test_info const & x )
		{
			BOOST_TEST_EQ(x.value(), 42);
		} );

	leaf::try_catch(
		[]
		{
			try
			{
				boost::throw_exception(test_ex());
			}
			catch( boost::exception & ex )
			{
				ex << test_info(42);
				throw;
			}
		},
		[]( leaf::match<test_info, 42> )
		{
		} );

	return boost::report_errors();
}

#endif
