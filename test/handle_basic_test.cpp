// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>
#ifndef LEAF_NO_EXCEPTIONS
#	include <boost/leaf/handle_exception.hpp>
#endif
#include <boost/leaf/handle_error.hpp>
#include <boost/leaf/result.hpp>
#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

struct test_info { int value;};

enum class error_code
{
	error1=1,
	error2,
	error3
};

namespace boost { namespace leaf {
	template <> struct is_e_type<error_code>: public std::true_type { };
} }

leaf::result<int> compute_answer( int what_to_do ) noexcept
{
	switch( what_to_do )
	{
	case 0:
		return 42;
	case 1:
		return leaf::new_error(error_code::error1);
	case 2:
		return leaf::new_error(error_code::error2);
	default:
		assert(what_to_do==3);
		return leaf::new_error(error_code::error3);
	}
}

leaf::result<int> handle_some_errors( int what_to_do )
{
	return leaf::try_handle_some(
		[=]
		{
			return compute_answer(what_to_do);
		},
		[]( leaf::match<error_code,error_code::error1> )
		{
			return -42;
		} );
}

leaf::result<float> handle_some_errors_float( int what_to_do )
{
	return leaf::try_handle_some(
		[=]() -> leaf::result<float>
		{
			return compute_answer(what_to_do);
		},
		[]( leaf::match<error_code,error_code::error2>  )
		{
			return -42.0f;
		} );
}

leaf::result<void> handle_some_errors_void( int what_to_do )
{
	return leaf::try_handle_some(
		[=]() -> leaf::result<void>
		{
			LEAF_AUTO(answer, compute_answer(what_to_do));
			(void) answer;
			return { };
		},
		[]( leaf::match<error_code,error_code::error3>  )
		{
		} );
}

int main()
{
	BOOST_TEST_EQ(handle_some_errors(0).value(), 42);
	BOOST_TEST_EQ(handle_some_errors(1).value(), -42);
	{
		int r = leaf::try_handle_all(
			[]() -> leaf::result<int>
			{
				LEAF_AUTO(answer,handle_some_errors(3));
				(void) answer;
				return 0;
			},
			[]( leaf::match<error_code,error_code::error3> )
			{
				return 1;
			},
			[]
			{
				return 2;
			} );
		BOOST_TEST_EQ(r, 1);
	}

	///////////////////////////

	BOOST_TEST_EQ(handle_some_errors_float(0).value(), 42.0f);
	BOOST_TEST_EQ(handle_some_errors_float(2).value(), -42.0f);
	{
		int r = leaf::try_handle_all(
			[]() -> leaf::result<int>
			{
				LEAF_AUTO(answer,handle_some_errors_float(1));
				(void) answer;
				return 0;
			},
			[]( leaf::match<error_code,error_code::error1> )
			{
				return 1;
			},
			[]
			{
				return 2;
			} );
		BOOST_TEST_EQ(r, 1);
	}

	///////////////////////////

	BOOST_TEST(handle_some_errors_void(0));
	BOOST_TEST(handle_some_errors_void(3));
	{
		int r = leaf::try_handle_all(
			[]() -> leaf::result<int>
			{
				LEAF_CHECK(handle_some_errors_void(2));
				return 0;
			},
			[]( leaf::match<error_code,error_code::error2> )
			{
				return 1;
			},
			[]
			{
				return 2;
			} );
		BOOST_TEST_EQ(r, 1);
	}

	///////////////////////////

#ifndef LEAF_NO_EXCEPTIONS
	{
		int r = leaf::try_handle_all(
			[]() -> leaf::result<int>
			{
				LEAF_CHECK(handle_some_errors_void(2));
				return 0;
			},
			[]( leaf::catch_<std::exception> )
			{
				return 1;
			},
			[]
			{
				return 2;
			} );
		BOOST_TEST_EQ(r, 2);
	}
#endif

	///////////////////////////

	{
		int r = leaf::try_handle_all(
			[]() -> leaf::result<int>
			{
				return leaf::new_error( test_info{42} );
			},
			[]( test_info const & x )
			{
				BOOST_TEST_EQ(x.value, 42);
				int r = leaf::try_handle_all(
					[]() -> leaf::result<int>
					{
						return leaf::new_error( test_info{43} );
					},
					[]()
					{
						return -1;
					} );
				BOOST_TEST_EQ(r, -1);
				BOOST_TEST_EQ(x.value, 42);
				return 0;
			},
			[]()
			{
				return -1;
			} );
		BOOST_TEST_EQ(r, 0);
	}

	///////////////////////////

	return boost::report_errors();
}
