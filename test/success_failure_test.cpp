// Copyright (c) 2018-2020 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/result.hpp>
#include <boost/leaf/handle_error.hpp>
#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

int main()
{
	{
		leaf::result<void> r = leaf::success();
		BOOST_TEST(r);
	}
	{
		leaf::result<int> r = leaf::success<int>();
		BOOST_TEST(r);
		BOOST_TEST_EQ(r.value(), 0);
	}
	{
		leaf::result<int> r = leaf::success(42);
		BOOST_TEST(r);
		BOOST_TEST_EQ(r.value(), 42);
	}

	(void) leaf::new_error();
	{
		leaf::error_id const ce = leaf::current_error();
		leaf::result<void> r = leaf::failure();
		BOOST_TEST(!r);
		BOOST_TEST_NE(leaf::error_id(r.error()), ce);
		BOOST_TEST_EQ(leaf::error_id(r.error()), leaf::current_error());
	}
	{
		leaf::error_id const ce = leaf::current_error();
		leaf::result<int> r = leaf::failure<int>();
		BOOST_TEST(!r);
		BOOST_TEST_NE(leaf::error_id(r.error()), ce);
		BOOST_TEST_EQ(leaf::error_id(r.error()), leaf::current_error());
	}

	{ // void, failure
		int r = 0;
		leaf::try_handle_all(
			[]
			{
				return false ? leaf::success() : leaf::failure(make_error_code(std::errc::broken_pipe));
			},

			[&]( std::error_code const & ec )
			{
				BOOST_TEST(ec == std::errc::broken_pipe);
				r = 1;
			},

			[&]
			{
				r = 2;
			} );
		BOOST_TEST_EQ(r, 1);
	}

	{ // void, success
		int r = 0;
		leaf::try_handle_all(
			[]
			{
				return true ? leaf::success() : leaf::failure(make_error_code(std::errc::broken_pipe));
			},

			[&]
			{
				r = 1;
			} );
		BOOST_TEST_EQ(r, 0);
	}

	{ // int, failure
		int r = leaf::try_handle_all(
			[]
			{
				return false ? leaf::success(42) : leaf::failure<int>(make_error_code(std::errc::broken_pipe));
			},

			[&]( std::error_code const & ec )
			{
				BOOST_TEST(ec == std::errc::broken_pipe);
				return 1;
			},

			[&]
			{
				return 2;
			} );
		BOOST_TEST_EQ(r, 1);
	}

	{ // int, success
		int r = leaf::try_handle_all(
			[]
			{
				return true ? leaf::success(42) : leaf::failure<int>(make_error_code(std::errc::broken_pipe));
			},

			[&]
			{
				return 1;
			} );
		BOOST_TEST_EQ(r, 42);
	}

	return boost::report_errors();
}
