// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/error_code.hpp>
#include <boost/leaf/handle.hpp>
#include "boost/core/lightweight_test.hpp"
#include <cstring>

namespace leaf = boost::leaf;

template <int> struct info { int value; };

int main()
{
	{
		int r = leaf::handle_all(
			[ ]
			{
				return std::error_code();
			},
			[ ]
			{
				return ENOENT;
			} );
		BOOST_TEST(!r);
	}
	{
		int r = leaf::handle_all(
			[&]
			{
				std::error_code ec = leaf::new_error(info<1>{1}).to_error_code();
				BOOST_TEST(ec);
				BOOST_TEST(ec.message()=="LEAF error, use with leaf::handle_some or leaf::handle_all.");
				BOOST_TEST(!std::strcmp(ec.category().name(),"LEAF error, use with leaf::handle_some or leaf::handle_all."));
				return ec;
			},
			[ ]( info<1> const & x )
			{
				BOOST_TEST(x.value==1);
				return 1;
			},
			[ ]
			{
				return 2;
			} );
		BOOST_TEST(r==1);
	}
	{
		int r = leaf::handle_all(
			[&]
			{
				return leaf::new_error(info<2>{2},std::error_code(ENOENT,std::system_category())).to_error_code();
			},
			[ ]( info<2> const & x, std::error_code const & ec )
			{
				BOOST_TEST(x.value==2);
				BOOST_TEST(ec==std::error_code(ENOENT,std::system_category()));
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
