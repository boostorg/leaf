// Copyright 2018-2024 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef BOOST_LEAF_TEST_SINGLE_HEADER
#   include "leaf.hpp"
#else
#   include <boost/leaf/result.hpp>
#   include <boost/leaf/handle_errors.hpp>
#endif

#if BOOST_LEAF_CFG_STD_STRING
#   include <sstream>
#   include <iostream>
#endif

#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

struct non_printable_value
{
};

template <int N>
struct e_err
{
    int value;
};

int main()
{
    {
        leaf::result<int> r = 42;
        BOOST_TEST(r);
#if BOOST_LEAF_CFG_STD_STRING
        std::ostringstream ss;
        ss << r;
        std::string s = ss.str();
        std::cout << s << std::endl;
        if( BOOST_LEAF_CFG_DIAGNOSTICS )
            BOOST_TEST_EQ(s, "Success: int: 42");
        else
            BOOST_TEST_EQ(s, "Success");
#endif
    }

    {
        leaf::result<non_printable_value> r;
        BOOST_TEST(r);
#if BOOST_LEAF_CFG_STD_STRING
        std::ostringstream ss;
        ss << r;
        std::string s = ss.str();
        std::cout << s << std::endl;
        if( BOOST_LEAF_CFG_DIAGNOSTICS )
            BOOST_TEST_EQ(s, "Success: non_printable_value");
        else
            BOOST_TEST_EQ(s, "Success");
#endif
    }

    {
        leaf::result<void> r;
        BOOST_TEST(r);
#if BOOST_LEAF_CFG_STD_STRING
        std::ostringstream ss;
        ss << r;
        std::string s = ss.str();
        std::cout << s << std::endl;
        BOOST_TEST_EQ(s, "Success");
#endif
    }

    {
        leaf::result<int> r = leaf::new_error();
        BOOST_TEST(!r);
#if BOOST_LEAF_CFG_STD_STRING
        std::ostringstream ss;
        ss << r;
        std::string s = ss.str();
        std::cout << s << std::endl;
        if( BOOST_LEAF_CFG_DIAGNOSTICS )
        {
            leaf::error_id err = r.error();
            BOOST_TEST_EQ(s, "Failure: boost::leaf::error_id: " + std::to_string(err.value()/4));
        }
        else
            BOOST_TEST_EQ(s, "Failure");
#endif
    }

#if BOOST_LEAF_CFG_CAPTURE
    {
        leaf::result<int> r = leaf::try_capture_all(
            []() -> leaf::result<int>
            {
                return leaf::new_error(e_err<1>{1}, e_err<2>{2});
            } );
#if BOOST_LEAF_CFG_STD_STRING
        std::ostringstream ss;
        ss << r;
        std::string s = ss.str();
        std::cout << s << std::endl;
        if( BOOST_LEAF_CFG_DIAGNOSTICS )
        {
            leaf::error_id err = r.error();
            BOOST_TEST_NE(s.find("Failure: boost::leaf::error_id: " + std::to_string(err.value()/4)), s.npos);
            BOOST_TEST_NE(s.find(", captured -> "), s.npos);
            BOOST_TEST_NE(s.find("e_err<1>: 1"), s.npos);
            BOOST_TEST_NE(s.find("e_err<2>: 2"), s.npos);
        }
        else
            BOOST_TEST_EQ(s, "Failure");
#endif
    }
#endif

    return boost::report_errors();
}
