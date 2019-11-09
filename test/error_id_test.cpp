// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/error.hpp>
#include "lightweight_test.hpp"
#include <future>
#include <vector>
#include <algorithm>
#include <iterator>

namespace leaf = boost::leaf;

constexpr int ids_per_thread = 10000;

std::vector<int> generate_ids()
{
	std::vector<int> ids;
	ids.reserve(ids_per_thread);
#ifndef _MSC_VER
	//This test is to ensure that the TL objects are initialized to the correct
	//values (which are implementation details). However, on MSVC std::async
	//reuses threads.
	BOOST_TEST_EQ(leaf::leaf_detail::last_id(), 0);
#endif
	for(int i=0; i!=ids_per_thread-1; ++i)
	{
		int next = leaf::leaf_detail::next_id();
		BOOST_TEST_EQ(next, leaf::leaf_detail::next_id());
		BOOST_TEST_NE(next&1, 0);
		int id = leaf::leaf_detail::new_id();
		BOOST_TEST_NE(id&1, 0);
		int last = leaf::leaf_detail::last_id();
		BOOST_TEST_EQ(last, leaf::leaf_detail::last_id());
		BOOST_TEST_NE(last&1, 0);
		BOOST_TEST_EQ(next, id);
		BOOST_TEST_EQ(last, id);
		ids.push_back(id);
		BOOST_TEST_NE(leaf::leaf_detail::next_id(), id);
	}
	return ids;
}

int main()
{
	{
		leaf::error_id e1;
		leaf::error_id e2;
		BOOST_TEST(!e1);
		BOOST_TEST_EQ(e1.value(), 0);
		BOOST_TEST(!e2);
		BOOST_TEST_EQ(e2.value(), 0);
		BOOST_TEST(e1==e2);
		BOOST_TEST(!(e1!=e2));
		BOOST_TEST(!(e1<e2));
		BOOST_TEST(!(e2<e1));
	}
	{
		leaf::error_id e1;
		leaf::error_id e2 = leaf::new_error();
		BOOST_TEST(!e1);
		BOOST_TEST_EQ(e1.value(), 0);
		BOOST_TEST(e2);
		BOOST_TEST_EQ(e2.value(), 1);
		BOOST_TEST(!(e1==e2));
		BOOST_TEST(e1!=e2);
		BOOST_TEST(e1<e2);
		BOOST_TEST(!(e2<e1));
	}
	{
		leaf::error_id e1 = leaf::new_error();
		leaf::error_id e2 = leaf::new_error();
		BOOST_TEST(e1);
		BOOST_TEST_EQ(e1.value(), 3);
		BOOST_TEST(e2);
		BOOST_TEST_EQ(e2.value(), 5);
		BOOST_TEST(!(e1==e2));
		BOOST_TEST(e1!=e2);
		BOOST_TEST(e1<e2);
		BOOST_TEST(!(e2<e1));
	}
	{
		leaf::error_id e1 = leaf::new_error();
		leaf::error_id e2 = e1;
		BOOST_TEST(e1);
		BOOST_TEST_EQ(e1.value(), 7);
		BOOST_TEST(e2);
		BOOST_TEST_EQ(e2.value(), 7);
		BOOST_TEST(e1==e2);
		BOOST_TEST(!(e1!=e2));
		BOOST_TEST(!(e1<e2));
		BOOST_TEST(!(e2<e1));
	}
	{
		leaf::error_id e1 = leaf::new_error();
		leaf::error_id e2; e2 = e1;
		BOOST_TEST(e1);
		BOOST_TEST_EQ(e1.value(), 9);
		BOOST_TEST(e2);
		BOOST_TEST_EQ(e2.value(), 9);
		BOOST_TEST(e1==e2);
		BOOST_TEST(!(e1!=e2));
		BOOST_TEST(!(e1<e2));
		BOOST_TEST(!(e2<e1));
	}
#ifdef LEAF_NO_THREADS
	std::vector<int> all_ids = generate_ids();
#else
	constexpr int thread_count = 100;
	using thread_ids = std::future<std::vector<int>>;
	std::vector<thread_ids> fut;
	fut.reserve(thread_count);
	std::generate_n(
		std::inserter(fut,fut.end()),
		thread_count,
		[=]
		{
			return std::async(std::launch::async, &generate_ids);
		});
	std::vector<int> all_ids;
	for(auto & f : fut)
	{
		auto fv = f.get();
		all_ids.insert(all_ids.end(), fv.begin(), fv.end());
	}
#endif
	std::sort(all_ids.begin(), all_ids.end());
	auto u = std::unique(all_ids.begin(), all_ids.end());
	BOOST_TEST(u==all_ids.end());
	return boost::report_errors();
}
