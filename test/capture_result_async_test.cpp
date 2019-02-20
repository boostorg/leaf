// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/capture.hpp>
#include <boost/leaf/result.hpp>
#include <boost/leaf/handle_error.hpp>
#include "boost/core/lightweight_test.hpp"
#include <vector>
#include <future>
#include <iterator>
#include <algorithm>

namespace leaf = boost::leaf;

template <int> struct info { int value; };

struct fut_info
{
	int a;
	int b;
	int result;
	std::future<leaf::result<int>> fut;
};

template <class H, class F>
std::vector<fut_info> launch_tasks( int task_count, F f )
{
	assert(task_count>0);
	std::vector<fut_info> fut;
	std::generate_n( std::inserter(fut,fut.end()), task_count,
		[=]
		{
			int const a = rand();
			int const b = rand();
			int const res = (rand()%10) - 5;
			return fut_info { a, b, res, std::async( std::launch::async,
				[=]
				{
					return leaf::capture(leaf::make_shared_context<H>(), f, a, b, res);
				} ) };
		} );
	return fut;
}

int main()
{
	auto error_handler = []( leaf::error_info const & err, int a, int b )
	{
		return leaf::remote_handle_all( err,
			[&]( info<1> const & x1, info<2> const & x2 )
			{
				BOOST_TEST_EQ(x1.value, a);
				BOOST_TEST_EQ(x2.value, b);
				return -1;
			},
			[]
			{
				return -2;
			} );
	};

	std::vector<fut_info> fut = launch_tasks<decltype(error_handler)>(
		42,
		[]( int a, int b, int res ) -> leaf::result<int>
		{
			if( res>=0 )
				return res;
			else
				return leaf::new_error( info<1>{a}, info<2>{b}, info<3>{} );
		} );

	for( auto & f : fut )
	{
		f.fut.wait();
		int r = leaf::remote_try_handle_all(
			[&]
			{
				return f.fut.get();
			},
			[&]( leaf::error_info const & err )
			{
				return error_handler(err, f.a, f.b);
			} );
		if( f.result>=0 )
			BOOST_TEST_EQ(r, f.result);
		else
			BOOST_TEST_EQ(r, -1);
	}

	return boost::report_errors();
}
