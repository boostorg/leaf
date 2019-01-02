//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/result.hpp>
#include <boost/leaf/expect.hpp>
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

enum class error_code
{
	error1=1,
	error2,
	error3
};

namespace boost { namespace leaf {
	template <> struct is_error_type<error_code>: public std::true_type { };
} }

leaf::result<int> compute_answer( int what_to_do ) noexcept
{
	switch( what_to_do )
	{
	case 0:
		return 42;
	case 1:
		return leaf::error(error_code::error1);
	case 2:
		return leaf::error(error_code::error2);
	default:
		assert(what_to_do==3);
		return leaf::error(error_code::error3);
	}
}

leaf::result<int> handle_some_errors( int what_to_do )
{
	leaf::expect<error_code> exp;
	if( leaf::result<int> r = compute_answer(what_to_do) )
		return r;
	else
		return handle_error( exp, r,
			[&r]( error_code ec ) -> leaf::result<int>
			{
				if( ec==error_code::error1 )
					return 42;
				else
					return r;
			} );
}

leaf::result<float> handle_some_errors_float( int what_to_do )
{
	leaf::expect<error_code> exp;
	if( leaf::result<int> r = compute_answer(what_to_do) )
		return r;
	else
		return handle_error( exp, r,
			[&r]( error_code ec ) -> leaf::result<float>
			{
				if( ec==error_code::error2 )
					return 42.0f;
				else
					return r;
			} );
}

leaf::result<void> handle_some_errors_void( int what_to_do )
{
	leaf::expect<error_code> exp;
	if( leaf::result<int> r = compute_answer(what_to_do) )
		return { };
	else
		return handle_error( exp, r,
			[&r]( error_code ec ) -> leaf::result<void>
			{
				if( ec==error_code::error3 )
					return { };
				else
					return r.error();
			} );
}

int main()
{
	leaf::expect<error_code> exp;

	///////////////////////////

	BOOST_TEST(handle_some_errors(0).value()==42);
	BOOST_TEST(handle_some_errors(1).value()==42);
	{
		leaf::result<int> r = handle_some_errors(2);
		BOOST_TEST(!r);
		int c=0;
		BOOST_TEST( handle_error( exp, r,
			[&c]( error_code ec )
			{
				BOOST_TEST(ec==error_code::error2);
				++c;
			} ) );
		BOOST_TEST(c==1);
	}
	{
		leaf::result<float> r = handle_some_errors(3);
		BOOST_TEST(!r);
		int c=0;
		BOOST_TEST( handle_error( exp, r,
			[&c]( error_code ec )
			{
				BOOST_TEST(ec==error_code::error3);
				++c;
			} ) );
		BOOST_TEST(c==1);
	}

	///////////////////////////

	BOOST_TEST(handle_some_errors_float(0).value()==42);
	{
		leaf::result<float> r = handle_some_errors_float(1);
		BOOST_TEST(!r);
		int c=0;
		BOOST_TEST( handle_error( exp, r,
			[&c]( error_code ec )
			{
				BOOST_TEST(ec==error_code::error1);
				++c;
			} ) );
		BOOST_TEST(c==1);
	}
	BOOST_TEST(handle_some_errors_float(2).value()==42.0f);
	{
		leaf::result<int> r = handle_some_errors_float(3);
		BOOST_TEST(!r);
		int c=0;
		BOOST_TEST( handle_error( exp, r,
			[&c]( error_code ec )
			{
				BOOST_TEST(ec==error_code::error3);
				++c;
			} ) );
		BOOST_TEST(c==1);
	}

	///////////////////////////

	BOOST_TEST(handle_some_errors_void(0));
	{
		leaf::result<void> r = handle_some_errors_void(1);
		BOOST_TEST(!r);
		int c=0;
		BOOST_TEST( handle_error( exp, r,
			[&c]( error_code ec )
			{
				BOOST_TEST(ec==error_code::error1);
				++c;
			} ) );
		BOOST_TEST(c==1);
	}
	{
		leaf::result<void> r = handle_some_errors_void(2);
		BOOST_TEST(!r);
		int c=0;
		BOOST_TEST( handle_error( exp, r,
			[&c]( error_code ec )
			{
				BOOST_TEST(ec==error_code::error2);
				++c;
			} ) );
		BOOST_TEST(c==1);
	}
	BOOST_TEST(handle_some_errors_void(3));

	///////////////////////////

	return boost::report_errors();
}
