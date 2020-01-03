// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>
#if defined(LEAF_NO_EXCEPTIONS) || defined(LEAF_NO_THREADS)

#include <iostream>

int main()
{
	std::cout << "Unit test not applicable." << std::endl;
	return 0;
}

#else

#include <boost/leaf/preload.hpp>
#include <boost/leaf/handle_exception.hpp>
#include <boost/leaf/result.hpp>
#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int>
struct info
{
	int value;
};

template <class Thrower>
void g1( Thrower th )
{
	auto load = leaf::preload( info<1>{} );
	th();
}

template <class Thrower>
void g2( Thrower th )
{
	auto load = leaf::preload( info<2>{} );
	th();
}

template <class Thrower>
void f1( Thrower th )
{
	return g1(th);
}

template <class Thrower>
void f2( Thrower th )
{
	return g2(th);
}

int main()
{
	BOOST_TEST_EQ(1,
		leaf::try_catch(
			[]
			{
				f1( [] { throw leaf::exception(std::exception()); } );
				return 0;
			},
			[]( leaf::error_info const & err, info<1> )
			{
				BOOST_TEST_EQ(err.error().value(), 1);
				return 1;
			},
			[]( info<2> )
			{
				return 2;
			},
			[]( info<1>, info<2> )
			{
				return 3;
			} ));

	BOOST_TEST_EQ(2,
		leaf::try_catch(
			[]
			{
				f2( [] { throw leaf::exception(std::exception()); } );
				return 0;
			},
			[]( info<1> )
			{
				return 1;
			},
			[]( leaf::error_info const & err, info<2> )
			{
				BOOST_TEST_EQ(err.error().value(), 5);
				return 2;
			},
			[]( info<1>, info<2> )
			{
				return 3;
			} ));

	BOOST_TEST_EQ(1,
		leaf::try_catch(
			[]
			{
				f1( [] { throw std::exception(); } );
				return 0;
			},
			[]( leaf::error_info const & err, info<1> )
			{
				BOOST_TEST_EQ(err.error().value(), 9);
				return 1;
			},
			[]( info<2> )
			{
				return 2;
			},
			[]( info<1>, info<2> )
			{
				return 3;
			} ) );

	BOOST_TEST_EQ(2,
		leaf::try_catch(
			[]
			{
				f2( [] { throw std::exception(); } );
				return 0;
			},
			[]( info<1> )
			{
				return 1;
			},
			[]( leaf::error_info const & err, info<2> )
			{
				BOOST_TEST_EQ(err.error().value(), 13);
				return 2;
			},
			[]( info<1>, info<2> )
			{
				return 3;
			} ));

	return boost::report_errors();
}

#endif
