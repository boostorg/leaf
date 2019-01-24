// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/try.hpp>
#include <boost/leaf/result.hpp>
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int> struct info { int value; };

struct my_exception: std::exception { };

int main()
{
	{
		leaf::result<int> r = leaf::try_(
			[ ]() -> leaf::result<int>
			{
				return 42;
			},
			[ ]() -> leaf::result<int>
			{
				return { };
			} );
		BOOST_TEST(r && r.value()==42);
	}
	{
		leaf::result<int> r = leaf::try_(
			[ ]() -> leaf::result<int>
			{
				throw leaf::exception( my_exception(), info<1>{1} );
			},
			[ ]( leaf::catch_<my_exception>, info<1> const & x ) -> leaf::result<int>
			{
				BOOST_TEST(x.value==1);
				return 42;
			} );
		BOOST_TEST(r && r.value()==42);
	}
	{
		leaf::result<int> r = leaf::try_(
			[ ]() -> leaf::result<int>
			{
				return leaf::new_error( info<1>{1} );
			},
			[ ]( info<1> const & x ) -> leaf::result<int>
			{
				BOOST_TEST(x.value==1);
				return 42;
			} );
		BOOST_TEST(r && r.value()==42);
	}
	return boost::report_errors();
}
