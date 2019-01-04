//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/expect.hpp>
#include "boost/core/lightweight_test.hpp"
#include <string.h>

namespace leaf = boost::leaf;

template <int A>
struct unexp
{
	int value;
};

template <int A>
struct info
{
	int value;
};

leaf::error f1()
{
	return LEAF_ERROR( info<1>{1}, unexp<1>{1}, unexp<2>{2} );
}

leaf::error f2()
{
	leaf::expect<info<1>> exp;
	return f1().propagate( info<2>{2} );
}

leaf::error f3()
{
	leaf::expect<info<2>,info<3>,unexp<1>> exp;
	leaf::error e = f2().propagate( info<4>{4} );
	BOOST_TEST(exp.peek<unexp<1>>(e)->value==1);
	return e;
}

leaf::error f4()
{
	leaf::expect<leaf::e_source_location,leaf::e_unexpected,info<1>,info<2>,info<3>,info<4>> exp;
	{
		leaf::error e = f3();
		bool handled = exp.handle_error( e,
			[ ]( info<1>, info<2>, info<3>, info<4> ){ },
			[ ]( info<1>, info<2>, info<4> ) { } );
		BOOST_TEST(handled);
	}
	leaf::error e = f3();
	int c1=0, c2=0;
	bool handled = exp.handle_error( e,
		[&c1]( info<1>,info<2>,info<3>,info<4> )
		{
			++c1;
		},
		[&c2]( leaf::e_source_location const & loc, leaf::e_unexpected const & unx, info<1> const & i1, info<2> const & i2, info<4> const & i4 )
		{
			BOOST_TEST(loc.line==27);
			BOOST_TEST(strcmp(loc.file,__FILE__)==0);
			BOOST_TEST(strstr(loc.function,"f1")!=0);
			BOOST_TEST(unx.count==2);
			BOOST_TEST(unx.first_type==&leaf::type<unexp<2>>);
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

int main()
{
	leaf::expect<info<2>,info<3>,info<4>> exp;
	leaf::error e=f4();
	BOOST_TEST(!exp.peek<info<2>>(e));
	BOOST_TEST(!exp.peek<info<3>>(e));
	BOOST_TEST(!exp.peek<info<4>>(e));
	BOOST_TEST(leaf::leaf_detail::tl_slot_ptr<info<1>>()==0);
	BOOST_TEST(leaf::leaf_detail::tl_slot_ptr<info<2>>()!=0);
	BOOST_TEST(leaf::leaf_detail::tl_slot_ptr<info<3>>()!=0);
	BOOST_TEST(leaf::leaf_detail::tl_slot_ptr<info<4>>()!=0);
	return boost::report_errors();
}
