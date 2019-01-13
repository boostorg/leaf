// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/preload.hpp>
#include <boost/leaf/handle.hpp>
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

struct info { int value; };

leaf::result<void> g1()
{
	auto propagate = leaf::defer( [ ] { return info{1}; } );
	return { };
}

leaf::result<void> g2()
{
	return leaf::new_error();
}

leaf::result<void> f()
{
	auto propagate = leaf::defer( [ ] { return info{2}; } );
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
			BOOST_TEST(x.value==2);
			return 2;
		},
		[ ]
		{
			return 3;
		 } );
	BOOST_TEST(r==2);

	return boost::report_errors();
}
