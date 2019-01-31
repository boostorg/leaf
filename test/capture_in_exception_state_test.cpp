// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/capture_in_exception.hpp>
#include <boost/leaf/try.hpp>
#include <boost/leaf/exception.hpp>
#include "boost/core/lightweight_test.hpp"

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
	auto error_handler = [ ]( leaf::error_in_remote_try_ const & error )
	{
		return leaf::handle_error( error,
			[ ]( info<1>, info<3> )
			{
			} );
	};
	BOOST_TEST_EQ(count, 0);
	std::exception_ptr ep;
	try
	{
		leaf::capture_in_exception<decltype(error_handler)>(
			[ ]
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
	leaf::remote_try_(
		[&]
		{
			std::rethrow_exception(ep);
		},
		[&]( leaf::error_in_remote_try_ const & error )
		{
			return error_handler(error);
		} );
	ep = std::exception_ptr();
	BOOST_TEST_EQ(count, 0);
	return boost::report_errors();
}
