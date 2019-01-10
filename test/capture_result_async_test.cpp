//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/capture_result.hpp>
#include <boost/leaf/handle.hpp>

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

template <class... E, class F>
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
					return f(a,b,res);
				} ) };
		} );
	return fut;
}

int main()
{
	std::vector<fut_info> fut = launch_tasks<info<1>, info<2>>( 42,
		leaf::capture_result<info<1>,info<2>,info<3>>(
			[ ]( int a, int b, int res ) -> leaf::result<int>
			{
				if( res>=0 )
					return res;
				else
					return leaf::error( info<1>{a}, info<2>{b}, info<3>{} );
			} ) );

	for( auto & f : fut )
	{
		f.fut.wait();
		int r = leaf::handle_all(
			[&]
			{
				return f.fut.get();
			},
			[&]( info<1> const & x1, info<2> const & x2 )
			{
				BOOST_TEST(x1.value==f.a);
				BOOST_TEST(x2.value==f.b);
				return -1;
			},
			[ ]
			{
				return -2;
			} );
		if( f.result>=0 )
			BOOST_TEST(r==f.result);
		else
			BOOST_TEST(r==-1);
	}

	return boost::report_errors();
}
