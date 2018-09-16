//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/transport.hpp>
#include <boost/leaf/put.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <future>
#include <algorithm>

namespace leaf = boost::leaf;

struct error { };
template <int> struct my_info { int value; };

struct
fut_info
	{
	int a;
	int b;
	std::future<void> fut;
	};

int
main()
	{
	int const thread_count = 20;
	std::vector<fut_info> fut;
		{
		std::generate_n( std::inserter(fut,fut.end()), thread_count, [ ]
			{
			int const a=rand();
			int const b=rand();
			auto trf = leaf::transport<my_info<1>,my_info<2>>( [a,b]
				{
				auto put = leaf::preload( my_info<1>{a}, my_info<2>{b} );
				throw error();
				} );
			if( rand()%2 )
				return fut_info { a, b, std::async( std::launch::async, trf ) };
			else
				{
				std::packaged_task<void()> task( trf );
				std::future<void> fut = task.get_future();
				std::thread(std::move(task)).detach();
				return fut_info { a, b, std::move(fut) };
				}
			} );
		}
	for( auto & f : fut )
		{
		using namespace leaf::leaf_detail;
		f.fut.wait();
		try
			{
			leaf::get([&f]{f.fut.get();});
			BOOST_TEST(false);
			}
		catch( error const & )
			{
			leaf::available info;
			BOOST_TEST(!tl_slot<my_info<1>>::tl_instance().is_open());
			BOOST_TEST(!tl_slot<my_info<2>>::tl_instance().is_open());
			unwrap( info.match<my_info<1>,my_info<2>>( [&f]( int x1, int x2 )
				{
				BOOST_TEST(x1==f.a);
				BOOST_TEST(x2==f.b);
				} ) );
			BOOST_TEST(!tl_slot<my_info<1>>::tl_instance().is_open());
			BOOST_TEST(!tl_slot<my_info<2>>::tl_instance().is_open());
			BOOST_TEST(tl_slot<my_info<1>>::tl_instance().has_value());
			BOOST_TEST(tl_slot<my_info<2>>::tl_instance().has_value());
			}
		BOOST_TEST(!tl_slot<my_info<1>>::tl_instance().has_value());
		BOOST_TEST(!tl_slot<my_info<2>>::tl_instance().has_value());
		}
	return boost::report_errors();
	}
