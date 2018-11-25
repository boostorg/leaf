//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/expect.hpp>
#include <boost/leaf/preload.hpp>
#include <boost/detail/lightweight_test.hpp>

namespace leaf = boost::leaf;

template <int A>
struct info
{
	int value;
};

leaf::error f0()
{
	auto propagate = leaf::preload( info<0>{0} );
	return leaf::error( info<2>{2} );
}

leaf::error f1()
{
	auto propagate = leaf::preload( info<0>{-1}, info<1>{1}, info<2>{-1} );
	return f0();
}

leaf::error f2()
{
	return f1().propagate( info<4>{4} );
}

int main()
{
	leaf::expect<info<0>,info<1>,info<2>,info<3>,info<4>> exp;
	leaf::error e = f2();
	BOOST_TEST(!leaf::peek<info<3>>(exp,e));
	int c=0;
	bool handled = handle_error( exp, e,
		[&c]( info<1> const & i1, info<2> const & i2, info<4> const & i4 )
		{
			BOOST_TEST(i1.value==1);
			BOOST_TEST(i2.value==2);
			BOOST_TEST(i4.value==4);
			++c;
		} );
	BOOST_TEST(handled);
	BOOST_TEST(c==1);
	return boost::report_errors();
}
