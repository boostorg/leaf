//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/expect.hpp>

namespace leaf = boost::leaf;

template <int> struct info { int value; };

struct test { };

leaf::error f()
{
	return leaf::error(info<1>{1},info<3>{3});
}

int main()
{
	leaf::expect<info<1>,info<2>,info<3>> exp;

	(void) handle_error( exp, f(),
		[ ]( info<1>, info<2> )
		{
			return test { };
		},
		[ ]( info<1>, info<3> )
		{
			return 42;
		} );

	return 0;
}
