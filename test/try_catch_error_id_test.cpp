// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/handle_exception.hpp>
#include <boost/leaf/exception.hpp>
#include <boost/leaf/preload.hpp>
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

struct info { int value; };

struct my_error: std::exception { };

int main()
{
	int r = leaf::try_catch(
		[]() -> int
		{
			throw leaf::exception( my_error(), info{42} );
		},
		[]( leaf::catch_<my_error> x, leaf::catch_<leaf::error_id> id )
		{
			BOOST_TEST(dynamic_cast<my_error const *>(&x.value())!=0);
			BOOST_TEST(dynamic_cast<leaf::error_id const *>(&id.value())!=0 && dynamic_cast<leaf::error_id const *>(&id.value())->value()==1);
			return 1;
		},
		[]
		{
			return 2;
		} );
	BOOST_TEST_EQ(r, 1);
	return boost::report_errors();
}
