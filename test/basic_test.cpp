//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/expect.hpp>
#include "boost/core/lightweight_test.hpp"

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

int main()
{
	leaf::expect<info<1>,info<2>,info<4>> exp0;
	leaf::error e0 = f4();
	{
		info<1> const * p = exp0.peek<info<1>>(e0);
		BOOST_TEST(p && p->value==1);
	}

	{
		info<2> const * p = exp0.peek<info<2>>(e0);
		BOOST_TEST(p && p->value==2);
	}

	BOOST_TEST(!exp0.peek<info<4>>(e0));
	leaf::expect<info<1>,info<2>,info<4>> exp;
	leaf::error e1 = f4();

	{
		info<1> const * p = exp0.peek<info<1>>(e0);
		BOOST_TEST(p && p->value==1);
	}

	{
		info<2> const * p = exp0.peek<info<2>>(e0);
		BOOST_TEST(p && p->value==2);
	}

	BOOST_TEST(!exp0.peek<info<4>>(e0));
	BOOST_TEST(!exp.peek<info<1>>(e0));
	BOOST_TEST(!exp.peek<info<2>>(e0));
	BOOST_TEST(!exp.peek<info<4>>(e0));

	{
		info<1> const * p = exp.peek<info<1>>(e1);
		BOOST_TEST(p && p->value==1);
	}

	{
		info<2> const * p = exp.peek<info<2>>(e1);
		BOOST_TEST(p && p->value==2);
	}

	BOOST_TEST(!exp.peek<info<4>>(e1));
	BOOST_TEST( !exp.handle_error( e1, [ ](info<1>,info<2>,info<4>)->void { } ) );
	leaf::error e2 = f4();

	{
		info<1> const * p = exp.peek<info<1>>(e2);
		BOOST_TEST(p && p->value==1);
	}

	{
		info<2> const * p = exp.peek<info<2>>(e2);
		BOOST_TEST(p && p->value==2);
	}

	BOOST_TEST(!exp.peek<info<4>>(e2));

	{
		int c1=0, c2=0, c3=0;
		bool handled = exp.handle_error( e2,
			[&c1]( info<1>, info<2>, info<4> )
			{
				++c1;
			},
			[&c2]( info<1>, info<2>, info<4> )
			{
				++c2;
			},
			[&c3]( info<2> const & i2, info<1> const & i1 )
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

	{
		int c=0;
		bool handled = exp0.handle_error( e0,
			[&c]( info<2> const & i2, info<1> const & i1 )
			{
				BOOST_TEST(i1.value==1);
				BOOST_TEST(i2.value==2);
				++c;
			} );
		BOOST_TEST(handled);
		BOOST_TEST(c==1);
	}

	return boost::report_errors();
}
