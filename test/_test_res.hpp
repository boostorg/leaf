#ifndef BOOST_LEAF_TEST_RES_HPP_INCLUDED
#define BOOST_LEAF_TEST_RES_HPP_INCLUDED

// Copyright (c) 2018-2020 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "_test_ec.hpp"

template <class T, class E>
class test_res
{
	enum class variant
	{
		value,
		error
	};
	T value_;
	E error_;
	variant which_;
public:
	test_res( T const & value ) noexcept:
		value_(value),
		error_(),
		which_(variant::value)
	{
	}
	test_res( E const & error ) noexcept:
		value_(),
		error_(error),
		which_(variant::error)
	{
	}
	explicit operator bool() const noexcept
	{
		return which_==variant::value;
	}
	T const & value() const
	{
		BOOST_LEAF_ASSERT(which_==variant::value);
		return value_;
	}
	E const & error() const
	{
		BOOST_LEAF_ASSERT(which_==variant::error);
		return error_;
	}
};

namespace boost { namespace leaf {

	template <class T>
	struct is_result_type;

	template <class T, class E>
	struct is_result_type<test_res<T, E>>: std::true_type
	{
	};

} }

#endif
