//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/expect.hpp>
#include <boost/detail/lightweight_test.hpp>

namespace leaf = boost::leaf;

struct info { int value; };

leaf::error g()
{
	leaf::expect<info> exp;
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
	BOOST_TEST( !handle_error( exp, f(),
		[&c]( info )
		{
			++c;
		} ) );
	BOOST_TEST(c==0);
	return boost::report_errors();
}
