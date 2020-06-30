// Copyright (c) 2018-2020 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/handle_error.hpp>
#include <boost/leaf/pred.hpp>
#include "_test_ec.hpp"
#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

enum class my_error { e1=1, e2, e3 };

struct e_my_error { my_error value; };

struct e_error_code { std::error_code value; };

int main()
{
	{
		int e = 42;

		static_assert(std::is_same<int const &, leaf::match<int, 42>::match_type>::value, "match type deduction error");
		BOOST_TEST(( leaf::match<int, 42>(e) ));
		BOOST_TEST(( !leaf::match<int, 41>(e) ));
		BOOST_TEST(( leaf::match<int, 42, 41>(e) ));
	}

	{
		my_error e = my_error::e1;

		static_assert(std::is_same<my_error const &, leaf::match<my_error, my_error::e1>::match_type>::value, "match type deduction error");
		BOOST_TEST(( leaf::match<my_error, my_error::e1>(e) ));
		BOOST_TEST(( !leaf::match<my_error, my_error::e2>(e) ));
		BOOST_TEST(( leaf::match<my_error, my_error::e2, my_error::e1>(e) ));
	}

	{
		e_my_error e = { my_error::e1 };

		static_assert(std::is_same<e_my_error const &, leaf::match_value<e_my_error, my_error::e1>::match_type>::value, "match type deduction error");
		BOOST_TEST(( leaf::match_value<e_my_error, my_error::e1>(e) ));
		BOOST_TEST(( !leaf::match_value<e_my_error, my_error::e2>(e) ));
		BOOST_TEST(( leaf::match_value<e_my_error, my_error::e2, my_error::e1>(e) ));

#if __cplusplus >= 201703L
		static_assert(std::is_same<e_my_error const &, leaf::match_member<&e_my_error::value, my_error::e1>::match_type>::value, "match type deduction error");
		BOOST_TEST(( leaf::match_member<&e_my_error::value, my_error::e1>(e) ));
		BOOST_TEST(( !leaf::match_member<&e_my_error::value, my_error::e2>(e) ));
		BOOST_TEST(( leaf::match_member<&e_my_error::value, my_error::e2, my_error::e1>(e) ));
#endif
	}

	{
		std::error_code e = errc_a::a0;

		static_assert(std::is_same<std::error_code const &, leaf::match<leaf::condition<cond_x>, cond_x::x00>::match_type>::value, "match type deduction error");
		BOOST_TEST(( leaf::match<leaf::condition<cond_x>, cond_x::x00>(e) ));
		BOOST_TEST(( !leaf::match<leaf::condition<cond_x>, cond_x::x11>(e) ));
		BOOST_TEST(( leaf::match<leaf::condition<cond_x>, cond_x::x11, cond_x::x00>(e) ));

#if __cplusplus >= 201703L
		static_assert(std::is_same<std::error_code const &, leaf::match<std::error_code, errc_a::a0>::match_type>::value, "match type deduction error");
		BOOST_TEST(( leaf::match<std::error_code, errc_a::a0>(e) ));
		BOOST_TEST(( !leaf::match<std::error_code, errc_a::a2>(e) ));
		BOOST_TEST(( leaf::match<std::error_code, errc_a::a2, errc_a::a0>(e) ));
#endif
	}

	{
		e_error_code e = { errc_a::a0 };

		static_assert(std::is_same<e_error_code const &, leaf::match_value<leaf::condition<e_error_code, cond_x>, cond_x::x00>::match_type>::value, "match type deduction error");
		BOOST_TEST(( leaf::match_value<leaf::condition<e_error_code, cond_x>, cond_x::x00>(e) ));
		BOOST_TEST(( !leaf::match_value<leaf::condition<e_error_code, cond_x>, cond_x::x11>(e) ));
		BOOST_TEST(( leaf::match_value<leaf::condition<e_error_code, cond_x>, cond_x::x11, cond_x::x00>(e) ));

#if __cplusplus >= 201703L
		static_assert(std::is_same<e_error_code const &, leaf::match_value<e_error_code, errc_a::a0>::match_type>::value, "match type deduction error");
		BOOST_TEST(( leaf::match_value<e_error_code, errc_a::a0>(e) ));
		BOOST_TEST(( !leaf::match_value<e_error_code, errc_a::a2>(e) ));
		BOOST_TEST(( leaf::match_value<e_error_code, errc_a::a2, errc_a::a0>(e) ));

		static_assert(std::is_same<e_error_code const &, leaf::match_member<&e_error_code::value, errc_a::a0>::match_type>::value, "match type deduction error");
		BOOST_TEST(( leaf::match_member<&e_error_code::value, errc_a::a0>(e) ));
		BOOST_TEST(( !leaf::match_member<&e_error_code::value, errc_a::a2>(e) ));
		BOOST_TEST(( leaf::match_member<&e_error_code::value, errc_a::a2, errc_a::a0>(e) ));
#endif
	}

	return boost::report_errors();
}
