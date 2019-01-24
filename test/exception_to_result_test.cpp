// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/exception_to_result.hpp>
#include <boost/leaf/handle.hpp>
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int> struct my_exception: std::exception { };

int main()
{
	{
		int r = leaf::handle_all(
			[ ]
			{
				return leaf::exception_to_result<my_exception<1>,my_exception<2>>(
					[ ]() -> int
					{
						throw my_exception<1>();
					} );
			},
			[ ]( my_exception<1> const & )
			{
				return 1;
			},
			[ ]( my_exception<2> const & )
			{
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST(r==1);
	}
	{
		int r = leaf::handle_all(
			[ ]
			{
				return leaf::exception_to_result<my_exception<1>,my_exception<2>>(
					[ ]() -> int
					{
						throw my_exception<2>();
					} );
			},
			[ ]( my_exception<1> const & )
			{
				return 1;
			},
			[ ]( my_exception<2> const & )
			{
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST(r==2);
	}
	{
		int r = leaf::handle_all(
			[ ]
			{
				return leaf::exception_to_result<std::exception,my_exception<1>>(
					[ ]() -> int
					{
						throw my_exception<1>();
					} );
			},
			[ ]( std::exception const & )
			{
				return 1;
			},
			[ ]( my_exception<1> const & )
			{
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST(r==1);
	}
	return boost::report_errors();
}
