//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/exception_capture.hpp>
#include <boost/leaf/exception.hpp>
#include <boost/leaf/common.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <sstream>

namespace leaf = boost::leaf;

struct my_error:
	virtual std::exception
{
	char const * what() const noexcept
	{
		return "my_error";
	}
};

struct printable_payload
{
	friend std::ostream & operator<<( std::ostream & os, printable_payload const & x )
	{
		return os << "printed printable_payload";
	}
};

struct non_printable_payload
{
};

struct printable_info_printable_payload
{
	printable_payload value;

	friend std::ostream & operator<<( std::ostream & os, printable_info_printable_payload const & x )
	{
		return os << "*** printable_info_printable_payload " << x.value << " ***";
	}
};

struct printable_info_non_printable_payload
{
	non_printable_payload value;

	friend std::ostream & operator<<( std::ostream & os, printable_info_non_printable_payload const & x )
	{
		return os << "*** printable_info_non_printable_payload ***";
	}
};

struct non_printable_info_printable_payload
{
	printable_payload value;
};

struct non_printable_info_non_printable_payload
{
	non_printable_payload value;
};

int main()
{
	{
		leaf::expect
			<
			printable_info_printable_payload,
			printable_info_non_printable_payload,
			non_printable_info_printable_payload,
			non_printable_info_non_printable_payload,
			leaf::e_errno,
			leaf::meta::e_source_location
			> exp;
		try
		{
			LEAF_THROW<my_error>(
				printable_info_printable_payload(),
				printable_info_non_printable_payload(),
				non_printable_info_printable_payload(),
				non_printable_info_non_printable_payload(),
				leaf::e_errno{ENOENT} );
		}
		catch( my_error & e )
		{
			std::ostringstream st;
			current_exception_diagnostic_output(st,exp);
			std::string s = st.str();
			BOOST_TEST(s.find("std::exception::what(): my_error")!=s.npos);
			BOOST_TEST(s.find(" = N/A")!=s.npos);
			BOOST_TEST(s.find(" = printed printable_payload")!=s.npos);
			BOOST_TEST(s.find("*** printable_info_non_printable_payload ***")!=s.npos);
			BOOST_TEST(s.find("*** printable_info_printable_payload printed printable_payload ***")!=s.npos);
			BOOST_TEST(s.find(") in function")!=s.npos);
			std::cout << s;
			handle_exception( exp, e, [ ]{ } );
		}
	}
	return boost::report_errors();
}
