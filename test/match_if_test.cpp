// Copyright (c) 2018-2020 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/handle_error.hpp>
#include <boost/leaf/pred.hpp>
#include <boost/leaf/result.hpp>
#include <exception>
#include "_test_ec.hpp"
#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

enum class my_error { e1=1, e2, e3 };

struct my_exception: std::exception
{
	int value;
};

template <my_error value>
bool cmp_my_error( my_error const & e )
{
	return e == value;
};

template <int value>
bool cmp_my_exception( my_exception const & e );

static_assert(!leaf::leaf_detail::handler_argument_traits<leaf::match_if<my_error, cmp_my_error<my_error::e1>>>::requires_catch, "requires_catch deduction error");
#if __cplusplus >= 201703L
static_assert(leaf::leaf_detail::handler_argument_traits<leaf::match_if<my_exception, cmp_my_exception<42>>>::requires_catch, "requires_catch deduction error");
#endif

template <class M, class E>
bool test(E const & e )
{
	if( M::evaluate(e) )
	{
		M m(e);
		BOOST_TEST_EQ(&e, &m.matched());
		return true;
	}
	else
		return false;
}

int main()
{
	{
		my_error e = my_error::e1;

		BOOST_TEST(( test<leaf::match_if<my_error, cmp_my_error<my_error::e1>>>(e) ));
		BOOST_TEST(( !test<leaf::match_if<my_error, cmp_my_error<my_error::e2>>>(e) ));
	}

	{
		int r = leaf::try_handle_all(
			[]() -> leaf::result<int>
			{
				return leaf::new_error(my_error::e1);
			},

			[]( leaf::match_if<my_error, cmp_my_error<my_error::e1>> )
			{
				return 1;
			},

			[]
			{
				return 2;
			} );
		BOOST_TEST_EQ(r, 1);
	}

	{
		int r = leaf::try_handle_all(
			[]() -> leaf::result<int>
			{
				return leaf::new_error(my_error::e1);
			},

			[]( leaf::match_if<my_error, cmp_my_error<my_error::e2>> )
			{
				return 1;
			},

			[]
			{
				return 2;
			} );
		BOOST_TEST_EQ(r, 2);
	}

	return boost::report_errors();
}
