//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/expect.hpp>
#include <boost/leaf/exception.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <functional>

namespace leaf = boost::leaf;

struct my_error: std::exception { };

std::function<void()> thrower;

template <int A>
struct
info
	{
	int value;
	};
void
f1()
	{
	thrower();
	}
void
f2()
	{
	leaf::expect<info<1>> exp;
	try
		{
		f1();
		BOOST_TEST(false);
		}
	catch( leaf::error const & e )
		{
		e.propagate( info<2>{2} );
		throw;
		}
	catch( ... )
		{
		leaf::preload( info<2>{2} );
		throw;
		}
	}
void
f3()
	{
	leaf::expect<info<2>,info<3>> exp;
	leaf::preload( info<4>{4} );
	f2();
	}
void
f4()
	{
	leaf::expect<info<1>,info<2>,info<3>,info<4>> exp;
	try
		{
		f3();
		BOOST_TEST(false);
		}
	catch(
	my_error const & e )
		{
		int c1=0, c2=0;
		handle_exception( exp, e,
			leaf::match<info<1>,info<2>,info<3>,info<4>>( [&c1]( int, int, int, int )
				{
				++c1;
				} ),
			leaf::match<info<1>,info<2>,info<4>>( [&c2]( int i1, int i2, int i4 )
				{
				BOOST_TEST(i1==1);
				BOOST_TEST(i2==2);
				BOOST_TEST(i4==4);
				++c2;
				} ) );
		BOOST_TEST(c1==0);
		BOOST_TEST(c2==1);
		}
	throw my_error();
	}
void
test( std::function<void()> const & f )
	{
	thrower = f;
	leaf::expect<info<2>,info<3>,info<4>> exp;
	try
		{
		f4();
		BOOST_TEST(false);
		}
	catch(
	my_error const & e )
		{
		BOOST_TEST(!leaf::peek<info<2>>(exp,e));
		BOOST_TEST(!leaf::peek<info<3>>(exp,e));
		BOOST_TEST(!leaf::peek<info<4>>(exp,e));
		BOOST_TEST(leaf::leaf_detail::tl_slot_ptr<info<1>>()==0);
		BOOST_TEST(leaf::leaf_detail::tl_slot_ptr<info<2>>()!=0);
		BOOST_TEST(leaf::leaf_detail::tl_slot_ptr<info<3>>()!=0);
		BOOST_TEST(leaf::leaf_detail::tl_slot_ptr<info<4>>()!=0);
		}
	}
int
main()
	{
	test( [ ]
		{
		leaf::throw_exception( my_error(), info<1>{1} );
		} );
	test( [ ]
		{
		leaf::preload(info<1>{1});
		throw my_error();
		} );
	return boost::report_errors();
	}
