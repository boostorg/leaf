// Copyright (c) 2018-2020 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/handle_error.hpp>
#include "lightweight_test.hpp"

using boost::leaf::leaf_detail::flatten_tuple;

static_assert(std::is_same<
	decltype(flatten_tuple(int{}, float{})),
	std::tuple<int, float>>
::value);

static_assert(std::is_same<
	decltype(flatten_tuple(std::tuple<int,float>{}, std::tuple<short,long>{})),
	std::tuple<int, float, short, long>>
::value);

static_assert(std::is_same<
	decltype(flatten_tuple(std::tuple<int,float>{}, short{}, long{})),
	std::tuple<int, float, short, long>>
::value);

static_assert(std::is_same<
	decltype(flatten_tuple(int{}, std::tuple<float, short>{}, long{})),
	std::tuple<int, float, short, long>>
::value);

int main()
{
	BOOST_TEST(
		flatten_tuple(int{1}, float{2}, std::make_tuple(short{3},long{4}))
		==
		std::make_tuple(int{1}, float{2}, short{3}, long{4}));

	BOOST_TEST(
		flatten_tuple(std::make_tuple(int{1}, float{2}), std::make_tuple(short{3},long{4}))
		==
		std::make_tuple(int{1}, float{2}, short{3}, long{4}));

	BOOST_TEST(
		flatten_tuple(std::make_tuple(int{1}, float{2}), short{3}, long{4})
		==
		std::make_tuple(int{1}, float{2}, short{3}, long{4}));

	BOOST_TEST(
		flatten_tuple(int{1}, std::make_tuple(float{2}, short{3}), long{4})
		==
		std::make_tuple(int{1}, float{2}, short{3}, long{4}));

	{
		int i = 1;
		float f = 2;
		auto t = std::make_tuple(short{3},long{4});
		BOOST_TEST(
			flatten_tuple(i, f, t)
			==
			std::make_tuple(int{1}, float{2}, short{3}, long{4}));
	}

	{
		int const i = 1;
		float const f = 2;
		auto const t = std::make_tuple(short{3},long{4});
		BOOST_TEST(
			flatten_tuple(i, f, t)
			==
			std::make_tuple(int{1}, float{2}, short{3}, long{4}));
	}

	{
		int const & i = 1;
		float const & f = 2;
		auto const & t = std::make_tuple(short{3},long{4});
		BOOST_TEST(
			flatten_tuple(i, f, t)
			==
			std::make_tuple(int{1}, float{2}, short{3}, long{4}));
	}

	return boost::report_errors();
}
