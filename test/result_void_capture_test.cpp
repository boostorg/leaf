//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/result.hpp>
#include <boost/leaf/expect.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <vector>
#include <future>
#include <iterator>
#include <algorithm>

namespace leaf = boost::leaf;

template <int> struct my_info { int value; };

struct
fut_info
	{
	int a;
	int b;
	int result;
	std::future<leaf::result<void>> fut;
	};
template <class F>
std::vector<fut_info>
launch_tasks( int task_count, F f )
	{
	assert(task_count>0);
	std::vector<fut_info> fut;
	std::generate_n( std::inserter(fut,fut.end()), task_count, [f]
		{
		int const a = rand();
		int const b = rand();
		int const res = (rand()%10) - 5;
		return fut_info { a, b, res, std::async( std::launch::async, [f,a,b,res]
			{
			leaf::expect<my_info<1>,my_info<2>,my_info<3>> exp;
			return capture(exp,f(a,b,res));
			} ) };
		} );
	return fut;
	}
int
main()
	{
	std::vector<fut_info> fut = launch_tasks( 42, [ ]( int a, int b, int res ) noexcept -> leaf::result<void>
		{
		if( res>=0 )
			return { };
		else
			return leaf::error(my_info<1>{a},my_info<2>{b},my_info<3>{});
		} );
	for( auto & f : fut )
		{
		using namespace leaf::leaf_detail;
		f.fut.wait();
		leaf::expect<my_info<1>,my_info<2>,my_info<4>> exp;
		if( leaf::result<void> r = f.fut.get() )
			{
			}
		else
			{
			handle_error( exp, r, leaf::match<my_info<1>,my_info<2>>( [&f]( int x1, int x2 )
				{
				BOOST_TEST(x1==f.a);
				BOOST_TEST(x2==f.b);
				} ) );
			}
		}
	return boost::report_errors();
	}
