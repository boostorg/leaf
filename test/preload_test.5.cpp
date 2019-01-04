//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/expect.hpp>
#include <boost/leaf/preload.hpp>
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int A>
struct info
{
	int value;
};

leaf::error f0()
{
	auto propagate = leaf::preload( info<0>{-1} );
	return leaf::error( info<1>{-1} );
}

leaf::error f1()
{
	auto propagate = leaf::preload( info<0>{0}, info<1>{1}, info<2>{2} );
	(void) f0();
	return leaf::error();
}

leaf::error f2()
{
	return f1().propagate( info<3>{3} );
}

int main()
{
	leaf::expect<info<0>,info<1>,info<2>,info<3>> exp;
	leaf::error e = f2();
	int c=0;
	bool handled = exp.handle_error( e,
		[&c]( info<0> const & i0, info<1> const & i1, info<2> const & i2, info<3> const & i3 )
		{
			BOOST_TEST(i0.value==0);
			BOOST_TEST(i1.value==1);
			BOOST_TEST(i2.value==2);
			BOOST_TEST(i3.value==3);
			++c;
		} );
	BOOST_TEST(handled);
	BOOST_TEST(c==1);
	return boost::report_errors();
}
