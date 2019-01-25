// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/preload.hpp>
#include <boost/leaf/try.hpp>
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

struct info { int value; };

void g1()
{
	auto propagate = leaf::defer( [ ] { return info{1}; } );
}

void g2()
{
	throw std::exception();
}

void f()
{
	auto propagate = leaf::defer( [ ] { return info{2}; } );
	g1();
	g2();
}

int main()
{
	int r = leaf::try_(
		[ ]
		{
			f();
			return 0;
		},
		[ ]( info x )
		{
			BOOST_TEST_EQ(x.value, 2);
			return 1;
		},
		[ ]
		{
			return 2;
		 } );
	BOOST_TEST_EQ(r, 1);

	return boost::report_errors();
}
