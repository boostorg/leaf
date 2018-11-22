//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/expect.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <string.h>

namespace leaf = boost::leaf;

template <int A>
struct info
{
	int value;
};

leaf::error f1()
{
	return LEAF_ERROR( info<1>{1} );
}

leaf::error f2()
{
	leaf::expect<info<1>> exp;
	return f1().propagate( info<2>{2} );
}

leaf::error f3()
{
	leaf::expect<info<2>,info<3>> exp;
	return f2().propagate( info<4>{4} );
}

leaf::error f4()
{
	leaf::expect<leaf::e_source_location,info<1>,info<2>,info<3>,info<4>> exp;
	{
		leaf::error e = f3();
		BOOST_TEST( handle_error( exp, e,
			leaf::match<info<1>,info<2>,info<3>,info<4>>(),
			leaf::match<info<1>,info<2>,info<4>>() ) );
	}
	leaf::error e = f3();
	int c1=0, c2=0;
	BOOST_TEST( handle_error( exp, e,
		leaf::match<info<1>,info<2>,info<3>,info<4>>( [&c1]( int, int, int, int )
		{
			++c1;
		} ),
		leaf::match<leaf::e_source_location,info<1>,info<2>,info<4>>( [&c2]( leaf::e_source_location::loc const & loc,int i1, int i2, int i4 )
		{
			BOOST_TEST(loc.line==21);
			BOOST_TEST(strcmp(loc.file,__FILE__)==0);
			BOOST_TEST(strstr(loc.function,"f1")!=0);
			BOOST_TEST(i1==1);
			BOOST_TEST(i2==2);
			BOOST_TEST(i4==4);
			++c2;
		} ) ) );
	BOOST_TEST(c1==0);
	BOOST_TEST(c2==1);
	return leaf::error();
}

int main()
{
	leaf::expect<info<2>,info<3>,info<4>> exp;
	leaf::error e=f4();
	BOOST_TEST(!leaf::peek<info<2>>(exp,e));
	BOOST_TEST(!leaf::peek<info<3>>(exp,e));
	BOOST_TEST(!leaf::peek<info<4>>(exp,e));
	BOOST_TEST(leaf::leaf_detail::tl_slot_ptr<info<1>>()==0);
	BOOST_TEST(leaf::leaf_detail::tl_slot_ptr<info<2>>()!=0);
	BOOST_TEST(leaf::leaf_detail::tl_slot_ptr<info<3>>()!=0);
	BOOST_TEST(leaf::leaf_detail::tl_slot_ptr<info<4>>()!=0);
	return boost::report_errors();
}
