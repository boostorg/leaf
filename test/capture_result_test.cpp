//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/capture_result.hpp>
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
	std::future<leaf::captured_result<int>> fut;
	};

int
main()
	{
	int const thread_count = 20;
	std::vector<fut_info> fut;
		{
		std::generate_n( std::inserter(fut,fut.end()), thread_count, [ ]
			{
			int const a = rand();
			int const b = rand();
			int const result = (rand()%10) - 5;
			return fut_info { a, b, result, std::async( std::launch::async,
				leaf::capture_result<my_info<1>,my_info<2>,my_info<3>>( [a,b,result]
					{
					auto put = leaf::preload( my_info<1>{a}, my_info<2>{b} );
					if( result>=0 )
						return leaf::result<int>(result);
					else
						return leaf::result<int>(leaf::error(my_info<3>{}));
					} ) ) };
			} );
		}
	for( auto & f : fut )
		{
		using namespace leaf::leaf_detail;
		f.fut.wait();
		leaf::expect<my_info<1>,my_info<2>,my_info<4>> exp;
		if( leaf::result<int> r = unpack(f.fut.get(),exp) )
			{
			BOOST_TEST(*r>=0);
			BOOST_TEST(*r==f.result);
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
