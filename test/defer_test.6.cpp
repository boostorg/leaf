//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/expect.hpp>
#include <boost/leaf/preload.hpp>
#include <boost/detail/lightweight_test.hpp>

namespace leaf = boost::leaf;

struct info
{
	int value;
};

leaf::error f0()
{
	return leaf::error();
}

leaf::error f1()
{
	auto propagate = leaf::defer( [ ] { return info{0}; } );
	(void) f0();
	return leaf::error();
}

int main()
{
	leaf::expect<info> exp;
	leaf::error e = f1();
	int c=0;
	bool handled = handle_error( exp, e,
		[&c]( info const & i0 )
		{
			BOOST_TEST(i0.value==0);
			++c;
		} );
	BOOST_TEST(handled);
	BOOST_TEST(c==1);
	return boost::report_errors();
}
