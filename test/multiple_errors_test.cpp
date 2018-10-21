//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/expect.hpp>
#include <boost/detail/lightweight_test.hpp>

namespace leaf = boost::leaf;

template <int A>
struct
info
	{
	int value;
	};
leaf::error
f12()
	{
	return leaf::error( info<1>{1}, info<2>{2} );
	}
leaf::error
f34()
	{
	return leaf::error( info<3>{3}, info<4>{4} );
	}
int
main()
	{
	leaf::expect<info<1>,info<2>,info<3>,info<4>> exp;
	leaf::error e1=f12();
	leaf::error e2=f34();
	int e1c1=0, e1c2=0;
	handle_error( exp, e1,
		leaf::match<info<3>,info<4>>( [&e1c1]( int, int )
			{
			++e1c1;
			} ),
		leaf::match<info<1>,info<2>>( [&e1c2]( int i1, int i2 )
			{
			BOOST_TEST(i1==1);
			BOOST_TEST(i2==2);
			++e1c2;
			} ) );
	BOOST_TEST(e1c1==0);
	BOOST_TEST(e1c2==1);
	int e2c1=0, e2c2=0;
	handle_error( exp, e2,
		leaf::match<info<1>,info<2>>( [&e2c1]( int, int )
			{
			++e2c1;
			} ),
		leaf::match<info<3>,info<4>>( [&e2c2]( int i3, int i4 )
			{
			BOOST_TEST(i3==3);
			BOOST_TEST(i4==4);
			++e2c2;
			} ) );
	BOOST_TEST(e2c1==0);
	BOOST_TEST(e2c2==1);
	return boost::report_errors();
	}
