// Copyright (c) 2018-2020 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>
#ifdef LEAF_NO_EXCEPTIONS

#include <iostream>

int main()
{
	std::cout << "Unit test not applicable." << std::endl;
	return 0;
}

#else

#include <boost/leaf/handle_exception.hpp>
#include <boost/leaf/result.hpp>
#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

struct e_test { int value; };

int check( leaf::catch_<leaf::bad_result>, leaf::match<e_test, 42> )
{
	return 1;
}

struct res { int val; };

int main()
{
	{
		int r = leaf::try_catch(
			[]
			{
				leaf::result<int> r = leaf::new_error(e_test{42});
				(void) r.value();
				return 0;
			},
			check );
		BOOST_TEST_EQ(r, 1);
	}
	{
		int r = leaf::try_catch(
			[]
			{
				leaf::result<int> const r = leaf::new_error(e_test{42});
				(void) r.value();
				return 0;
			},
			check );
		BOOST_TEST_EQ(r, 1);
	}
	{
		int r = leaf::try_catch(
			[]
			{
				leaf::result<int> r = leaf::new_error(e_test{42});
				(void) *r;
				return 0;
			},
			check );
		BOOST_TEST_EQ(r, 1);
	}
	{
		int r = leaf::try_catch(
			[]
			{
				leaf::result<int> const r = leaf::new_error(e_test{42});
				(void) *r;
				return 0;
			},
			check );
		BOOST_TEST_EQ(r, 1);
	}
	{
		int r = leaf::try_catch(
			[]
			{
				leaf::result<res> r = leaf::new_error(e_test{42});
				(void) r->val;
				return 0;
			},
			check );
		BOOST_TEST_EQ(r, 1);
	}
	{
		int r = leaf::try_catch(
			[]
			{
				leaf::result<res> const r = leaf::new_error(e_test{42});
				(void) r->val;
				return 0;
			},
			check );
		BOOST_TEST_EQ(r, 1);
	}
	return boost::report_errors();
}

#endif
