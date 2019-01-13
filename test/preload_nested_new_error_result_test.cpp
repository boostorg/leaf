// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/preload.hpp>
#include <boost/leaf/handle.hpp>
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
	return leaf::new_error( info<1>{-1} );
}

leaf::error f1()
{
	auto propagate = leaf::preload( info<0>{0}, info<1>{1}, info<2>{2} );
	(void) f0();
	return leaf::new_error();
}

leaf::error f2()
{
	return f1().propagate( info<3>{3} );
}

int main()
{
	int r = leaf::handle_all(
		[ ]() -> leaf::result<int>
		{
			return f2();
		},
		[ ]( info<0> i0, info<1> i1, info<2> i2, info<3> i3 )
		{
			BOOST_TEST(i0.value==0);
			BOOST_TEST(i1.value==1);
			BOOST_TEST(i2.value==2);
			BOOST_TEST(i3.value==3);
			return 1;
		},
		[ ]
		{
			return 2;
		} );
	BOOST_TEST(r==1);

	return boost::report_errors();
}
