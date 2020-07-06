// Copyright (c) 2018-2020 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/result.hpp>
#include <boost/leaf/handle_error.hpp>
#include "_test_ec.hpp"
#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

int main()
{
	{ // success, void, default
		leaf::result<void> r = leaf::success();
		BOOST_TEST(r);
	}
	{ // success, int, default
		leaf::result<int> r = leaf::success<int>();
		BOOST_TEST(r);
		BOOST_TEST_EQ(r.value(), 0);
	}
	{ // success, int, value
		leaf::result<int> r = leaf::success(42);
		BOOST_TEST(r);
		BOOST_TEST_EQ(r.value(), 42);
	}

	(void) leaf::new_error();
	{ // failure, void, default
		leaf::error_id const ce = leaf::current_error();
		leaf::result<void> r = leaf::failure();
		BOOST_TEST(!r);
		BOOST_TEST_NE(leaf::error_id(r.error()), ce);
		BOOST_TEST_EQ(leaf::error_id(r.error()), leaf::current_error());
	}
	{ // failure, int, default
		leaf::error_id const ce = leaf::current_error();
		leaf::result<int> r = leaf::failure<int>();
		BOOST_TEST(!r);
		BOOST_TEST_NE(leaf::error_id(r.error()), ce);
		BOOST_TEST_EQ(leaf::error_id(r.error()), leaf::current_error());
	}
	{ // failure, int, std::is_error_code_enum error code
		leaf::error_id const ce = leaf::current_error();
		leaf::result<int> r = errc_a::a0;
		BOOST_TEST(!r);
		BOOST_TEST_NE(leaf::error_id(r.error()), ce);
		BOOST_TEST_EQ(leaf::error_id(r.error()), leaf::current_error());
	}

	{ // void, success
		int r = 0;
		leaf::try_handle_all(
			[]
			{
				return true ? leaf::success() : errc_a::a0;
			},

			[&]
			{
				r = 1;
			} );
		BOOST_TEST_EQ(r, 0);
	}

	{ // void, failure
		int r = 0;
		leaf::try_handle_all(
			[]
			{
				return false ? leaf::success() : errc_a::a0;
			},

			[&]( std::error_code const & ec )
			{
				BOOST_TEST(ec == errc_a::a0);
				r = 1;
			},

			[&]
			{
				r = 2;
			} );
		BOOST_TEST_EQ(r, 1);
	}

	{ // int, success
		int r = leaf::try_handle_all(
			[]
			{
				return true ? leaf::success(42) : errc_a::a0;
			},

			[&]
			{
				return 1;
			} );
		BOOST_TEST_EQ(r, 42);
	}

	{ // int, failure
		int r = leaf::try_handle_all(
			[]
			{
				return false ? leaf::success(42) : errc_a::a0;
			},

			[&]( std::error_code const & ec )
			{
				BOOST_TEST(ec == errc_a::a0);
				return 1;
			},

			[&]
			{
				return 2;
			} );
		BOOST_TEST_EQ(r, 1);
	}

	return boost::report_errors();
}
