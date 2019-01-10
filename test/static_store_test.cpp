//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/static_store.hpp>
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int A>
struct info
{
	int value;
};

leaf::error f1()
{
	return leaf::new_error( info<1>{1} );
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
	using leaf::leaf_detail::static_store;

	static_store<info<1>,info<2>,info<4>> exp0;
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
	static_store<info<1>,info<2>,info<4>> exp;
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
	{
		int r = exp.handle_error( e1, 0,
			[ ](info<1>,info<2>,info<4>)
			{
				return 1;
			},
			[ ]
			{
				return 2;
			} );
		BOOST_TEST(r==2);
	}
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
		int r = exp.handle_error( e2, 0,
			[ ]( info<1>, info<2>, info<4> )
			{
				return 1;
			},
			[ ]( info<1>, info<2>, info<4> )
			{
				return 2;
			},
			[ ]( info<2> const & i2, info<1> const & i1 )
			{
				BOOST_TEST(i1.value==1);
				BOOST_TEST(i2.value==2);
				return 3;
			},
			[ ]
			{
				return 4;
			} );
		BOOST_TEST(r==3);
	}

	{
		int r = exp0.handle_error( e0, 0,
			[ ]( info<2> const & i2, info<1> const & i1 )
			{
				BOOST_TEST(i1.value==1);
				BOOST_TEST(i2.value==2);
				return 1;
			},
			[ ]
			{
				return 2;
			} );
		BOOST_TEST(r==1);
	}

	return boost::report_errors();
}
