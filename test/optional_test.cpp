// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/optional.hpp>
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

int object_count=0;
int value_count=0;

class my_info
{
	my_info & operator=( my_info const & ) = delete;

public:

	int value;

	explicit my_info( int value ):
		value(value)
	{
		BOOST_TEST(++object_count>0);
		BOOST_TEST(++value_count>0);
	}

	my_info( my_info const & x ):
		value(x.value)
	{
		BOOST_TEST(++object_count>0);
		BOOST_TEST(++value_count>0);
	}

	my_info( my_info && x ):
		value(x.value)
	{
		x.value=-1;
		BOOST_TEST(++object_count>0);
	}
	~my_info()
	{
		BOOST_TEST(--object_count>=0);
		if( value!=-1 )
			BOOST_TEST(--value_count>=0);
	}
};

class throws_on_copy
{
	throws_on_copy & operator=( throws_on_copy const & )=delete;

public:

	int value;

	throws_on_copy()
	{
		BOOST_TEST(++object_count>0);
	}

	throws_on_copy( throws_on_copy const & )
	{
		throw std::exception();
	}

	throws_on_copy( throws_on_copy && )
	{
		BOOST_TEST(++object_count>0);
	}

	~throws_on_copy()
	{
		BOOST_TEST(--object_count>=0);
	}
};

void run_tests()
{
	using leaf::leaf_detail::optional;
	{
		optional<my_info> x;
		BOOST_TEST(!x.has_value());
	}
	BOOST_TEST(!object_count);
	BOOST_TEST(!value_count);
	{
		my_info a(42);
		BOOST_TEST_EQ(object_count, 1);
		BOOST_TEST_EQ(value_count, 1);
		optional<my_info> x(a);
		BOOST_TEST_EQ(object_count, 2);
		BOOST_TEST_EQ(value_count, 2);
		BOOST_TEST(x.has_value());
		BOOST_TEST_EQ(x.value().value, 42);
	}
	BOOST_TEST(!object_count);
	BOOST_TEST(!value_count);
	{
		throws_on_copy a;
		BOOST_TEST_EQ(object_count, 1);
		try
		{
			optional<throws_on_copy> x(a);
			BOOST_TEST(false);
		}
		catch( std::exception & )
		{
		}
	}
	BOOST_TEST(!object_count);
	BOOST_TEST(!value_count);
	{
		my_info a(42);
		BOOST_TEST_EQ(object_count, 1);
		BOOST_TEST_EQ(value_count, 1);
		optional<my_info> x(std::move(a));
		BOOST_TEST_EQ(object_count, 2);
		BOOST_TEST_EQ(value_count, 1);
		BOOST_TEST(x.has_value());
		BOOST_TEST_EQ(x.value().value, 42);
	}
	BOOST_TEST(!object_count);
	BOOST_TEST(!value_count);
	{
		optional<my_info> x(my_info(42));
		BOOST_TEST_EQ(object_count, 1);
		BOOST_TEST_EQ(value_count, 1);
		BOOST_TEST(x.has_value());
		BOOST_TEST_EQ(x.value().value, 42);
		optional<my_info> y(x);
		BOOST_TEST_EQ(object_count, 2);
		BOOST_TEST_EQ(value_count, 2);
		BOOST_TEST(x.has_value());
		BOOST_TEST_EQ(x.value().value, 42);
		BOOST_TEST(y.has_value());
		BOOST_TEST_EQ(y.value().value, 42);
	}
	BOOST_TEST(!object_count);
	BOOST_TEST(!value_count);
	{
		optional<my_info> x(my_info(42));
		BOOST_TEST_EQ(object_count, 1);
		BOOST_TEST_EQ(value_count, 1);
		BOOST_TEST(x.has_value());
		BOOST_TEST_EQ(x.value().value, 42);
		optional<my_info> y(std::move(x));
		BOOST_TEST_EQ(object_count, 1);
		BOOST_TEST_EQ(value_count, 1);
		BOOST_TEST(!x.has_value());
		BOOST_TEST(y.has_value());
		BOOST_TEST_EQ(y.value().value, 42);
	}
	BOOST_TEST(!object_count);
	BOOST_TEST(!value_count);
	{
		optional<my_info> x(my_info(42));
		BOOST_TEST_EQ(object_count, 1);
		BOOST_TEST_EQ(value_count, 1);
		BOOST_TEST(x.has_value());
		BOOST_TEST_EQ(x.value().value, 42);
		optional<my_info> y;
		BOOST_TEST_EQ(&(y=x), &y);
		BOOST_TEST_EQ(object_count, 2);
		BOOST_TEST_EQ(value_count, 2);
		BOOST_TEST(x.has_value());
		BOOST_TEST_EQ(x.value().value, 42);
		BOOST_TEST(y.has_value());
		BOOST_TEST_EQ(y.value().value, 42);
	}
	BOOST_TEST(!object_count);
	BOOST_TEST(!value_count);
	{
		optional<my_info> x(my_info(42));
		BOOST_TEST_EQ(object_count, 1);
		BOOST_TEST_EQ(value_count, 1);
		BOOST_TEST(x.has_value());
		BOOST_TEST_EQ(x.value().value, 42);
		optional<my_info> y(my_info(43));
		BOOST_TEST_EQ(object_count, 2);
		BOOST_TEST_EQ(value_count, 2);
		BOOST_TEST_EQ(&(y=x), &y);
		BOOST_TEST_EQ(object_count, 2);
		BOOST_TEST_EQ(value_count, 2);
		BOOST_TEST(x.has_value());
		BOOST_TEST_EQ(x.value().value, 42);
		BOOST_TEST(y.has_value());
		BOOST_TEST_EQ(y.value().value, 42);
	}
	BOOST_TEST(!object_count);
	BOOST_TEST(!value_count);
	{
		optional<throws_on_copy> x((throws_on_copy()));
		BOOST_TEST_EQ(object_count, 1);
		BOOST_TEST(x.has_value());
		optional<throws_on_copy> y;
		try
		{
			(void) (y=x);
		}
		catch( std::exception & )
		{
		}
		BOOST_TEST_EQ(object_count, 1);
		BOOST_TEST(x.has_value());
		BOOST_TEST(!y.has_value());
	}
	BOOST_TEST(!object_count);
	BOOST_TEST(!value_count);
	{
		optional<my_info> x(my_info(42));
		BOOST_TEST_EQ(object_count, 1);
		BOOST_TEST_EQ(value_count, 1);
		BOOST_TEST(x.has_value());
		BOOST_TEST_EQ(x.value().value, 42);
		optional<my_info> y;
		BOOST_TEST_EQ(&(y=std::move(x)), &y);
		BOOST_TEST_EQ(object_count, 1);
		BOOST_TEST_EQ(value_count, 1);
		BOOST_TEST(!x.has_value());
		BOOST_TEST(y.has_value());
		BOOST_TEST_EQ(y.value().value, 42);
	}
	BOOST_TEST(!object_count);
	BOOST_TEST(!value_count);
	{
		optional<my_info> x(my_info(42));
		BOOST_TEST_EQ(object_count, 1);
		BOOST_TEST_EQ(value_count, 1);
		BOOST_TEST(x.has_value());
		BOOST_TEST_EQ(x.value().value, 42);
		optional<my_info> y(my_info(43));
		BOOST_TEST_EQ(object_count, 2);
		BOOST_TEST_EQ(value_count, 2);
		BOOST_TEST_EQ(&(y=std::move(x)), &y);
		BOOST_TEST_EQ(object_count, 1);
		BOOST_TEST_EQ(value_count, 1);
		BOOST_TEST(!x.has_value());
		BOOST_TEST(y.has_value());
		BOOST_TEST_EQ(y.value().value, 42);
	}
	BOOST_TEST(!object_count);
	BOOST_TEST(!value_count);
	{
		optional<my_info> x;
		my_info a(42);
		x.put(a);
		BOOST_TEST_EQ(object_count, 2);
		BOOST_TEST_EQ(value_count, 2);
		BOOST_TEST(x.has_value());
		BOOST_TEST_EQ(x.value().value, 42);
	}
	BOOST_TEST(!object_count);
	BOOST_TEST(!value_count);
	{
		optional<my_info> x(my_info(43));
		BOOST_TEST_EQ(object_count, 1);
		BOOST_TEST_EQ(value_count, 1);
		my_info a(42);
		x.put(a);
		BOOST_TEST_EQ(object_count, 2);
		BOOST_TEST_EQ(value_count, 2);
		BOOST_TEST(x.has_value());
		BOOST_TEST_EQ(x.value().value, 42);
	}
	BOOST_TEST(!object_count);
	BOOST_TEST(!value_count);
	{
		optional<my_info> x;
		x.put(my_info(42));
		BOOST_TEST_EQ(object_count, 1);
		BOOST_TEST_EQ(value_count, 1);
		BOOST_TEST(x.has_value());
		BOOST_TEST_EQ(x.value().value, 42);
	}
	BOOST_TEST(!object_count);
	BOOST_TEST(!value_count);
	{
		optional<my_info> x(my_info(43));
		BOOST_TEST_EQ(object_count, 1);
		BOOST_TEST_EQ(value_count, 1);
		x.put(my_info(42));
		BOOST_TEST_EQ(object_count, 1);
		BOOST_TEST_EQ(value_count, 1);
		BOOST_TEST(x.has_value());
		BOOST_TEST_EQ(x.value().value, 42);
	}
	BOOST_TEST(!object_count);
	BOOST_TEST(!value_count);
	{
		optional<my_info> x(my_info(42));
		BOOST_TEST_EQ(object_count, 1);
		BOOST_TEST_EQ(value_count, 1);
		my_info a = std::move(x).value();
		BOOST_TEST_EQ(object_count, 1);
		BOOST_TEST_EQ(value_count, 1);
		BOOST_TEST(!x.has_value());
		BOOST_TEST_EQ(a.value, 42);
	}
	BOOST_TEST(!object_count);
	BOOST_TEST(!value_count);
	{
		optional<my_info> x(my_info(42));
		BOOST_TEST_EQ(object_count, 1);
		BOOST_TEST_EQ(value_count, 1);
		BOOST_TEST(x.has_value());
		x.reset();
		BOOST_TEST(!x.has_value());
	}
	BOOST_TEST(!object_count);
	BOOST_TEST(!value_count);
}

int main()
{
	run_tests();
	return boost::report_errors();
}
