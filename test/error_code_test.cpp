// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/handle_error.hpp>
#include <boost/leaf/result.hpp>
#include "_test_res.hpp"
#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

struct e_wrapped_error_code { std::error_code value; };

template <class R>
void test()
{
	{
		int r = leaf::try_handle_all(
			[]() -> R
			{
				return make_error_code(errc_a::a0);
			},
			[]( std::error_code const & ec )
			{
				BOOST_TEST_EQ(ec, make_error_code(errc_a::a0));
				return 42;
			} );
		BOOST_TEST_EQ(r, 42);
	}
	{
		int r = leaf::try_handle_all(
			[]() -> R
			{
				return make_error_code(errc_a::a0);
			},
			[]( leaf::match<leaf::condition<cond_x>, cond_x::x00> )
			{
				return 42;
			},
			[]
			{
				return -42;
			} );
		BOOST_TEST_EQ(r, 42);
	}
	{
		int r = leaf::try_handle_all(
			[]() -> R
			{
				return leaf::new_error( e_wrapped_error_code { make_error_code(errc_a::a0) } ).to_error_code();
			},
			[]( leaf::match<leaf::condition<e_wrapped_error_code, cond_x>, cond_x::x00> )
			{
				return 42;
			},
			[]
			{
				return -42;
			} );
		BOOST_TEST_EQ(r, 42);
	}
	{
		int r = leaf::try_handle_all(
			[]() -> R
			{
				auto r1 = leaf::try_handle_some(
					[]() -> R
					{
						return make_error_code(errc_a::a0);
					} );
				auto r2 = leaf::try_handle_some(
					[]() -> R
					{
						return make_error_code(errc_b::b0);
					} );
				(void) r1;
				return r2;
			},
			[]( leaf::match<leaf::condition<cond_y>, cond_y::y03> )
			{
				return 42;
			},
			[]
			{
				return -42;
			} );
		BOOST_TEST_EQ(r, 42);
	}
	{
		int r = leaf::try_handle_all(
			[]() -> R
			{
				auto r1 = leaf::try_handle_some(
					[]() -> R
					{
						return leaf::new_error( e_wrapped_error_code { make_error_code(errc_a::a0) } ).to_error_code();
					} );
				auto r2 = leaf::try_handle_some(
					[]() -> R
					{
						return make_error_code(errc_b::b0);
					} );
				(void) r1;
				return r2;
			},
			[]( leaf::match<leaf::condition<cond_y>, cond_y::y03> )
			{
				return 42;
			},
			[]
			{
				return -42;
			} );
		BOOST_TEST_EQ(r, 42);
	}
}

int main()
{
	test<leaf::result<int>>();
	test<test_res<int, std::error_code>>();
	return boost::report_errors();
}
