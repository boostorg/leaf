// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>
#if defined(LEAF_NO_EXCEPTIONS) || defined(LEAF_NO_THREADS)

#include <iostream>

int main()
{
	std::cout << "Unit test not applicable." << std::endl;
	return 0;
}

#else

#include <boost/leaf/capture.hpp>
#include <boost/leaf/handle_exception.hpp>
#include <boost/leaf/exception.hpp>
#include <boost/leaf/on_error.hpp>
#include "lightweight_test.hpp"
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
	std::future<int> fut;
};

template <class H, class F>
std::vector<fut_info> launch_tasks( int task_count, F f )
{
	BOOST_LEAF_ASSERT(task_count>0);
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
		return leaf::remote_handle_exception( err,
			[&]( info<1> const & x1, info<2> const & x2, info<4> const & )
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

	{
		std::vector<fut_info> fut = launch_tasks<decltype(error_handler)>(
			42,
			[]( int a, int b, int res )
			{
				if( res >= 0 )
					return res;
				else
					throw leaf::exception( std::exception(), info<1>{a}, info<2>{b}, info<3>{} );
			} );

		for( auto & f : fut )
		{
			f.fut.wait();
			int r = leaf::remote_try_catch(
				[&]
				{
					auto load = leaf::on_error( info<4>{} );

					// Calling future_get is required in order to make the preload (above) work.
					return leaf::future_get(f.fut);
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
	}

	{
		std::vector<fut_info> fut = launch_tasks<decltype(error_handler)>(
			42,
			[]( int a, int b, int res )
			{
				if( res >= 0 )
					return res;
				else
					throw leaf::exception( std::exception(), info<1>{a}, info<2>{b}, info<3>{} );
			} );

		for( auto & f : fut )
		{
			f.fut.wait();
			int r = leaf::remote_try_catch(
				[&]
				{
					auto load = leaf::on_error( info<4>{} );

					return leaf::try_catch(
						[&]
						{
							// Not calling future_get, a preload in this scope won't work correctly.
							// This is to verify that the preload in the outer scope (above) works.
							return f.fut.get();
						},
						[]() -> int
						{
							throw;
						} );
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
	}

	return boost::report_errors();
}

#endif
