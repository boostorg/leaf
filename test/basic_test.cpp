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
f1()
	{
	return leaf::error( info<1>{1} );
	}
leaf::error
f2()
	{
	return f1().propagate( info<2>{2} );
	}
leaf::error
f3()
	{
	return f2().propagate( info<3>{3} );
	}
leaf::error
f4()
	{
	return f3().propagate();
	}
int
main()
	{
	leaf::expect<info<1>,info<2>,info<4>> exp0;
	BOOST_TEST(!leaf::current_error());
	leaf::error e0 = f4();
	BOOST_TEST(*leaf::current_error()==e0);
		{
		int const * p = leaf::peek<info<1>>(exp0,e0);
		BOOST_TEST(p && *p==1);
		}
		{
		int const * p = leaf::peek<info<2>>(exp0,e0);
		BOOST_TEST(p && *p==2);
		}
	BOOST_TEST(!leaf::peek<info<4>>(exp0,e0));
	BOOST_TEST(*leaf::current_error()==e0);
	leaf::expect<info<1>,info<2>,info<4>> exp;
	leaf::error e1 = f4();
		{
		int const * p = leaf::peek<info<1>>(exp0,e0);
		BOOST_TEST(p && *p==1);
		}
		{
		int const * p = leaf::peek<info<2>>(exp0,e0);
		BOOST_TEST(p && *p==2);
		}
	BOOST_TEST(!leaf::peek<info<4>>(exp0,e0));
	BOOST_TEST(!leaf::peek<info<1>>(exp,e0));
	BOOST_TEST(!leaf::peek<info<2>>(exp,e0));
	BOOST_TEST(!leaf::peek<info<4>>(exp,e0));
	BOOST_TEST(*leaf::current_error()==e1);
		{
		int const * p = leaf::peek<info<1>>(exp,e1);
		BOOST_TEST(p && *p==1);
		}
		{
		int const * p = leaf::peek<info<2>>(exp,e1);
		BOOST_TEST(p && *p==2);
		}
	BOOST_TEST(!leaf::peek<info<4>>(exp,e1));
	try
		{
		handle_error(exp,e1,leaf::match<info<1>,info<2>,info<4>>( [ ](int,int,int) { } ));
		BOOST_TEST(false);
		}
	catch(
	leaf::mismatch_error const & e )
		{
		BOOST_TEST(*leaf::current_error()==dynamic_cast<leaf::error const &>(e));
		}
	leaf::error e2 = f4();
		{
		int const * p = leaf::peek<info<1>>(exp,e2);
		BOOST_TEST(p && *p==1);
		}
		{
		int const * p = leaf::peek<info<2>>(exp,e2);
		BOOST_TEST(p && *p==2);
		}
	BOOST_TEST(!leaf::peek<info<4>>(exp,e2));
	BOOST_TEST(*leaf::current_error()==e2);
		{
		int c1=0, c2=0, c3=0;
		handle_error( exp, e2,
			leaf::match<info<1>,info<2>,info<4>>( [&c1]( int, int, int )
				{
				++c1;
				} ),
			leaf::match<info<1>,info<2>,info<4>>( [&c2]( int, int, int )
				{
				++c2;
				} ),
			leaf::match<info<2>,info<1>>( [&c3]( int i2,int i1 )
				{
				BOOST_TEST(i1==1);
				BOOST_TEST(i2==2);
				++c3;
				} ) );
		BOOST_TEST(c1==0);
		BOOST_TEST(c2==0);
		BOOST_TEST(c3==1);
		}
		{
		int c=0;
		handle_error( exp0, e0,
			leaf::match<info<2>,info<1>>( [&c]( int i2,int i1 )
				{
				BOOST_TEST(i1==1);
				BOOST_TEST(i2==2);
				++c;
				} ) );
		BOOST_TEST(c==1);
		}
	BOOST_TEST(!leaf::current_error());
	return boost::report_errors();
	}
