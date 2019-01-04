//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/error_capture.hpp>
#include <boost/leaf/expect.hpp>
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

int count = 0;

template <int>
struct info
{
	info() noexcept
	{
		++count;
	}

	info( info const & ) noexcept
	{
		++count;
	}

	~info() noexcept
	{
		--count;
	}
};

namespace boost { namespace leaf {
	template <int I> struct is_error_type<info<I>>: public std::true_type { };
} }

leaf::error f()
{
	return leaf::error(info<1>{},info<3>{});
}

leaf::error_capture make_capture()
{
	leaf::expect<info<1>,info<2>,info<3>> exp;
	return capture(exp,f());
}

int main()
{
	{
		leaf::error_capture ec1 = make_capture();
		BOOST_TEST( ec1.handle_error( [ ]( info<1>, info<3> ) { } ) );
		BOOST_TEST(count==2);
		leaf::error_capture ec2(ec1);
		BOOST_TEST(count==2);
		BOOST_TEST( ec1.handle_error( [ ]( info<1>, info<3> ) { } ) );
		BOOST_TEST( ec2.handle_error( [ ]( info<1>, info<3> ) { } ) );
		leaf::error_capture ec3(std::move(ec2));
		BOOST_TEST(count==2);
		BOOST_TEST( ec1.handle_error( [ ]( info<1>, info<3> ) { } ) );
		BOOST_TEST( ec3.handle_error( [ ]( info<1>, info<3> ) { } ) );
		leaf::error_capture ec4; ec4 = ec3;
		BOOST_TEST(count==2);
		BOOST_TEST( ec1.handle_error( [ ]( info<1>, info<3> ) { } ) );
		BOOST_TEST( ec3.handle_error( [ ]( info<1>, info<3> ) { } ) );
		BOOST_TEST( ec4.handle_error( [ ]( info<1>, info<3> ) { } ) );
		leaf::error_capture ec5; ec5 = std::move(ec4);
		BOOST_TEST(count==2);
		BOOST_TEST( ec1.handle_error( [ ]( info<1>, info<3> ) { } ) );
		BOOST_TEST( ec3.handle_error( [ ]( info<1>, info<3> ) { } ) );
		BOOST_TEST( ec5.handle_error( [ ]( info<1>, info<3> ) { } ) );
	}
	BOOST_TEST(count==0);
	return boost::report_errors();
}
