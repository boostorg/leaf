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

int main()
{
	constexpr int thread_count = 100;
	constexpr int ids_per_thread = 10000;
	using thread_ids = std::future<std::vector<int>>;
	std::vector<thread_ids> fut;
	fut.reserve(thread_count);
	std::generate_n(
		std::inserter(fut,fut.end()),
		thread_count,
		[=]
		{
			return std::async(
				std::launch::async,
				[=]
				{
					std::vector<int> ids;
					ids.reserve(ids_per_thread);
					BOOST_TEST(leaf::leaf_detail::last_id()==0);
					for(int i=0; i!=ids_per_thread-1; ++i)
					{
						int next = leaf::leaf_detail::next_id();
						BOOST_TEST(next==leaf::leaf_detail::next_id());
						BOOST_TEST(next&1);
						int id = leaf::leaf_detail::new_id();
						BOOST_TEST(id&1);
						int last = leaf::leaf_detail::last_id();
						BOOST_TEST(last==leaf::leaf_detail::last_id());
						BOOST_TEST(last&1);
						BOOST_TEST(next==id);
						BOOST_TEST(last==id);
						ids.push_back(id);
						BOOST_TEST(leaf::leaf_detail::next_id()!=id);
					}
					return ids;
				});
		});
	std::vector<int> all_ids;
	for(auto & f : fut)
	{
		auto fv = f.get();
		all_ids.insert(all_ids.end(), fv.begin(), fv.end());
	}
	std::sort(all_ids.begin(), all_ids.end());
	auto u = std::unique(all_ids.begin(), all_ids.end());
	BOOST_TEST(u==all_ids.end());
	return boost::report_errors();
}
