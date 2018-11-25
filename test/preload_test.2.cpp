//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/result.hpp>
#include <boost/leaf/expect.hpp>
#include <boost/leaf/preload.hpp>
#include <boost/detail/lightweight_test.hpp>

namespace leaf = boost::leaf;

struct info { int value; };

leaf::result<void> g1()
{
	auto propagate = leaf::preload( info{1} );
	return { };
}

leaf::result<void> g2()
{
	return leaf::error();
}

leaf::result<void> f()
{
	auto propagate = leaf::preload( info{2} );
	LEAF_CHECK(g1());
	return g2();
}

int
main()
{
	leaf::expect<info> exp;
	leaf::result<void> r = f();
	int c=0;
	bool handled = handle_error( exp, r,
		[&c]( info const & x )
		{
			BOOST_TEST(x.value==2);
			++c;
		} );
	BOOST_TEST(handled);
	BOOST_TEST(c==1);
	return boost::report_errors();
}
