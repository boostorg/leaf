//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/try.hpp>
#include <boost/leaf/result.hpp>
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

int check( leaf::catch_<leaf::bad_result>, leaf::e_source_location const & x )
{
	BOOST_TEST(strstr(x.file,"result.hpp")!=0);
	BOOST_TEST(x.line>0);
	BOOST_TEST(strstr(x.function,"value")!=0);
	return 1;
}

int main()
{
	{
		int r = leaf::try_(
			[ ]
			{
				leaf::result<int> r = leaf::new_error();
				(void) r.value();
				return 0;
			},
			check );
		BOOST_TEST(r==1);
	}
	{
		int r = leaf::try_(
			[ ]
			{
				leaf::result<int> const r = leaf::new_error();
				(void) r.value();
				return 0;
			},
			check );
		BOOST_TEST(r==1);
	}
	{
		int r = leaf::try_(
			[ ]
			{
				leaf::result<int> r = leaf::new_error();
				(void) *r;
				return 0;
			},
			check );
		BOOST_TEST(r==1);
	}
	{
		int r = leaf::try_(
			[ ]
			{
				leaf::result<int> const r = leaf::new_error();
				(void) *r;
				return 0;
			},
			check );
		BOOST_TEST(r==1);
	}
	return boost::report_errors();
}
