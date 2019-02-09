// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/preload.hpp>
#include <boost/leaf/handle_error.hpp>
#include <boost/leaf/result.hpp>
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

struct info { int value; };

leaf::result<void> g1()
{
	auto load = leaf::defer( [ ] { return info{1}; } );
	return { };
}

leaf::result<void> g2()
{
	return leaf::new_error();
}

leaf::result<void> f()
{
	auto load = leaf::defer( [ ] { return info{2}; } );
	LEAF_CHECK(g1());
	return g2();
}

int main()
{
	int r = leaf::handle_all(
		[ ]() -> leaf::result<int>
		{
			LEAF_CHECK(f());
			return 1;
		},
		[ ]( info x )
		{
			BOOST_TEST_EQ(x.value, 2);
			return 2;
		},
		[ ]
		{
			return 3;
		 } );
	BOOST_TEST_EQ(r, 2);

	return boost::report_errors();
}
