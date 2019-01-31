// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/capture_in_result.hpp>
#include <boost/leaf/handle_all.hpp>

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
	auto handler = [ ]( leaf::error_in_capture_handle_all const & err )
	{
		return leaf::handle_error( err,
			[ ]( info<1>, info<3> )
			{
			},
			[ ]
			{
			} );
	};
	{
		auto r = leaf::capture_in_result<decltype(handler)>(
			[ ]() -> leaf::result<void>
			{
				return leaf::new_error( info<1>{}, info<3>{} );
			} );
		BOOST_TEST_EQ(count, 2);
		leaf::capture_handle_all(
			[&r]
			{
				return r;
			},
			[&]( leaf::error_in_capture_handle_all const & err )
			{
				return handler(err);
			} );
	}
	BOOST_TEST_EQ(count, 0);
	return boost::report_errors();
}
