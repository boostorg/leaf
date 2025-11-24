// Copyright 2018-2024 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef BOOST_LEAF_TEST_SINGLE_HEADER
#   include "leaf.hpp"
#else
#   include <boost/leaf/exception.hpp>
#   include <boost/leaf/result.hpp>
#   include <boost/leaf/on_error.hpp>
#endif

#ifdef BOOST_LEAF_NO_EXCEPTIONS
#include <iostream>
namespace boost
{
    [[noreturn]] void throw_exception( std::exception const & e )
    {
        std::cerr << "Terminating due to a C++ exception under BOOST_LEAF_NO_EXCEPTIONS: " << e.what();
        std::terminate();
    }

    struct source_location;
    [[noreturn]] void throw_exception( std::exception const & e, boost::source_location const & )
    {
        throw_exception(e);
    }
}
#endif

#define BOOST_LEAF_SO_DLL_TEST_BUILDING_LIB1

#include "so_dll_lib1.hpp"

namespace leaf = boost::leaf;

BOOST_LEAF_SO_DLL_TEST_LIB1_API leaf::result<void> hidden_result1()
{
    auto load = leaf::on_error( my_info<1>{1}, my_info<3>{3} );
    return leaf::new_error( my_info<2>{2} );
}

#ifndef BOOST_LEAF_NO_EXCEPTIONS

BOOST_LEAF_SO_DLL_TEST_LIB1_API void hidden_throw1()
{
    auto load = leaf::on_error( my_info<1>{1}, my_info<3>{3} );
    leaf::throw_exception( my_info<2>{2} );
}

#endif
