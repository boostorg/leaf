//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/exception.hpp>
#include <boost/leaf/expect.hpp>
#include <boost/leaf/preload.hpp>
#include <boost/detail/lightweight_test.hpp>

namespace leaf = boost::leaf;

struct my_error: std::exception { };

struct info { int value; };

void g1()
{
	auto propagate = leaf::defer( [ ] { return info{1}; } );
}

void g2()
{
	throw leaf::exception<my_error>();
}

void f()
{
	auto propagate = leaf::defer( [ ] { return info{2}; } );
	g1();
	g2();
}

int main()
{
	leaf::expect<info> exp;
	try
	{
		f();
	}
	catch( my_error & e )
	{
		int c=0;
		handle_exception( exp, e,
			[&c]( info const & x )
			{
				BOOST_TEST(x.value==2);
				++c;
			} );
		BOOST_TEST(c==1);
	}
	return boost::report_errors();
}
