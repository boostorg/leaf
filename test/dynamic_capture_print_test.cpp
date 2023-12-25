// Copyright 2018-2023 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>

#if !BOOST_LEAF_CFG_CAPTURE || !BOOST_LEAF_CFG_STD_STRING || !BOOST_LEAF_CFG_DIAGNOSTICS

#include <iostream>

int main()
{
    std::cout << "Unit test not applicable." << std::endl;
    return 0;
}

#else

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

struct e_err
{
    template <class CharT, class Traits>
    friend std::ostream & operator<<( std::basic_ostream<CharT, Traits> & os, e_err const & )
    {
        return os << "e_err";
    }
};

int main()
{
    leaf::try_handle_all(
        []() -> leaf::result<void>
        {
            return leaf::new_error(e_err{ });
        },
        [&]( leaf::dynamic_capture const & cap )
        {
            std::ostringstream st;
            st << cap;
            std::string s = st.str();
            std::cout << s << std::endl;
            BOOST_TEST_NE(s.find("Captured error objects"), s.npos);
            BOOST_TEST_NE(s.find("e_err"), s.npos);
        } );

    return boost::report_errors();
}

#endif
