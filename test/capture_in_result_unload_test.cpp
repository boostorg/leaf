// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/capture_in_result.hpp>
#include <boost/leaf/handle_all.hpp>
#include "_test_ec.hpp"
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int> struct info { int value; };

template <class F>
void test( F f_ )
{
	auto f = leaf::capture_in_result<info<1>, info<2>, info<3>>( [=] { return f_(); } );

	{
		int c=0;
		auto r = f();
		leaf::handle_all(
			[&r]
			{
				return r;
			},
			[&c]( info<1> const & x )
			{
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(c, 0);
				c = 1;
			},
			[&c]
			{
				BOOST_TEST_EQ(c, 0);
				c = 2;
			} );
		BOOST_TEST_EQ(c, 1);
	}

	{
		int c=0;
		auto r = f();
		leaf::handle_all(
			[&r]
			{
				return r;
			},
			[&c]( info<2> const & x )
			{
				BOOST_TEST_EQ(x.value, 2);
				BOOST_TEST_EQ(c, 0);
				c = 1;
			},
			[&c]
			{
				BOOST_TEST_EQ(c, 0);
				c = 2;
			} );
		BOOST_TEST_EQ(c, 2);
	}

	{
		auto r = f();
		int what = leaf::handle_all(
			[&r]() -> leaf::result<int>
			{
				return r.error();
			},
			[ ]( info<1> const & x )
			{
				BOOST_TEST_EQ(x.value, 1);
				return 1;
			},
			[ ]
			{
				return 2;
			} );
		BOOST_TEST_EQ(what, 1);
	}

	{
		auto r = f();
		int what = leaf::handle_all(
			[&r]() -> leaf::result<int>
			{
				return r.error();
			},
			[ ]( info<2> const & x )
			{
				BOOST_TEST_EQ(x.value, 2);
				return 1;
			},
			[ ]
			{
				return 2;
			} );
		BOOST_TEST_EQ(what, 2);
	}

	{
		auto r = f();
		bool what = leaf::handle_all(
			[&r]() -> leaf::result<bool>
			{
				return r.error();
			},
			[ ]( info<1> const & x, info<2> const & )
			{
				return true;
			},
			[ ]( info<1> const & x, info<3> const & y )
			{
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 3);
				return false;
			},
			[ ]( info<1> const & x )
			{
				return true;
			},
			[ ]
			{
				return true;
			} );
		BOOST_TEST(!what);
	}

	{
		auto r = f();
		bool what = leaf::handle_all(
			[&r]() -> leaf::result<bool>
			{
				return r.error();
			},
			[ ]( info<1> const & x, info<2> const & )
			{
				return false;
			},
			[ ]( info<1> const & x, info<3> const & y, leaf::match<leaf::condition<cond_x>, cond_x::x00> )
			{
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 3);
				return true;
			},
			[ ]( info<1> const & x )
			{
				return false;
			},
			[ ]
			{
				return false;
			} );
		BOOST_TEST(what);
	}
}

int main()
{
	test(
		[ ]() -> leaf::result<void>
		{
			return leaf::new_error(errc_a::a0, info<1>{1}, info<3>{3} );
		} );
	return boost::report_errors();
}
