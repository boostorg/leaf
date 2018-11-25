//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/result.hpp>
#include <boost/leaf/expect.hpp>
#include <boost/leaf/error_capture.hpp>
#include <boost/detail/lightweight_test.hpp>

namespace leaf = boost::leaf;

struct err
{
	static int count;

	err()
	{
		++count;
	}

	err( err const & )
	{
		++count;
	}

	err( err && )
	{
		++count;
	}

	~err()
	{
		--count;
	}
};
int err::count = 0;
struct e_err1 { err value; };
struct e_err2 { err value; };

int main()
{
	leaf::result<int> r0;
	{
		leaf::expect<e_err1> exp;
		leaf::result<int> r1 = leaf::error( e_err1{ } );
		BOOST_TEST(!r1);
		r0 = capture(exp,r1);
		BOOST_TEST(err::count==1);
	}
	BOOST_TEST(err::count==1);
	BOOST_TEST(!r0);
	leaf::expect<e_err1,e_err2> exp;
	leaf::result<int> r2 = r0.error( e_err2{ } );
	BOOST_TEST(err::count==2);
	BOOST_TEST(handle_error(exp,r2,[ ](e_err1,e_err2){ }));
	return boost::report_errors();
}
