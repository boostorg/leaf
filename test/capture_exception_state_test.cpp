// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>
#ifdef LEAF_NO_EXCEPTIONS

#include <iostream>

int main()
{
	std::cout << "Unit test not applicable." << std::endl;
	return 0;
}

#else

#include <boost/leaf/capture.hpp>
#include <boost/leaf/handle_exception.hpp>
#include <boost/leaf/exception.hpp>
#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

int count = 0;

template <int>
struct info
{
	info() noexcept
	{
		++count;
	}

	info( info const & ) noexcept
	{
		++count;
	}

	~info() noexcept
	{
		--count;
	}
};

namespace boost { namespace leaf {
	template <int I> struct is_e_type<info<I>>: public std::true_type { };
} }

int main()
{
	auto error_handler = []( leaf::error_info const & err )
	{
		return leaf::remote_handle_exception( err,
			[]( info<1>, info<3> )
			{
				return 1;
			},
			[]
			{
				return 2;
			} );
	};
	BOOST_TEST_EQ(count, 0);
	std::exception_ptr ep;
	try
	{
		leaf::capture(
			leaf::make_shared_context(&error_handler),
			[]
			{
				throw leaf::exception( std::exception(), info<1>{}, info<3>{} );
			} );
		BOOST_TEST(false);
	}
	catch(...)
	{
		ep = std::current_exception();
	}
	BOOST_TEST_EQ(count, 2);
	int r = leaf::remote_try_catch(
		[&]() -> int
		{
			std::rethrow_exception(ep);
		},
		[&]( leaf::error_info const & err )
		{
			return error_handler(err);
		} );
	BOOST_TEST_EQ(r, 1);
	ep = std::exception_ptr();
	BOOST_TEST_EQ(count, 0);
	return boost::report_errors();
}

#endif
