//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/try.hpp>
#include <boost/leaf/throw.hpp>
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

struct info { int value; };

struct my_error: std::exception { };

template <class F>
int test( F && f )
{
	return leaf::try_(
		[&]() -> int
		{
			f();
			return 1;
		},
		[ ]( leaf::catch_<my_error>, info, leaf::e_source_location )
		{
			return 2;
		},
		[ ]( leaf::catch_<my_error>, info x )
		{
			return 3;
		},
		[ ]( leaf::catch_<my_error>, leaf::e_source_location )
		{
			return 4;
		},
		[ ]( leaf::catch_<my_error> )
		{
			return 5;
		},
		[ ]
		{
			return 6;
		} );
}

int main()
{
	BOOST_TEST( 5==test( [ ] { throw my_error(); } ) );

	BOOST_TEST( 5==test( [ ] { throw leaf::exception(my_error()); } ) );
	BOOST_TEST( 3==test( [ ] { throw leaf::exception(my_error(),info{42}); } ) );

	BOOST_TEST( 4==test( [ ] { throw LEAF_EXCEPTION(my_error()); } ) );
	BOOST_TEST( 2==test( [ ] { throw LEAF_EXCEPTION(my_error(),info{42}); } ) );

	BOOST_TEST( 4==test( [ ] { LEAF_THROW(my_error()); } ) );
	BOOST_TEST( 2==test( [ ] { LEAF_THROW(my_error(),info{42}); } ) );

	return boost::report_errors();
}
