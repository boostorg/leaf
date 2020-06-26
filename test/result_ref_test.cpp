// Copyright (c) 2018-2020 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/result.hpp>
#include <boost/leaf/capture.hpp>
#include <boost/leaf/handle_error.hpp>
#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

struct val
{
	int id;

	friend bool operator==( val const & a, val const & b )
	{
		return a.id==b.id;
	}

	friend std::ostream & operator<<( std::ostream & os, val const & v )
	{
		return os << v.id;
	}
};

struct base
{
};

struct derived: base
{
};

int main()
{
	// Mutable:

	static_assert(std::is_same<decltype(std::declval<leaf::result<val const>>().value()), val const &>::value, "result type deduction bug");
	static_assert(std::is_same<decltype(std::declval<leaf::result<val>>().value()), val &>::value, "result type deduction bug");
	static_assert(std::is_same<decltype(std::declval<leaf::result<val const &>>().value()), val const &>::value, "result type deduction bug");
	static_assert(std::is_same<decltype(std::declval<leaf::result<val &>>().value()), val &>::value, "result type deduction bug");

	static_assert(std::is_same<decltype(*std::declval<leaf::result<val const>>()), val const &>::value, "result type deduction bug");
	static_assert(std::is_same<decltype(*std::declval<leaf::result<val>>()), val &>::value, "result type deduction bug");
	static_assert(std::is_same<decltype(*std::declval<leaf::result<val const &>>()), val const &>::value, "result type deduction bug");
	static_assert(std::is_same<decltype(*std::declval<leaf::result<val &>>()), val &>::value, "result type deduction bug");

	static_assert(std::is_same<decltype(std::declval<leaf::result<val const>>().operator->()), val const *>::value, "result type deduction bug");
	static_assert(std::is_same<decltype(std::declval<leaf::result<val>>().operator->()), val *>::value, "result type deduction bug");
	static_assert(std::is_same<decltype(std::declval<leaf::result<val const &>>().operator->()), val const *>::value, "result type deduction bug");
	static_assert(std::is_same<decltype(std::declval<leaf::result<val &>>().operator->()), val *>::value, "result type deduction bug");

	{
		val x = { 42 };
		leaf::result<val const &> r(x);
		BOOST_TEST(r);
		val a = r.value();
		BOOST_TEST_EQ(a, x);
		val b = *r;
		BOOST_TEST_EQ(b, x);
	}
	{
		val x = { 42 };
		leaf::result<val const &> r(x);
		BOOST_TEST(r);
		val const & a = r.value();
		BOOST_TEST_EQ(&a, &x);
		val const & b = *r;
		BOOST_TEST_EQ(&b, &x);
	}
	{
		val x = { 42 };
		leaf::result<val const &> r(x);
		BOOST_TEST(r);
		auto & a = r.value();
		BOOST_TEST_EQ(&a, &x);
		auto & b = *r;
		BOOST_TEST_EQ(&b, &x);
	}

	{
		val x = { 42 };
		leaf::result<val &> r(x);
		BOOST_TEST(r);
		val a = r.value();
		BOOST_TEST_EQ(a, x);
		val b = *r;
		BOOST_TEST_EQ(b, x);
		int id = x.id;
		BOOST_TEST_EQ(id+1, ++r->id);
	}
	{
		val x = { 42 };
		leaf::result<val &> r(x);
		BOOST_TEST(r);
		val & a = r.value();
		BOOST_TEST_EQ(&a, &x);
		val & b = *r;
		BOOST_TEST_EQ(&b, &x);
		int id = x.id;
		BOOST_TEST_EQ(id+1, ++r->id);
	}
	{
		val x = { 42 };
		leaf::result<val &> r(x);
		BOOST_TEST(r);
		auto & a = r.value();
		BOOST_TEST_EQ(&a, &x);
		auto & b = *r;
		BOOST_TEST_EQ(&b, &x);
		int id = x.id;
		BOOST_TEST_EQ(id+1, ++r->id);
	}

	// Const:

	static_assert(std::is_same<decltype(std::declval<leaf::result<val const> const>().value()), val const &>::value, "result type deduction bug");
	static_assert(std::is_same<decltype(std::declval<leaf::result<val> const>().value()), val const &>::value, "result type deduction bug");
	static_assert(std::is_same<decltype(std::declval<leaf::result<val const &> const>().value()), val const &>::value, "result type deduction bug");
	static_assert(std::is_same<decltype(std::declval<leaf::result<val &> const>().value()), val &>::value, "result type deduction bug");

	static_assert(std::is_same<decltype(*std::declval<leaf::result<val const> const>()), val const &>::value, "result type deduction bug");
	static_assert(std::is_same<decltype(*std::declval<leaf::result<val> const>()), val const &>::value, "result type deduction bug");
	static_assert(std::is_same<decltype(*std::declval<leaf::result<val const &> const>()), val const &>::value, "result type deduction bug");
	static_assert(std::is_same<decltype(*std::declval<leaf::result<val &> const>()), val &>::value, "result type deduction bug");

	static_assert(std::is_same<decltype(std::declval<leaf::result<val const> const>().operator->()), val const *>::value, "result type deduction bug");
	static_assert(std::is_same<decltype(std::declval<leaf::result<val> const>().operator->()), val const *>::value, "result type deduction bug");
	static_assert(std::is_same<decltype(std::declval<leaf::result<val const &> const>().operator->()), val const *>::value, "result type deduction bug");
	static_assert(std::is_same<decltype(std::declval<leaf::result<val &> const>().operator->()), val *>::value, "result type deduction bug");

	{
		val x = { 42 };
		leaf::result<val const &> const r(x);
		BOOST_TEST(r);
		val a = r.value();
		BOOST_TEST_EQ(a, x);
		val b = *r;
		BOOST_TEST_EQ(b, x);
	}
	{
		val x = { 42 };
		leaf::result<val const &> const r(x);
		BOOST_TEST(r);
		val const & a = r.value();
		BOOST_TEST_EQ(&a, &x);
		val const & b = *r;
		BOOST_TEST_EQ(&b, &x);
	}
	{
		val x = { 42 };
		leaf::result<val const &> const r(x);
		BOOST_TEST(r);
		auto & a = r.value();
		BOOST_TEST_EQ(&a, &x);
		auto & b = *r;
		BOOST_TEST_EQ(&b, &x);
	}

	{
		val x = { 42 };
		leaf::result<val &> const r(x);
		BOOST_TEST(r);
		val a = r.value();
		BOOST_TEST_EQ(a, x);
		val b = *r;
		BOOST_TEST_EQ(b, x);
		int id = x.id;
		BOOST_TEST_EQ(id+1, ++r->id);
	}
	{
		val x = { 42 };
		leaf::result<val &> const r(x);
		BOOST_TEST(r);
		val & a = r.value();
		BOOST_TEST_EQ(&a, &x);
		val & b = *r;
		BOOST_TEST_EQ(&b, &x);
	}
	{
		val x = { 42 };
		leaf::result<val &> const r(x);
		BOOST_TEST(r);
		auto & a = r.value();
		BOOST_TEST_EQ(&a, &x);
		auto & b = *r;
		BOOST_TEST_EQ(&b, &x);
		int id = x.id;
		BOOST_TEST_EQ(id+1, ++r->id);
	}

	// Hierarchy

	{
		derived d;
		leaf::result<base &> r = d;
		BOOST_TEST_EQ(&r.value(), &d);
		BOOST_TEST_EQ(&*r, &d);
		BOOST_TEST_EQ(r.operator->(), &d);
	}

	{
		derived d;
		leaf::result<base *> r = &d;
		BOOST_TEST_EQ(r.value(), &d);
		BOOST_TEST_EQ(*r, &d);
		BOOST_TEST_EQ(*r.operator->(), &d);
	}

	return boost::report_errors();
}
