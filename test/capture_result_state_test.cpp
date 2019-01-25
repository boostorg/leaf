// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/capture_result.hpp>
#include <boost/leaf/handle.hpp>

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
	template <int I> struct is_error_type<info<I>>: public std::true_type { };
} }

int main()
{
	auto f = leaf::capture_result<info<1>, info<2>, info<3>>(
		[ ]() -> leaf::result<void>
		{
			return leaf::new_error( info<1>{}, info<3>{} );
		} );
	leaf::handle_all(
		[&f]
		{
			BOOST_TEST_EQ(count, 0);
			auto r = f();
			BOOST_TEST_EQ(count, 2);
			return r;
		},
		[ ]
		{
		} );
	BOOST_TEST_EQ(count, 0);
	return boost::report_errors();
}
