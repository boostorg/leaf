// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/try.hpp>
#include <boost/leaf/capture_exception.hpp>
#include <boost/leaf/common.hpp>
#include "boost/core/lightweight_test.hpp"
#include <sstream>

namespace leaf = boost::leaf;

template <int A>
struct unexpected_test
{
	int value;
};

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
	leaf::try_(
		[ ]
		{
			LEAF_THROW( my_error(),
				printable_info_printable_payload(),
				printable_info_non_printable_payload(),
				non_printable_info_printable_payload(),
				non_printable_info_non_printable_payload(),
				unexpected_test<1>{1},
				unexpected_test<2>{2},
				leaf::e_errno{ENOENT} );
		},
		[ ](
			leaf::e_source_location,
			printable_info_printable_payload,
			printable_info_non_printable_payload,
			non_printable_info_printable_payload,
			non_printable_info_non_printable_payload,
			leaf::e_errno,
			leaf::unexpected_error_info const &,
			leaf::error_info const & unmatched )
		{
			std::ostringstream st;
			st << unmatched;
			std::string s = st.str();
			BOOST_TEST(s.find("std::exception::what(): my_error")!=s.npos);
			BOOST_TEST(s.find(": N/A")!=s.npos);
			BOOST_TEST(s.find(": printed printable_payload")!=s.npos);
			BOOST_TEST(s.find("*** printable_info_non_printable_payload ***")!=s.npos);
			BOOST_TEST(s.find("*** printable_info_printable_payload printed printable_payload ***")!=s.npos);
			BOOST_TEST(s.find(") in function")!=s.npos);
			BOOST_TEST(s.find("Detected 2 attempts to communicate unexpected error objects, the first one of type ")!=s.npos);
			BOOST_TEST(s.find("unexpected_test<2>")==s.npos);
			std::cout << s;
		} );

	std::cout << std::endl;

	leaf::try_(
		[ ]
		{
			LEAF_THROW( my_error(),
				printable_info_printable_payload(),
				printable_info_non_printable_payload(),
				non_printable_info_printable_payload(),
				non_printable_info_non_printable_payload(),
				unexpected_test<1>{1},
				unexpected_test<2>{2},
				leaf::e_errno{ENOENT} );
		},
		[ ](
			leaf::e_source_location,
			printable_info_printable_payload,
			printable_info_non_printable_payload,
			non_printable_info_printable_payload,
			non_printable_info_non_printable_payload,
			leaf::e_errno,
			leaf::unexpected_error_info const &,
			leaf::verbose_diagnostic_info const & di )
		{
			std::ostringstream st;
			st << di;
			std::string s = st.str();
			BOOST_TEST(s.find("std::exception::what(): my_error")!=s.npos);
			BOOST_TEST(s.find(": N/A")!=s.npos);
			BOOST_TEST(s.find(": printed printable_payload")!=s.npos);
			BOOST_TEST(s.find("*** printable_info_non_printable_payload ***")!=s.npos);
			BOOST_TEST(s.find("*** printable_info_printable_payload printed printable_payload ***")!=s.npos);
			BOOST_TEST(s.find(") in function")!=s.npos);
			BOOST_TEST(s.find("Detected 2 attempts to communicate unexpected error objects, the first one of type ")!=s.npos);
			BOOST_TEST(s.find("unexpected_test<2>")!=s.npos);
			std::cout << s;
		} );

	std::cout << std::endl;

	leaf::try_(
		leaf::capture_exception<
			printable_info_printable_payload,
			printable_info_non_printable_payload,
			non_printable_info_printable_payload,
			non_printable_info_non_printable_payload,
			unexpected_test<1>,
			unexpected_test<2>,
			leaf::e_errno>(
				[ ]
				{
					LEAF_THROW( my_error(),
					printable_info_printable_payload(),
					printable_info_non_printable_payload(),
					non_printable_info_printable_payload(),
					non_printable_info_non_printable_payload(),
					unexpected_test<1>{1},
					unexpected_test<2>{2},
					leaf::e_errno{ENOENT} );
				} ),
		[ ](
			leaf::e_source_location,
			printable_info_printable_payload,
			printable_info_non_printable_payload,
			non_printable_info_printable_payload,
			non_printable_info_non_printable_payload,
			leaf::e_errno,
			leaf::unexpected_error_info const &,
			leaf::verbose_diagnostic_info const & di )
		{
			std::ostringstream st;
			st << di;
			std::string s = st.str();
			BOOST_TEST(s.find("Detected exception_capture")!=s.npos);
			BOOST_TEST(s.find("std::exception::what(): my_error")!=s.npos);
			BOOST_TEST(s.find(": N/A")!=s.npos);
			BOOST_TEST(s.find(": printed printable_payload")!=s.npos);
			BOOST_TEST(s.find("*** printable_info_non_printable_payload ***")!=s.npos);
			BOOST_TEST(s.find("*** printable_info_printable_payload printed printable_payload ***")!=s.npos);
			BOOST_TEST(s.find(") in function")!=s.npos);
			BOOST_TEST(s.find("Detected 2 attempts to communicate unexpected error objects, the first one of type ")!=s.npos);
			BOOST_TEST(s.find("unexpected_test<2>")!=s.npos);
			std::cout << s;
		} );

	return boost::report_errors();
}
