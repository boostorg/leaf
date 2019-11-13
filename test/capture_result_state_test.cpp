// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/capture.hpp>
#include <boost/leaf/result.hpp>
#include <boost/leaf/handle_error.hpp>
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
		return leaf::remote_handle_all( err,
			[]( info<1>, info<3> )
			{
				return 42;
			},
			[]
			{
				return -42;
			} );
	};
	{
		auto r = leaf::capture(
			leaf::make_shared_context(&error_handler),
			[]() -> leaf::result<int>
			{
				return leaf::new_error( info<1>{}, info<3>{} );
			} );
		BOOST_TEST_EQ(count, 2);
		int answer = leaf::remote_try_handle_all(
			[&r]
			{
				return std::move(r);
			},
			[&]( leaf::error_info const & err )
			{
				return error_handler(err);
			} );
		BOOST_TEST_EQ(answer, 42);
	}
	BOOST_TEST_EQ(count, 0);
	return boost::report_errors();
}
