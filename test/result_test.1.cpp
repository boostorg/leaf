//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/result.hpp>
#include <boost/leaf/expect.hpp>
#include <boost/leaf/preload.hpp>
#include <boost/detail/lightweight_test.hpp>

namespace leaf = boost::leaf;

struct my_value
{
};

template <int A>
struct info
{
	int value;
};

leaf::result<my_value> f1( bool success )
{
	if( success )
		return { };
	else
		return leaf::error( info<1>{1} );
}

leaf::result<my_value> f2( bool success )
{
	leaf::expect<info<1>> exp;
	if( leaf::result<my_value> r=f1(success) )
		return r;
	else
		return r.error( info<2>{2} );
}

leaf::result<my_value> f3( bool success )
{
	leaf::expect<info<2>,info<3>> exp;
	auto propagate = leaf::preload( info<4>{4} );
	return f2(success);
}

leaf::result<my_value> f4( bool success )
{
	leaf::expect<info<1>,info<2>,info<3>,info<4>> exp;
	if( leaf::result<my_value> r = f3( success ) )
		return r;
	else
	{
		int c1=0, c2=0;
		bool handled = handle_error( exp, r,
			[&c1]( info<1>, info<2>, info<3>, info<4> )
			{
				++c1;
			},
			[&c2]( info<1> const & i1, info<2> const & i2, info<4> const & i4 )
			{
				BOOST_TEST(i1.value==1);
				BOOST_TEST(i2.value==2);
				BOOST_TEST(i4.value==4);
				++c2;
			} );
		BOOST_TEST(handled);
		BOOST_TEST(c1==0);
		BOOST_TEST(c2==1);
		return leaf::error();
	}
}

int main()
{
	leaf::expect<info<2>,info<3>,info<4>> exp;
	BOOST_TEST(f4(true));
	leaf::result<my_value> r=f4(false);
	BOOST_TEST(!r);
	leaf::error e = r.error();
	BOOST_TEST(!leaf::peek<info<2>>(exp,e));
	BOOST_TEST(!leaf::peek<info<3>>(exp,e));
	BOOST_TEST(!leaf::peek<info<4>>(exp,e));
	BOOST_TEST(leaf::leaf_detail::tl_slot_ptr<info<1>>()==0);
	BOOST_TEST(leaf::leaf_detail::tl_slot_ptr<info<2>>()!=0);
	BOOST_TEST(leaf::leaf_detail::tl_slot_ptr<info<3>>()!=0);
	BOOST_TEST(leaf::leaf_detail::tl_slot_ptr<info<4>>()!=0);
	return boost::report_errors();
}
