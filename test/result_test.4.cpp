//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/result.hpp>
#include <boost/leaf/expect.hpp>
#include <boost/leaf/exception.hpp>
#include <boost/detail/lightweight_test.hpp>

namespace leaf = boost::leaf;

template <class Expect>
void check( Expect & exp, leaf::bad_result const & e )
{
		handle_exception( exp, e,
			[ ]( leaf::meta::e_source_location const & x )
			{
				BOOST_TEST(strstr(x.file,"result.hpp")!=0);
				BOOST_TEST(x.line>0);
				BOOST_TEST(strstr(x.function,"value")!=0);
			} );
}

int main()
{
	{
		leaf::expect<leaf::meta::e_source_location> exp;
		try
		{
			leaf::result<int> r((leaf::error()));
			(void) r.value();
			BOOST_TEST(false);
		}
		catch( leaf::bad_result const & e )
		{
			check(exp,e);
		}
	}
	{
		leaf::expect<leaf::meta::e_source_location> exp;
		try
		{
			leaf::result<int> const r((leaf::error()));
			(void) r.value();
			BOOST_TEST(false);
		}
		catch( leaf::bad_result const & e )
		{
			check(exp,e);
		}
	}
	{
		leaf::expect<leaf::meta::e_source_location> exp;
		try
		{
			leaf::result<int> r((leaf::error()));
			(void) *r;
			BOOST_TEST(false);
		}
		catch( leaf::bad_result const & e )
		{
			check(exp,e);
		}
	}
	{
		leaf::expect<leaf::meta::e_source_location> exp;
		try
		{
			leaf::result<int> const r((leaf::error()));
			(void) *r;
			BOOST_TEST(false);
		}
		catch( leaf::bad_result const & e )
		{
			check(exp,e);
		}
	}
	return boost::report_errors();
}
