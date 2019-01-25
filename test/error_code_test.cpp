// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/handle.hpp>
#include "_test_res.hpp"
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

int main()
{
	{
		int r = leaf::handle_all(
			[ ]() -> res<int,std::error_code>
			{
				return make_error_code(errc_a::a0);
			},
			[ ]( std::error_code const & ec )
			{
				BOOST_TEST_EQ(ec, make_error_code(errc_a::a0));
				return 42;
			} );
		BOOST_TEST_EQ(r, 42);
	}
	{
		int r = leaf::handle_all(
			[ ]() -> res<int, std::error_code>
			{
				return make_error_code(errc_a::a0);
			},
			[ ]( leaf::match<leaf::condition<cond_x>, cond_x::x00> )
			{
				return 42;
			},
			[ ]
			{
				return -42;
			} );
		BOOST_TEST_EQ(r, 42);
	}
	{
		int r = leaf::handle_all(
			[ ]() -> res<int, std::error_code>
			{
				auto r1 = leaf::handle_some(
					[ ]() -> res<int, std::error_code>
					{
						return make_error_code(errc_a::a0);
					} );
				auto r2 = leaf::handle_some(
					[ ]() -> res<int, std::error_code>
					{
						return make_error_code(errc_b::b0);
					} );
				(void) r2;
				return r1;
			},
			[ ]( leaf::match<leaf::condition<cond_y>, cond_y::y03> )
			{
				return 42;
			},
			[ ]
			{
				return -42;
			} );
		BOOST_TEST_EQ(r, 42);
	}
	{
		int r = leaf::handle_all(
			[ ]() -> res<int, std::error_code>
			{
				return leaf::new_error(errc_a::a0, e_errc_a<1>{make_error_code(errc_a::a1)});
			},
			[ ]( leaf::match<leaf::condition<cond_x>, cond_x::x00>, leaf::match<leaf::condition<e_errc_a<1>, cond_y>, cond_y::y12, cond_y::y03> )
			{
				return 42;
			},
			[ ]
			{
				return -42;
			} );
		BOOST_TEST_EQ(r, 42);
	}
	{
		int r = leaf::handle_all(
			[ ]() -> res<int, std::error_code>
			{
				return leaf::new_error(errc_a::a1, e_errc_a<1>{make_error_code(errc_a::a0)});
			},
			[ ]( leaf::match<leaf::condition<cond_x>, cond_x::x11>, leaf::match<leaf::condition<e_errc_a<1>, cond_y>, cond_y::y12, cond_y::y03> )
			{
				return 42;
			},
			[ ]
			{
				return -42;
			} );
		BOOST_TEST_EQ(r, 42);
	}
	return boost::report_errors();
}
