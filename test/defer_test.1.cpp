//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/expect.hpp>
#include <boost/leaf/preload.hpp>
#include <boost/detail/lightweight_test.hpp>

namespace leaf = boost::leaf;

int global;

int get_global() noexcept
{
	return global;
}

struct info
{
	int value;
};

leaf::error g()
{
	global = 0;
	auto propagate = leaf::defer( [ ] { return info{get_global()}; } );
	global = 42;
	return leaf::error();
}

leaf::error f()
{
	return g();
}

int main()
{
	leaf::expect<info> exp;
	int c=0;
	bool handled = handle_error( exp, f(),
		[&c]( info const & i42 )
		{
			BOOST_TEST(i42.value==42);
			++c;
		} );
	BOOST_TEST(handled);
	return boost::report_errors();
	BOOST_TEST(c==1);
}
