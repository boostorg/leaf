//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/expect.hpp>
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int> struct info { int value; };

leaf::error f()
{
	return leaf::error(info<1>{1},info<3>{3});
}

int main()
{
	leaf::expect<info<1>,info<2>,info<3>> exp;

	{
		int c=0;
		bool r = exp.handle_error( f(),
			[&c]( info<1> const & x )
			{
				BOOST_TEST(c==0);
				BOOST_TEST(x.value==1);
				c = 1;
			} );
		BOOST_TEST(r);
		BOOST_TEST(c==1);
	}

	{
		int c=0;
		bool r = exp.handle_error( f(),
			[&c]( info<2> const & x )
			{
				BOOST_TEST(c==0);
				BOOST_TEST(x.value==2);
				c = 1;
			} );
		BOOST_TEST(!r);
		BOOST_TEST(c==0);
	}

	{
		int c=0;
		int r = exp.handle_error( f(),
			[&c]( info<1> const & x )
			{
				BOOST_TEST(c==0);
				BOOST_TEST(x.value==1);
				return c = 1;
			} );
		BOOST_TEST(r==1);
	}

	{
		int c=0;
		int r = exp.handle_error( f(),
			[&c]( info<2> const & x )
			{
				BOOST_TEST(c==0);
				BOOST_TEST(x.value==2);
				return c = 1;
			} );
		BOOST_TEST(r==-1);
	}

	{
		int c=0;
		bool r = exp.handle_error( f(),
			[&c]( info<1> const & x, info<2> const & )
			{
				BOOST_TEST(c==0);
				c = 1;
			},
			[&c]( info<1> const & x, info<3> const & y )
			{
				BOOST_TEST(c==0);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==3);
				c = 2;
				return false;
			},
			[&c]( info<1> const & x )
			{
				BOOST_TEST(c==0);
				c = 3;
			} );
		BOOST_TEST(!r);
		BOOST_TEST(c==2);
	}

	{
		int c=0;
		bool r = exp.handle_error( f(),
			[&c]( info<1> const & x, info<2> const & )
			{
				BOOST_TEST(c==0);
				c = 1;
				return false;
			},
			[&c]( info<1> const & x, info<3> const & y )
			{
				BOOST_TEST(c==0);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==3);
				c = 2;
			},
			[&c]( info<1> const & x )
			{
				BOOST_TEST(c==0);
				c = 3;
				return false;
			} );
		BOOST_TEST(r);
		BOOST_TEST(c==2);
	}

	return boost::report_errors();
}
