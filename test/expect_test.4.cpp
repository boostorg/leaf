//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/expect.hpp>
#include <boost/detail/lightweight_test.hpp>

namespace leaf = boost::leaf;

class MStatus { };

namespace boost { namespace leaf {
	template<> struct is_error_type<MStatus>: std::true_type { };
} }

int main()
{
	leaf::expect<MStatus> exp;
	MStatus ms;
	{
		MStatus copy = ms;
		{
			int c=0;
			bool handled = leaf::handle_error( exp, leaf::error(copy),
				[&c]( MStatus ms )
				{
					++c;
				} );
			BOOST_TEST(handled);
			BOOST_TEST(c==1);
		}
		{
			int c=0;
			bool handled = leaf::handle_error( exp, leaf::error(copy),
				[&c]( MStatus const ms )
				{
					++c;
				} );
			BOOST_TEST(handled);
			BOOST_TEST(c==1);
		}
		{
			int c=0;
			bool handled = leaf::handle_error( exp, leaf::error(copy),
				[&c]( MStatus const & ms )
				{
					++c;
				} );
			BOOST_TEST(handled);
			BOOST_TEST(c==1);
		}
	}
	{
		MStatus & ref = ms;
		{
			int c=0;
			bool handled = leaf::handle_error( exp, leaf::error(ref),
				[&c]( MStatus ms )
				{
					++c;
				} );
			BOOST_TEST(handled);
			BOOST_TEST(c==1);
		}
		{
			int c=0;
			bool handled = leaf::handle_error( exp, leaf::error(ref),
				[&c]( MStatus const ms )
				{
					++c;
				} );
			BOOST_TEST(handled);
			BOOST_TEST(c==1);
		}
		{
			int c=0;
			bool handled = leaf::handle_error( exp, leaf::error(ref),
				[&c]( MStatus const & ms )
				{
					++c;
				} );
			BOOST_TEST(handled);
			BOOST_TEST(c==1);
		}
	}
	{
		{
			MStatus const & cref = ms;
			int c=0;
			bool handled = leaf::handle_error( exp, leaf::error(cref),
				[&c]( MStatus ms )
				{
					++c;
				} );
			BOOST_TEST(handled);
			BOOST_TEST(c==1);
		}
		{
			MStatus const & cref = ms;
			int c=0;
			bool handled = leaf::handle_error( exp, leaf::error(cref),
				[&c]( MStatus const ms )
				{
					++c;
				} );
			BOOST_TEST(handled);
			BOOST_TEST(c==1);
		}
		{
			MStatus const & cref = ms;
			int c=0;
			bool handled = leaf::handle_error( exp, leaf::error(cref),
				[&c]( MStatus const & ms )
				{
					++c;
				} );
			BOOST_TEST(handled);
			BOOST_TEST(c==1);
		}
	}
	return boost::report_errors();
}
