// Copyright 2018-2024 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef BOOST_LEAF_TEST_SINGLE_HEADER
#   include "leaf.hpp"
#else
#   include <boost/leaf/config.hpp>
#   include <boost/leaf/diagnostics.hpp>
#   include <boost/leaf/on_error.hpp>
#   include <boost/leaf/result.hpp>
#endif

#if BOOST_LEAF_CFG_STD_STRING
#   include <sstream>
#   include <iostream>
#endif

#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int I>
struct info
{
    friend std::ostream & operator<<( std::ostream & os, info const & )
    {
        return os << "printed info<" << I << '>';
    }
};

int main()
{
    leaf::try_handle_all(
        []() -> leaf::result<void>
        {
            return leaf::try_handle_some(
                []() -> leaf::result<void>
                {
                    leaf::try_handle_all(
                        []() -> leaf::result<void>
                        {
                            return leaf::new_error(info<1>{});
                        },
                        []
                        {
                        } );
                    return leaf::new_error(info<2>{});
                },
                []( info<1> )
                {
                } );
        },
        []( leaf::diagnostic_details const & di )
        {
#if BOOST_LEAF_CFG_STD_STRING
            std::ostringstream st;
            st << di;
            std::string s = st.str();
            std::cout << s << std::endl;
            if( BOOST_LEAF_CFG_DIAGNOSTICS && BOOST_LEAF_CFG_CAPTURE )
                BOOST_TEST_NE(s.find("info<2>"), s.npos);
#endif
        } );

#ifndef BOOST_LEAF_NO_EXCEPTIONS

    std::cout << __LINE__  << " ----\n";

    leaf::try_catch(
        []
        {
            leaf::try_catch(
                []
                {
                    leaf::try_catch(
                        []
                        {
                            leaf::throw_exception(info<1>{});
                        },
                        []
                        {
                        } );
                    leaf::throw_exception(info<2>{});
                },
                []( info<1> )
                {
                } );
        },
        []( leaf::diagnostic_details const & di )
        {
#if BOOST_LEAF_CFG_STD_STRING
            std::ostringstream st;
            st << di;
            std::string s = st.str();
            std::cout << s << std::endl;
            if( BOOST_LEAF_CFG_DIAGNOSTICS && BOOST_LEAF_CFG_CAPTURE )
                BOOST_TEST_NE(s.find("info<2>"), s.npos);
#endif
        } );

    std::cout << __LINE__  << " ----\n";

    leaf::try_catch(
        []
        {
            leaf::try_catch(
                []
                {
                    leaf::try_catch(
                        []
                        {
                            auto load = leaf::on_error(info<1>{});
                            throw std::exception();
                        },
                        []
                        {
                        } );
                    auto load = leaf::on_error(info<2>{});
                    throw std::exception();
                },
                []( info<1> )
                {
                } );
        },
        []( leaf::diagnostic_details const & di )
        {
#if BOOST_LEAF_CFG_STD_STRING
            std::ostringstream st;
            st << di;
            std::string s = st.str();
            std::cout << s << std::endl;
            if( BOOST_LEAF_CFG_DIAGNOSTICS && BOOST_LEAF_CFG_CAPTURE )
                BOOST_TEST_NE(s.find("info<2>"), s.npos);
#endif
        } );

#endif

    return boost::report_errors();
}
