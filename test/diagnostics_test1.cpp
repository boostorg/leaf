// Copyright 2018-2023 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef BOOST_LEAF_TEST_SINGLE_HEADER
#   include "leaf.hpp"
#else
#   include <boost/leaf/config.hpp>
#   include <boost/leaf/diagnostics.hpp>
#   include <boost/leaf/result.hpp>
#   include <boost/leaf/common.hpp>
#endif

#if BOOST_LEAF_CFG_STD_STRING
#   include <sstream>
#   include <iostream>
#endif

#include <algorithm>

#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

enum class enum_class
{
    enum0,
    enum1
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
        return "my_exception what";
    }
};

struct printable_value
{
    friend std::ostream & operator<<( std::ostream & os, printable_value const & )
    {
        return os << "printed printable_value";
    }
};

struct non_printable_value
{
};

struct printable_info_printable_value
{
    printable_value value;

    friend std::ostream & operator<<( std::ostream & os, printable_info_printable_value const & x )
    {
        return os << x.value;
    }
};

struct printable_info_non_printable_value
{
    non_printable_value value;

    friend std::ostream & operator<<( std::ostream & os, printable_info_non_printable_value const & )
    {
        return os << "*** printable_info non_printable_value ***";
    }
};

class non_printable_info_printable_value
{
public:
    printable_value value;
};

struct non_printable_info_non_printable_value
{
    non_printable_value value;
};

struct hidden_printable_info_printable_value
{
    printable_value value;

    friend std::ostream & operator<<( std::ostream & os, hidden_printable_info_printable_value const & x )
    {
        return os << x.value << " ***";
    }
};

struct hidden_non_printable_info_printable_value
{
    printable_value value;
};

namespace boost { namespace leaf {

template <> struct show_in_diagnostics<hidden_printable_info_printable_value>: std::false_type { };
template <> struct show_in_diagnostics<hidden_non_printable_info_printable_value>: std::false_type { };

} }

#if BOOST_LEAF_CFG_STD_STRING
namespace
{
    void remove_value(std::string & s, std::string const & name)
    {
        auto s1 = s.find(name);
        if( s1 == s.npos )
            return;
        auto s2 = s.find("\n\t", s1 + name.size());
        if( s1 == s.npos )
            return;
        s.replace(s1 + name.size(), s2 - s1 - name.size(), "<removed variance>");
    }

    void remove_variance(std::string & s)
    {
        s.erase(std::remove(s.begin(), s.end(), '\r'), s.end());
        remove_value(s, "boost::leaf::e_source_location: ");
        remove_value(s, "Caught C++ exception:\n\tType: ");
    }

    bool cmp(std::string a, std::string const & b) noexcept
    {
        remove_variance(a);
        auto i = a.begin();
        auto j = b.begin();
        for( auto e = i + std::min(a.size(), b.size()); i != e; ++i, ++j )
            if( *i != *j )
            {
                std::cout <<
                    "a =\n----\n" << a << "----\n\n"
                    "b =\n----\n" << b << "----\n\n"
                    "Difference immediately after:\n" << std::string(a.begin(), i) << '\n';
                return false;
            }
        return i == a.end() && j == b.end();
    }
}
#endif

#define BOOST_LEAF_DIAGNOSTIC_INFO_NO_BOOST_LEAF_CFG_DIAGNOSTICS "\nboost::leaf::diagnostic_info N/A due to BOOST_LEAF_CFG_DIAGNOSTICS=0\n"
#define BOOST_LEAF_DIAGNOSTIC_DETAILS_NO_BOOST_LEAF_CFG_CAPTURE "\nboost::leaf::diagnostic_details N/A due to BOOST_LEAF_CFG_CAPTURE=0\n"
#define BOOST_LEAF_DIAGNOSTIC_DETAILS_NO_BOOST_LEAF_CFG_DIAGNOSTICS "\nboost::leaf::diagnostic_details N/A due to BOOST_LEAF_CFG_DIAGNOSTICS=0\n"

int main()
{
    std::cout << __LINE__  << " ---- result / error_info\n";
    leaf::try_handle_all(
        []() -> leaf::result<void>
        {
            return BOOST_LEAF_NEW_ERROR(
                printable_info_printable_value(),
                printable_info_non_printable_value(),
                non_printable_info_printable_value(),
                non_printable_info_non_printable_value(),
                hidden_printable_info_printable_value(),
                hidden_non_printable_info_printable_value(),
                enum_class::enum0,
                leaf::e_errno{ENOENT} );
        },
        [](
            leaf::e_source_location,
            printable_info_printable_value,
            printable_info_non_printable_value,
            non_printable_info_printable_value,
            non_printable_info_non_printable_value,
            hidden_printable_info_printable_value,
            hidden_non_printable_info_printable_value,
            enum_class,
            leaf::e_errno,
            leaf::error_info const & unmatched )
        {
#if BOOST_LEAF_CFG_STD_STRING
            std::ostringstream st;
            st << unmatched;
            std::string s = st.str();
            std::cout << s << std::endl;
            BOOST_TEST(cmp(s,
                "Error serial #1\n"
            ));

#endif
        },
        []()
        {
            BOOST_ERROR("Bad error dispatch");
        } );

    std::cout << __LINE__  << " ---- result / diagnostic_info\n";
    leaf::try_handle_all(
        []() -> leaf::result<void>
        {
            return BOOST_LEAF_NEW_ERROR(
                printable_info_printable_value(),
                printable_info_non_printable_value(),
                non_printable_info_printable_value(),
                non_printable_info_non_printable_value(),
                hidden_printable_info_printable_value(),
                hidden_non_printable_info_printable_value(),
                enum_class::enum1,
                leaf::e_errno{ENOENT} );
        },
        [](
            leaf::e_source_location,
            printable_info_printable_value,
            printable_info_non_printable_value,
            non_printable_info_printable_value,
            non_printable_info_non_printable_value,
            hidden_printable_info_printable_value,
            hidden_non_printable_info_printable_value,
            enum_class,
            leaf::e_errno,
            leaf::diagnostic_info const & unmatched )
        {
#if BOOST_LEAF_CFG_STD_STRING
            std::ostringstream st;
            st << unmatched;
            std::string s = st.str();
            std::cout << s << std::endl;
            if( BOOST_LEAF_CFG_DIAGNOSTICS )
                BOOST_TEST(cmp(s,
                    "Error serial #2\n"
                    "Caught:"
                    "\n\tboost::leaf::e_source_location: <removed variance>"
                    "\n\tprintable_info_printable_value: printed printable_value"
                    "\n\tprintable_info_non_printable_value: *** printable_info non_printable_value ***"
                    "\n\tnon_printable_info_printable_value: printed printable_value"
                    "\n\tnon_printable_info_non_printable_value"
                    "\n\tenum_class: 1"
                    "\n\tboost::leaf::e_errno: 2, \"No such file or directory\"\n"
                ));
            else
                BOOST_TEST(cmp(s,
                    "Error serial #2"
                    BOOST_LEAF_DIAGNOSTIC_INFO_NO_BOOST_LEAF_CFG_DIAGNOSTICS
                ));
#endif
        },
        []()
        {
            BOOST_ERROR("Bad error dispatch");
        } );

    std::cout << __LINE__  << " ---- result / diagnostic_details\n";
    leaf::try_handle_all(
        []() -> leaf::result<void>
        {
            return BOOST_LEAF_NEW_ERROR(
                printable_info_printable_value(),
                printable_info_non_printable_value(),
                non_printable_info_printable_value(),
                non_printable_info_non_printable_value(),
                hidden_printable_info_printable_value(),
                hidden_non_printable_info_printable_value(),
                unexpected_test<1>{1},
                unexpected_test<2>{2},
                enum_class::enum0,
                leaf::e_errno{ENOENT} );
        },
        [](
            leaf::e_source_location,
            printable_info_printable_value,
            printable_info_non_printable_value,
            non_printable_info_printable_value,
            non_printable_info_non_printable_value,
            hidden_printable_info_printable_value,
            hidden_non_printable_info_printable_value,
            enum_class,
            leaf::e_errno,
            leaf::diagnostic_details const & di )
        {
#if BOOST_LEAF_CFG_STD_STRING
            std::ostringstream st;
            st << di;
            std::string s = st.str();
            std::cout << s << std::endl;
            if( BOOST_LEAF_CFG_DIAGNOSTICS )
                if( BOOST_LEAF_CFG_CAPTURE )
                    BOOST_TEST(cmp(s,
                        "Error serial #3\n"
                        "Caught:"
                        "\n\tboost::leaf::e_source_location: <removed variance>"
                        "\n\tprintable_info_printable_value: printed printable_value"
                        "\n\tprintable_info_non_printable_value: *** printable_info non_printable_value ***"
                        "\n\tnon_printable_info_printable_value: printed printable_value"
                        "\n\tnon_printable_info_non_printable_value"
                        "\n\tenum_class: 0"
                        "\n\tboost::leaf::e_errno: 2, \"No such file or directory\"\n"
                        "Diagnostic details:"
                        "\n\tunexpected_test<1>: 1"
                        "\n\tunexpected_test<2>: 2\n"
                    ));
                else
                    BOOST_TEST(cmp(s,
                        "Error serial #3\n"
                        "Caught:"
                        "\n\tboost::leaf::e_source_location: <removed variance>"
                        "\n\tprintable_info_printable_value: printed printable_value"
                        "\n\tprintable_info_non_printable_value: *** printable_info non_printable_value ***"
                        "\n\tnon_printable_info_printable_value: printed printable_value"
                        "\n\tnon_printable_info_non_printable_value"
                        "\n\tenum_class: 0"
                        "\n\tboost::leaf::e_errno: 2, \"No such file or directory\""
                        BOOST_LEAF_DIAGNOSTIC_DETAILS_NO_BOOST_LEAF_CFG_CAPTURE
                    ));
            else
                BOOST_TEST(cmp(s,
                    "Error serial #3"
                    BOOST_LEAF_DIAGNOSTIC_DETAILS_NO_BOOST_LEAF_CFG_DIAGNOSTICS
                ));
#endif
        },
        []()
        {
            BOOST_ERROR("Bad error dispatch");
        } );

    std::cout << __LINE__  << " ---- result / diagnostic_details / nothing caught\n";
    leaf::try_handle_all(
        []() -> leaf::result<void>
        {
            return BOOST_LEAF_NEW_ERROR(
                printable_info_printable_value(),
                printable_info_non_printable_value(),
                non_printable_info_printable_value(),
                non_printable_info_non_printable_value(),
                hidden_printable_info_printable_value(),
                hidden_non_printable_info_printable_value(),
                unexpected_test<1>{1},
                unexpected_test<2>{2},
                enum_class::enum0,
                leaf::e_errno{ENOENT} );
        },
        []( leaf::diagnostic_details const & di )
        {
#if BOOST_LEAF_CFG_STD_STRING
            std::ostringstream st;
            st << di;
            std::string s = st.str();
            std::cout << s << std::endl;
            if( BOOST_LEAF_CFG_DIAGNOSTICS )
                if( BOOST_LEAF_CFG_CAPTURE )
                    BOOST_TEST(cmp(s,
                        "Error serial #4\n"
                        "Diagnostic details:"
                        "\n\tprintable_info_printable_value: printed printable_value"
                        "\n\tprintable_info_non_printable_value: *** printable_info non_printable_value ***"
                        "\n\tnon_printable_info_printable_value: printed printable_value"
                        "\n\tnon_printable_info_non_printable_value"
                        "\n\tunexpected_test<1>: 1"
                        "\n\tunexpected_test<2>: 2"
                        "\n\tenum_class: 0"
                        "\n\tboost::leaf::e_errno: 2, \"No such file or directory\""
                        "\n\tboost::leaf::e_source_location: <removed variance>"
                    ));
                else
                    BOOST_TEST(cmp(s,
                        "Error serial #4"
                        BOOST_LEAF_DIAGNOSTIC_DETAILS_NO_BOOST_LEAF_CFG_CAPTURE
                    ));
            else
                BOOST_TEST(cmp(s,
                    "Error serial #4"
                    BOOST_LEAF_DIAGNOSTIC_DETAILS_NO_BOOST_LEAF_CFG_DIAGNOSTICS
                ));
#endif
        },
        []()
        {
            BOOST_ERROR("Bad error dispatch");
        } );

    ///////////////////////////////////

#ifndef BOOST_LEAF_NO_EXCEPTIONS

    std::cout << __LINE__  << " ---- exception / error_info\n";
    leaf::try_catch(
        []
        {
            BOOST_LEAF_THROW_EXCEPTION( my_exception(),
                printable_info_printable_value(),
                printable_info_non_printable_value(),
                non_printable_info_printable_value(),
                non_printable_info_non_printable_value(),
                hidden_printable_info_printable_value(),
                hidden_non_printable_info_printable_value(),
                enum_class::enum1,
                leaf::e_errno{ENOENT} );
        },
        [](
            leaf::e_source_location,
            printable_info_printable_value,
            printable_info_non_printable_value,
            non_printable_info_printable_value,
            non_printable_info_non_printable_value,
            hidden_printable_info_printable_value,
            hidden_non_printable_info_printable_value,
            enum_class,
            leaf::e_errno,
            leaf::error_info const & unmatched )
        {
#if BOOST_LEAF_CFG_STD_STRING
            std::ostringstream st;
            st << unmatched;
            std::string s = st.str();
            std::cout << s << std::endl;
            BOOST_TEST(cmp(s,
                "Error serial #5\n"
                "Caught C++ exception:"
                "\n\tType: <removed variance>"
                "\n\tstd::exception::what(): my_exception what\n"
            ));
#endif
        } );

    std::cout << __LINE__  << " ---- exception / diagnostic_info\n";
    leaf::try_catch(
        []
        {
            BOOST_LEAF_THROW_EXCEPTION( my_exception(),
                printable_info_printable_value(),
                printable_info_non_printable_value(),
                non_printable_info_printable_value(),
                non_printable_info_non_printable_value(),
                hidden_printable_info_printable_value(),
                hidden_non_printable_info_printable_value(),
                enum_class::enum0,
                leaf::e_errno{ENOENT} );
        },
        [](
            leaf::e_source_location,
            printable_info_printable_value,
            printable_info_non_printable_value,
            non_printable_info_printable_value,
            non_printable_info_non_printable_value,
            hidden_printable_info_printable_value,
            hidden_non_printable_info_printable_value,
            enum_class,
            leaf::e_errno,
            leaf::diagnostic_info const & unmatched )
        {
#if BOOST_LEAF_CFG_STD_STRING
            std::ostringstream st;
            st << unmatched;
            std::string s = st.str();
            std::cout << s << std::endl;
            if( BOOST_LEAF_CFG_DIAGNOSTICS )
                BOOST_TEST(cmp(s,
                    "Error serial #6\n"
                    "Caught C++ exception:"
                    "\n\tType: <removed variance>"
                    "\n\tstd::exception::what(): my_exception what"
                    "\n\tboost::leaf::e_source_location: <removed variance>"
                    "\n\tprintable_info_printable_value: printed printable_value"
                    "\n\tprintable_info_non_printable_value: *** printable_info non_printable_value ***"
                    "\n\tnon_printable_info_printable_value: printed printable_value"
                    "\n\tnon_printable_info_non_printable_value"
                    "\n\tenum_class: 0"
                    "\n\tboost::leaf::e_errno: 2, \"No such file or directory\"\n"
                ));
            else
                BOOST_TEST(cmp(s,
                    "Error serial #6\n"
                    "Caught C++ exception:"
                    "\n\tType: <removed variance>"
                    "\n\tstd::exception::what(): my_exception what"
                    BOOST_LEAF_DIAGNOSTIC_INFO_NO_BOOST_LEAF_CFG_DIAGNOSTICS
                ));
#endif
        } );

    std::cout << __LINE__  << " ---- exception / diagnostic_details\n";
    leaf::try_catch(
        []
        {
            BOOST_LEAF_THROW_EXCEPTION( my_exception(),
                printable_info_printable_value(),
                printable_info_non_printable_value(),
                non_printable_info_printable_value(),
                non_printable_info_non_printable_value(),
                hidden_printable_info_printable_value(),
                hidden_non_printable_info_printable_value(),
                enum_class::enum1,
                unexpected_test<1>{1},
                unexpected_test<2>{2},
                leaf::e_errno{ENOENT} );
        },
        [](
            leaf::e_source_location,
            printable_info_printable_value,
            printable_info_non_printable_value,
            non_printable_info_printable_value,
            non_printable_info_non_printable_value,
            hidden_printable_info_printable_value,
            hidden_non_printable_info_printable_value,
            enum_class,
            leaf::e_errno,
            leaf::diagnostic_details const & di )
        {
#if BOOST_LEAF_CFG_STD_STRING
            std::ostringstream st;
            st << di;
            std::string s = st.str();
            std::cout << s << std::endl;
            if( BOOST_LEAF_CFG_DIAGNOSTICS )
                if( BOOST_LEAF_CFG_CAPTURE )
                    BOOST_TEST(cmp(s,
                        "Error serial #7\n"
                        "Caught C++ exception:"
                        "\n\tType: <removed variance>"
                        "\n\tstd::exception::what(): my_exception what"
                        "\n\tboost::leaf::e_source_location: <removed variance>"
                        "\n\tprintable_info_printable_value: printed printable_value"
                        "\n\tprintable_info_non_printable_value: *** printable_info non_printable_value ***"
                        "\n\tnon_printable_info_printable_value: printed printable_value"
                        "\n\tnon_printable_info_non_printable_value"
                        "\n\tenum_class: 1"
                        "\n\tboost::leaf::e_errno: 2, \"No such file or directory\"\n"
                        "Diagnostic details:"
                        "\n\tunexpected_test<1>: 1"
                        "\n\tunexpected_test<2>: 2\n"
                    ));
                else
                    BOOST_TEST(cmp(s,
                        "Error serial #7\n"
                        "Caught C++ exception:"
                        "\n\tType: <removed variance>"
                        "\n\tstd::exception::what(): my_exception what"
                        "\n\tboost::leaf::e_source_location: <removed variance>"
                        "\n\tprintable_info_printable_value: printed printable_value"
                        "\n\tprintable_info_non_printable_value: *** printable_info non_printable_value ***"
                        "\n\tnon_printable_info_printable_value: printed printable_value"
                        "\n\tnon_printable_info_non_printable_value"
                        "\n\tenum_class: 1"
                        "\n\tboost::leaf::e_errno: 2, \"No such file or directory\""
                        BOOST_LEAF_DIAGNOSTIC_DETAILS_NO_BOOST_LEAF_CFG_CAPTURE
                    ));
            else
                BOOST_TEST(cmp(s,
                    "Error serial #7\n"
                    "Caught C++ exception:"
                    "\n\tType: <removed variance>"
                    "\n\tstd::exception::what(): my_exception what"
                    BOOST_LEAF_DIAGNOSTIC_DETAILS_NO_BOOST_LEAF_CFG_DIAGNOSTICS
                ));
#endif
        } );

    std::cout << __LINE__  << " ---- exception / diagnostic_details / nothing caught\n";
    leaf::try_catch(
        []
        {
            BOOST_LEAF_THROW_EXCEPTION( my_exception(),
                printable_info_printable_value(),
                printable_info_non_printable_value(),
                non_printable_info_printable_value(),
                non_printable_info_non_printable_value(),
                hidden_printable_info_printable_value(),
                hidden_non_printable_info_printable_value(),
                enum_class::enum1,
                unexpected_test<1>{1},
                unexpected_test<2>{2},
                leaf::e_errno{ENOENT} );
        },
        []( leaf::diagnostic_details const & di )
        {
#if BOOST_LEAF_CFG_STD_STRING
            std::ostringstream st;
            st << di;
            std::string s = st.str();
            std::cout << s << std::endl;
            if( BOOST_LEAF_CFG_DIAGNOSTICS )
                if( BOOST_LEAF_CFG_CAPTURE )
                    BOOST_TEST(cmp(s,
                        "Error serial #8\n"
                        "Caught C++ exception:"
                        "\n\tType: <removed variance>"
                        "\n\tstd::exception::what(): my_exception what\n"
                        "Diagnostic details:"
                        "\n\tprintable_info_printable_value: printed printable_value"
                        "\n\tprintable_info_non_printable_value: *** printable_info non_printable_value ***"
                        "\n\tnon_printable_info_printable_value: printed printable_value"
                        "\n\tnon_printable_info_non_printable_value"
                        "\n\tenum_class: 1"
                        "\n\tunexpected_test<1>: 1"
                        "\n\tunexpected_test<2>: 2"
                        "\n\tboost::leaf::e_errno: 2, \"No such file or directory\""
                        "\n\tboost::leaf::e_source_location: <removed variance>"
                    ));
                else
                    BOOST_TEST(cmp(s,
                        "Error serial #8\n"
                        "Caught C++ exception:"
                        "\n\tType: <removed variance>"
                        "\n\tstd::exception::what(): my_exception what"
                        BOOST_LEAF_DIAGNOSTIC_DETAILS_NO_BOOST_LEAF_CFG_CAPTURE
                    ));
            else
                BOOST_TEST(cmp(s,
                    "Error serial #8\n"
                    "Caught C++ exception:"
                    "\n\tType: <removed variance>"
                    "\n\tstd::exception::what(): my_exception what"
                    BOOST_LEAF_DIAGNOSTIC_DETAILS_NO_BOOST_LEAF_CFG_DIAGNOSTICS
                ));
#endif
        } );

#endif

    return boost::report_errors();
}
