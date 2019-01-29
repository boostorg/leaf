#ifndef BOOST_LEAF_18ADC47C206611E983ECE95B87D1CC0E
#define BOOST_LEAF_18ADC47C206611E983ECE95B87D1CC0E

// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "_test_ec.hpp"

template <class T, class E>
class res
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
	res( T const & value ) noexcept:
		value_(value),
		which_(variant::value)
	{
	}
	res( E const & error ) noexcept:
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
		assert(which_==variant::value);
		return value_;
	}
	E const & error() const
	{
		assert(which_==variant::error);
		return error_;
	}
};

namespace boost { namespace leaf {

	template <class T>
	struct is_result_type;

	template <class T, class E>
	struct is_result_type<res<T, E>>: std::true_type
	{
	};

} }

#endif
