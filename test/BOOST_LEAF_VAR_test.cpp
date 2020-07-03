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
		int r = leaf::try_handle_all(
			[]() -> leaf::result<int>
			{
				leaf::result<int> r(42);
				BOOST_LEAF_VAR(auto && rx, r);
				BOOST_TEST_EQ(r.value(), rx);
				return 0;
			},
			[]
			{
				return 1;
			} );
		BOOST_TEST_EQ(r, 0);
	}

	{
		int r = leaf::try_handle_all(
			[]() -> leaf::result<int>
			{
				int x = 42;
				leaf::result<int &> r(x);
				BOOST_LEAF_VAR(auto && rx, r);
				BOOST_TEST_EQ(x, rx);
				return 0;
			},
			[]
			{
				return 1;
			} );
		BOOST_TEST_EQ(r, 0);
	}

	{
		int r = leaf::try_handle_all(
			[]() -> leaf::result<int>
			{
				int x = 42;
				leaf::result<int &> r(x);
				BOOST_LEAF_VAR(auto & rx, r);
				BOOST_TEST_EQ(&x, &rx);
				return 0;
			},
			[]
			{
				return 1;
			} );
		BOOST_TEST_EQ(r, 0);
	}

	return boost::report_errors();
}
