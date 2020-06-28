// Copyright (c) 2018-2020 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/handle_exception.hpp>
#include <boost/exception/info.hpp>
#include <boost/exception/get_error_info.hpp>

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
		} );

	return 0;
}

#endif
