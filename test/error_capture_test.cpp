//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/error_capture.hpp>
#include <boost/leaf/expect.hpp>
#include <boost/detail/lightweight_test.hpp>

namespace leaf = boost::leaf;

template <int A>
struct info
{
	int value;
};

leaf::error f1()
{
	return leaf::error( info<1>{1} );
}

leaf::error f2()
{
	return f1().propagate( info<2>{2} );
}

leaf::error f3()
{
	return f2().propagate( info<3>{3} );
}

leaf::error f4()
{
	return f3().propagate();
}

leaf::error_capture make_capture()
{
	leaf::expect<info<1>,info<2>,info<4>> exp;
	return capture(exp,f4());
}

int main()
{
	leaf::error_capture e1 = make_capture();
	BOOST_TEST(e1);

	{
		info<1> const * p = leaf::peek<info<1>>(e1);
		BOOST_TEST(p && p->value==1);
	}

	{
		info<2> const * p = leaf::peek<info<2>>(e1);
		BOOST_TEST(p && p->value==2);
	}

	BOOST_TEST(!leaf::peek<info<3>>(e1));
	leaf::expect<info<1>,info<2>,info<4>> exp;
	leaf::error e2 = f4();

	{
		info<1> const * p = leaf::peek<info<1>>(e1);
		BOOST_TEST(p && p->value==1);
	}

	{
		info<2> const * p = leaf::peek<info<2>>(e1);
		BOOST_TEST(p && p->value==2);
	}

	BOOST_TEST(!leaf::peek<info<3>>(e1));

	{
		info<1> const * p = leaf::peek<info<1>>(exp,e2);
		BOOST_TEST(p && p->value==1);
	}

	{
		info<2> const * p = leaf::peek<info<2>>(exp,e2);
		BOOST_TEST(p && p->value==2);
	}

	BOOST_TEST(!leaf::peek<info<4>>(exp,e2));

	{
		int c1=0, c2=0;
		bool handled = handle_error( exp, e2,
			[&c1]( info<1>, info<2>, info<4> )
			{
				++c1;
			},
			[&c2]( info<2> const & i2,info<1> const &  i1 )
			{
				BOOST_TEST(i1.value==1);
				BOOST_TEST(i2.value==2);
				++c2;
			} );
		BOOST_TEST(handled);
		BOOST_TEST(c1==0);
		BOOST_TEST(c2==1);
	}

	{
		int c1=0, c2=0, c3=0;
		bool handled = handle_error( e1,
			[&c1]( info<1>, info<2>, info<3> )
			{
				++c1;
			},
			[&c2]( info<1>, info<2>, info<4> )
			{
				++c2;
			},
			[&c3]( info<2> const & i2,info<1> const & i1 )
			{
				BOOST_TEST(i1.value==1);
				BOOST_TEST(i2.value==2);
				++c3;
			} );
		BOOST_TEST(handled);
		BOOST_TEST(c1==0);
		BOOST_TEST(c2==0);
		BOOST_TEST(c3==1);
	}

	BOOST_TEST( handle_error( e1,
		[ ] ( info<1>, info<2>, info<3> ) { },
		[ ] ( info<1>, info<2>, info<4> ) { },
		[ ] ( info<2>, info<1> ) { } ) );

	return boost::report_errors();
	}
