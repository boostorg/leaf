//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/expect.hpp>
#include <boost/leaf/exception.hpp>
#include <boost/detail/lightweight_test.hpp>

namespace leaf = boost::leaf;

struct info { int value; };

struct my_error: std::exception { };

void g()
{
	leaf::expect<info> exp;
	throw my_error();
}

void f()
{
	return g();
}

int main()
{
	leaf::expect<info> exp;
	try
	{
		f();
		BOOST_TEST(false);
	}
	catch( my_error & e )
	{
		int c=0;
		try
		{
			handle_exception( exp, e,
				[&c]( info )
				{
					++c;
				} );
			BOOST_TEST(false);
		}
		catch( my_error & )
		{
		}
		BOOST_TEST(c==0);
	}

	return boost::report_errors();
}
