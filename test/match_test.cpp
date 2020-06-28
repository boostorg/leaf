// Copyright (c) 2018-2020 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/handle_error.hpp>
#include "_test_ec.hpp"
#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

enum class my_error { e1=1, e2, e3 };

struct e_my_error { my_error value; };
struct e_my_error_f { my_error v; my_error value() const { return v; } };

int main()
{
	{
		my_error ec = my_error::e1;
		static_assert(std::is_same<my_error, leaf::match<my_error, my_error::e1>::match_type>::value, "match type deduction error");
		BOOST_TEST(( leaf::match<my_error, my_error::e1>(ec) ));
		BOOST_TEST(( !leaf::match<my_error, my_error::e2>(ec) ));
		BOOST_TEST(( leaf::match<my_error, my_error::e2, my_error::e1>(ec) ));
	}

	{
		e_my_error ec = { my_error::e1 };
		static_assert(std::is_same<e_my_error const &, leaf::match<e_my_error, my_error::e1>::match_type>::value, "match type deduction error");
		BOOST_TEST(( leaf::match<e_my_error, my_error::e1>(ec) ));
		BOOST_TEST(( !leaf::match<e_my_error, my_error::e2>(ec) ));
		BOOST_TEST(( leaf::match<e_my_error, my_error::e2, my_error::e1>(ec) ));
	}

	{
		e_my_error_f ec = { my_error::e1 };
		static_assert(std::is_same<e_my_error_f const &, leaf::match<e_my_error_f, my_error::e1>::match_type>::value, "match type deduction error");
		BOOST_TEST(( leaf::match<e_my_error_f, my_error::e1>(ec) ));
		BOOST_TEST(( !leaf::match<e_my_error_f, my_error::e2>(ec) ));
		BOOST_TEST(( leaf::match<e_my_error_f, my_error::e2, my_error::e1>(ec) ));
	}

	{
		std::error_code ec = errc_a::a0;

		static_assert(std::is_same<std::error_code const &, leaf::match<leaf::condition<cond_x>, cond_x::x00>::match_type>::value, "match type deduction error");
		BOOST_TEST(( leaf::match<leaf::condition<cond_x>, cond_x::x00>(ec) ));
		BOOST_TEST(( !leaf::match<leaf::condition<cond_x>, cond_x::x11>(ec) ));
		BOOST_TEST(( leaf::match<leaf::condition<cond_x>, cond_x::x11, cond_x::x00>(ec) ));

#if __cplusplus >= 201703L
		static_assert(std::is_same<std::error_code const &, leaf::match<std::error_code, errc_a::a0>::match_type>::value, "match type deduction error");
		BOOST_TEST(( leaf::match<std::error_code, errc_a::a0>(ec) ));
		BOOST_TEST(( !leaf::match<std::error_code, errc_a::a2>(ec) ));
		BOOST_TEST(( leaf::match<std::error_code, errc_a::a2, errc_a::a0>(ec) ));
#endif
	}

	return boost::report_errors();
}
