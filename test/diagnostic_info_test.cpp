// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/handle_exception.hpp>
#include <boost/leaf/exception.hpp>
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
	leaf::try_catch(
		[]
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
		[](
			leaf::e_source_location,
			printable_info_printable_payload,
			printable_info_non_printable_payload,
			non_printable_info_printable_payload,
			non_printable_info_non_printable_payload,
			leaf::e_errno,
			leaf::error_info const & unmatched )
		{
			{
				std::ostringstream st;
				st << unmatched;
				std::string s = st.str();
				BOOST_TEST(s.find("leaf::error_info:")!=s.npos);
				BOOST_TEST(s.find("std::exception::what(): my_error")!=s.npos);
				BOOST_TEST(s.find(": {Non-Printable}")!=s.npos);
				BOOST_TEST(s.find(": printed printable_payload")!=s.npos);
				BOOST_TEST(s.find("*** printable_info_non_printable_payload ***")!=s.npos);
				BOOST_TEST(s.find("*** printable_info_printable_payload printed printable_payload ***")!=s.npos);
				BOOST_TEST(s.find(") in function")!=s.npos);
				BOOST_TEST_EQ(s.find("unexpected"), s.npos);
				std::cout << s;
			}
			std::cout << "polymorphic_context::print():" << std::endl;
			{
				std::ostringstream st;
				unmatched.ctx_.print(st);
				std::string s = st.str();
				std::cout << s;
			}
		} );

	std::cout << std::endl;

	leaf::try_catch(
		[]
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
		[](
			leaf::e_source_location,
			printable_info_printable_payload,
			printable_info_non_printable_payload,
			non_printable_info_printable_payload,
			non_printable_info_non_printable_payload,
			leaf::e_errno,
			leaf::diagnostic_info const & unmatched )
		{
			{
				std::ostringstream st;
				st << unmatched;
				std::string s = st.str();
#ifdef BOOST_LEAF_DISCARD_UNEXPECTED
				BOOST_TEST(s.find("BOOST_LEAF_DISCARD_UNEXPECTED")!=s.npos);
#else
				BOOST_TEST(s.find("leaf::diagnostic_info:")!=s.npos);
				BOOST_TEST(s.find("std::exception::what(): my_error")!=s.npos);
				BOOST_TEST(s.find(": {Non-Printable}")!=s.npos);
				BOOST_TEST(s.find(": printed printable_payload")!=s.npos);
				BOOST_TEST(s.find("*** printable_info_non_printable_payload ***")!=s.npos);
				BOOST_TEST(s.find("*** printable_info_printable_payload printed printable_payload ***")!=s.npos);
				BOOST_TEST(s.find(") in function")!=s.npos);
				BOOST_TEST(s.find("Detected 2 attempts")!=s.npos);
				BOOST_TEST(s.find("unexpected_test<1>")!=s.npos);
#endif
				BOOST_TEST_EQ(s.find("unexpected_test<2>"), s.npos);
				std::cout << s;
			}
			std::cout << "polymorphic_context::print():" << std::endl;
			{
				std::ostringstream st;
				unmatched.ctx_.print(st);
				std::string s = st.str();
				std::cout << s;
			}
		} );

	std::cout << std::endl;

	leaf::try_catch(
		[]
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
		[](
			leaf::e_source_location,
			printable_info_printable_payload,
			printable_info_non_printable_payload,
			non_printable_info_printable_payload,
			non_printable_info_non_printable_payload,
			leaf::e_errno,
			leaf::verbose_diagnostic_info const & di )
		{
			{
				std::ostringstream st;
				st << di;
				std::string s = st.str();
#ifdef BOOST_LEAF_DISCARD_UNEXPECTED
				BOOST_TEST(s.find("BOOST_LEAF_DISCARD_UNEXPECTED")!=s.npos);
#else
				BOOST_TEST(s.find("leaf::verbose_diagnostic_info:")!=s.npos);
				BOOST_TEST(s.find("std::exception::what(): my_error")!=s.npos);
				BOOST_TEST(s.find(": {Non-Printable}")!=s.npos);
				BOOST_TEST(s.find(": printed printable_payload")!=s.npos);
				BOOST_TEST(s.find("*** printable_info_non_printable_payload ***")!=s.npos);
				BOOST_TEST(s.find("*** printable_info_printable_payload printed printable_payload ***")!=s.npos);
				BOOST_TEST(s.find(") in function")!=s.npos);
				BOOST_TEST(s.find("unexpected_test<1>")!=s.npos);
				BOOST_TEST(s.find("unexpected_test<2>")!=s.npos);
#endif
				std::cout << s;
			}
			std::cout << "polymorphic_context::print():" << std::endl;
			{
				std::ostringstream st;
				di.ctx_.print(st);
				std::string s = st.str();
				std::cout << s;
			}
		} );

	std::cout << std::endl;

	leaf::try_catch(
		[]
		{
			LEAF_THROW( my_error(), leaf::e_errno{ENOENT} );
		},
		[]( leaf::e_source_location, leaf::e_errno, leaf::diagnostic_info const & di )
		{
			{
				std::ostringstream st;
				st << di;
				std::string s = st.str();
				BOOST_TEST(s.find("leaf::diagnostic_info")!=s.npos);
				std::cout << s;
			}
			std::cout << "polymorphic_context::print():" << std::endl;
			{
				std::ostringstream st;
				di.ctx_.print(st);
				std::string s = st.str();
				std::cout << s;
			}
		} );

	std::cout << std::endl;

	leaf::try_catch(
		[]
		{
			LEAF_THROW( my_error(), leaf::e_errno{ENOENT} );
		},
		[]( leaf::e_source_location, leaf::e_errno, leaf::verbose_diagnostic_info const & vdi )
		{
			{
				std::ostringstream st;
				st << vdi;
				std::string s = st.str();
				BOOST_TEST(s.find("leaf::verbose_diagnostic_info")!=s.npos);
				std::cout << s;
			}
			std::cout << "polymorphic_context::print():" << std::endl;
			{
				std::ostringstream st;
				vdi.ctx_.print(st);
				std::string s = st.str();
				std::cout << s;
			}
		} );

	std::cout << std::endl;

	return boost::report_errors();
}
