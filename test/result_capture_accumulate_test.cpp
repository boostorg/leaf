// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/capture.hpp>
#include <boost/leaf/result.hpp>
#include <boost/leaf/handle_error.hpp>
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

struct info
{
	int value;
};

int main()
{
	auto handle_error = [ ]( leaf::error_info const & error )
	{
		return leaf::remote_handle_all( error,
			[  ]( info x )
			{
				BOOST_TEST_EQ(x.value, 2);
				return 1;
			},
			[ ]
			{
				return 2;
			} );
	};

	leaf::result<void> r = leaf::capture( leaf::make_shared_context(&handle_error),
		[ ]() -> leaf::result<void>
		{
			return leaf::new_error( info{1} );
		} );

	r.accumulate( [ ]( info & x ) { ++x.value; } );

	int rr = leaf::remote_try_handle_all(
		[&]() -> leaf::result<int>
		{
			LEAF_CHECK(r);
			return 0;
		},
		[&]( leaf::error_info const & error )
		{
			return handle_error(error);
		} );

	BOOST_TEST_EQ(rr, 1);
	return boost::report_errors();
}
