// Copyright 2018-2023 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef BOOST_LEAF_TEST_SINGLE_HEADER
#   include "leaf.hpp"
#else
#   include <boost/leaf/config.hpp>
#   include <boost/leaf/handle_errors.hpp>
#   include <boost/leaf/result.hpp>
#   include <boost/leaf/common.hpp>
#endif

#if BOOST_LEAF_CFG_STD_STRING
#   include <sstream>
#   include <iostream>
#endif

#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

enum class enum_class_payload
{
    value
};

template <int A>
struct unexpected_test
{
    int value;
};

struct my_exception:
    virtual std::exception
{
    char const * what() const noexcept
    {
        return "my_exception";
    }
};

struct printable_payload
{
    friend std::ostream & operator<<( std::ostream & os, printable_payload const & )
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

    friend std::ostream & operator<<( std::ostream & os, printable_info_non_printable_payload const & )
    {
        return os << "*** printable_info_non_printable_payload ***";
    }
};

class non_printable_info_printable_payload
{
public:
    printable_payload value;
};

struct non_printable_info_non_printable_payload
{
    non_printable_payload value;
};

int main()
{
    leaf::try_handle_all(
        []() -> leaf::result<void>
        {
            return BOOST_LEAF_NEW_ERROR(
                printable_info_printable_payload(),
                printable_info_non_printable_payload(),
                non_printable_info_printable_payload(),
                non_printable_info_non_printable_payload(),
                enum_class_payload{},
                leaf::e_errno{ENOENT} );
        },
        [](
            leaf::e_source_location,
            printable_info_printable_payload,
            printable_info_non_printable_payload,
            non_printable_info_printable_payload,
            non_printable_info_non_printable_payload,
            enum_class_payload,
            leaf::e_errno,
            leaf::error_info const & unmatched )
        {
#if BOOST_LEAF_CFG_STD_STRINfG
            std::ostringstream st;
            st << unmatched;
            std::string s = st.str();
            std::cout << s << std::endl;
            BOOST_TEST_NE(s.find("Error ID: "), s.npos);
#endif
        },
        []()
        {
            BOOST_ERROR("Bad error dispatch");
        } );

    std::cout << __LINE__  << " ---- diagnostic_info\n";
    leaf::try_handle_all(
        []() -> leaf::result<void>
        {
            return BOOST_LEAF_NEW_ERROR(
                printable_info_printable_payload(),
                printable_info_non_printable_payload(),
                non_printable_info_printable_payload(),
                non_printable_info_non_printable_payload(),
                enum_class_payload{},
                leaf::e_errno{ENOENT} );
        },
        [](
            leaf::e_source_location,
            printable_info_printable_payload,
            printable_info_non_printable_payload,
            non_printable_info_printable_payload,
            non_printable_info_non_printable_payload,
            enum_class_payload,
            leaf::e_errno,
            leaf::diagnostic_info const & unmatched )
        {
#if BOOST_LEAF_CFG_STD_STRING
            std::ostringstream st;
            st << unmatched;
            std::string s = st.str();
            std::cout << s << std::endl;
            BOOST_TEST_NE(s.find("Error ID: "), s.npos);
            if( BOOST_LEAF_CFG_DIAGNOSTICS )
            {
                BOOST_TEST_NE(s.find("e_source_location"), s.npos);
                BOOST_TEST_NE(s.find("*** printable_info_printable_payload printed printable_payload ***"), s.npos);
                BOOST_TEST_NE(s.find("*** printable_info_non_printable_payload ***"), s.npos);
                BOOST_TEST_NE(s.find(": printed printable_payload"), s.npos);
                BOOST_TEST_NE(s.find(": {not printable}"), s.npos);
                BOOST_TEST_NE(s.find("enum_class_payload"), s.npos);
            }
            else
                BOOST_TEST_NE(s.find("diagnostic_info not available due to BOOST_LEAF_CFG_DIAGNOSTICS=0"), s.npos);
#endif
        },
        []()
        {
            BOOST_ERROR("Bad error dispatch");
        } );

    std::cout << __LINE__  << " ---- verbose_diagnostic_info\n";
    leaf::try_handle_all(
        []() -> leaf::result<void>
        {
            return BOOST_LEAF_NEW_ERROR(
                printable_info_printable_payload(),
                printable_info_non_printable_payload(),
                non_printable_info_printable_payload(),
                non_printable_info_non_printable_payload(),
                unexpected_test<1>{1},
                unexpected_test<2>{2},
                enum_class_payload{},
                leaf::e_errno{ENOENT} );
        },
        [](
            leaf::e_source_location,
            printable_info_printable_payload,
            printable_info_non_printable_payload,
            non_printable_info_printable_payload,
            non_printable_info_non_printable_payload,
            enum_class_payload,
            leaf::e_errno,
            leaf::verbose_diagnostic_info const & di )
        {
#if BOOST_LEAF_CFG_STD_STRING
            std::ostringstream st;
            st << di;
            std::string s = st.str();
            std::cout << s << std::endl;
            BOOST_TEST_NE(s.find("Error ID: "), s.npos);
            if( BOOST_LEAF_CFG_DIAGNOSTICS )
            {
                BOOST_TEST_NE(s.find("e_source_location"), s.npos);
                BOOST_TEST_NE(s.find("*** printable_info_printable_payload printed printable_payload ***"), s.npos);
                BOOST_TEST_NE(s.find("*** printable_info_non_printable_payload ***"), s.npos);
                BOOST_TEST_NE(s.find(": printed printable_payload"), s.npos);
                BOOST_TEST_NE(s.find(": {not printable}"), s.npos);
                BOOST_TEST_NE(s.find("enum_class"), s.npos);
                BOOST_TEST_EQ(s.find("dynamic_allocator"), s.npos);
                if( BOOST_LEAF_CFG_CAPTURE )
                {
                    BOOST_TEST_NE(s.find("Unhandled error objects:"), s.npos);
                    BOOST_TEST_NE(s.find("unexpected_test<1>"), s.npos);
                    BOOST_TEST_NE(s.find("unexpected_test<2>"), s.npos);
                    BOOST_TEST_NE(s.find(": 1"), s.npos);
                    BOOST_TEST_NE(s.find(": 2"), s.npos);
                }
                else
                    BOOST_TEST_NE(s.find("verbose_diagnostic_info not available due to BOOST_LEAF_CFG_CAPTURE=0"), s.npos);
            }
            else
                BOOST_TEST_NE(s.find("verbose_diagnostic_info not available due to BOOST_LEAF_CFG_DIAGNOSTICS=0"), s.npos);
#endif
        },
        []()
        {
            BOOST_ERROR("Bad error dispatch");
        } );

    ///////////////////////////////////

#ifndef BOOST_LEAF_NO_EXCEPTIONS

    std::cout << __LINE__  << " ---- error_info\n";
    leaf::try_catch(
        []
        {
            BOOST_LEAF_THROW_EXCEPTION( my_exception(),
                printable_info_printable_payload(),
                printable_info_non_printable_payload(),
                non_printable_info_printable_payload(),
                non_printable_info_non_printable_payload(),
                enum_class_payload{},
                leaf::e_errno{ENOENT} );
        },
        [](
            leaf::e_source_location,
            printable_info_printable_payload,
            printable_info_non_printable_payload,
            non_printable_info_printable_payload,
            non_printable_info_non_printable_payload,
            enum_class_payload,
            leaf::e_errno,
            leaf::error_info const & unmatched )
        {
#if BOOST_LEAF_CFG_STD_STRING
            std::ostringstream st;
            st << unmatched;
            std::string s = st.str();
            std::cout << s << std::endl;
            if( BOOST_LEAF_CFG_DIAGNOSTICS )
            {
                BOOST_TEST_NE(s.find("Exception dynamic type: "), s.npos);
                BOOST_TEST_NE(s.find("std::exception::what(): my_exception"), s.npos);
            }
#endif
        } );

    std::cout << __LINE__  << " ---- diagnostic_info\n";
    leaf::try_catch(
        []
        {
            BOOST_LEAF_THROW_EXCEPTION( my_exception(),
                printable_info_printable_payload(),
                printable_info_non_printable_payload(),
                non_printable_info_printable_payload(),
                non_printable_info_non_printable_payload(),
                enum_class_payload{},
                leaf::e_errno{ENOENT} );
        },
        [](
            leaf::e_source_location,
            printable_info_printable_payload,
            printable_info_non_printable_payload,
            non_printable_info_printable_payload,
            non_printable_info_non_printable_payload,
            enum_class_payload,
            leaf::e_errno,
            leaf::diagnostic_info const & unmatched )
        {
#if BOOST_LEAF_CFG_STD_STRING
            std::ostringstream st;
            st << unmatched;
            std::string s = st.str();
            std::cout << s << std::endl;
            BOOST_TEST_NE(s.find("Error ID: "), s.npos);
            BOOST_TEST_NE(s.find("Exception dynamic type: "), s.npos);
            BOOST_TEST_NE(s.find("std::exception::what(): my_exception"), s.npos);
            if( BOOST_LEAF_CFG_DIAGNOSTICS )
            {
                BOOST_TEST_NE(s.find("e_source_location"), s.npos);
                BOOST_TEST_NE(s.find("*** printable_info_printable_payload printed printable_payload ***"), s.npos);
                BOOST_TEST_NE(s.find("*** printable_info_non_printable_payload ***"), s.npos);
                BOOST_TEST_NE(s.find(": printed printable_payload"), s.npos);
                BOOST_TEST_NE(s.find(": {not printable}"), s.npos);
                BOOST_TEST_NE(s.find("enum_class_payload"), s.npos);
            }
            else
                BOOST_TEST_NE(s.find("diagnostic_info not available"), s.npos);
#endif
        } );

    std::cout << __LINE__  << " ---- verbose_diagnostic_info\n";
    leaf::try_catch(
        []
        {
            BOOST_LEAF_THROW_EXCEPTION( my_exception(),
                printable_info_printable_payload(),
                printable_info_non_printable_payload(),
                non_printable_info_printable_payload(),
                non_printable_info_non_printable_payload(),
                enum_class_payload{},
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
            enum_class_payload,
            leaf::e_errno,
            leaf::verbose_diagnostic_info const & di )
        {
#if BOOST_LEAF_CFG_STD_STRING
            std::ostringstream st;
            st << di;
            std::string s = st.str();
            std::cout << s << std::endl;
            BOOST_TEST_NE(s.find("Error ID: "), s.npos);
            BOOST_TEST_NE(s.find("Exception dynamic type: "), s.npos);
            BOOST_TEST_NE(s.find("std::exception::what(): my_exception"), s.npos);
            if( BOOST_LEAF_CFG_DIAGNOSTICS )
            {
                BOOST_TEST_NE(s.find("e_source_location"), s.npos);
                BOOST_TEST_NE(s.find("*** printable_info_printable_payload printed printable_payload ***"), s.npos);
                BOOST_TEST_NE(s.find("*** printable_info_non_printable_payload ***"), s.npos);
                BOOST_TEST_NE(s.find(": printed printable_payload"), s.npos);
                BOOST_TEST_NE(s.find(": {not printable}"), s.npos);
                BOOST_TEST_NE(s.find("enum_class_payload"), s.npos);
                BOOST_TEST_EQ(s.find("dynamic_allocator"), s.npos);
                if( BOOST_LEAF_CFG_CAPTURE )
                {
                    BOOST_TEST_NE(s.find("Unhandled error objects:"), s.npos);
                    BOOST_TEST_NE(s.find("unexpected_test<1>"), s.npos);
                    BOOST_TEST_NE(s.find("unexpected_test<2>"), s.npos);
                    BOOST_TEST_NE(s.find(": 1"), s.npos);
                    BOOST_TEST_NE(s.find(": 2"), s.npos);
                }
                else
                    BOOST_TEST_NE(s.find("verbose_diagnostic_info not available due to BOOST_LEAF_CFG_CAPTURE=0"), s.npos);
            }
            else
                BOOST_TEST_NE(s.find("verbose_diagnostic_info not available due to BOOST_LEAF_CFG_DIAGNOSTICS=0"), s.npos);
#endif
        } );

#endif

    return boost::report_errors();
}
