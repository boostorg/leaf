//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/expect.hpp>
#include <boost/leaf/preload.hpp>
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

struct info
{
	int value;
};

leaf::error f0()
{
	return leaf::error();
}

leaf::error f1()
{
	auto propagate = leaf::preload( info{42} );
	return f0();
}

int main()
{
	leaf::expect<leaf::e_unexpected,leaf::e_unexpected_diagnostic_output> exp;
	leaf::error e = f1();
	int c=0;
	bool handled = exp.handle_error( e,
		[&c]( leaf::e_unexpected const & unx, leaf::e_unexpected_diagnostic_output const & unxdo )
		{
			BOOST_TEST(unx.count==1);
			BOOST_TEST(unx.first_type==&leaf::type<info>);
			BOOST_TEST(unxdo.value.find(": 42")!=std::string::npos);
			++c;
		} );
	BOOST_TEST(handled);
	BOOST_TEST(c==1);
	return boost::report_errors();
}
