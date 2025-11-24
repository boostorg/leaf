#ifndef SO_DLL_TEST_LIB1_HPP_INCLUDED
#define SO_DLL_TEST_LIB1_HPP_INCLUDED

// Copyright 2018-2024 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "so_dll_test.hpp"

#ifdef _WIN32
#   ifdef BOOST_LEAF_SO_DLL_TEST_BUILDING_LIB1
#       define BOOST_LEAF_SO_DLL_TEST_LIB1_API __declspec(dllexport)
#   else
#       define BOOST_LEAF_SO_DLL_TEST_LIB1_API __declspec(dllimport)
#   endif
#else
#   define BOOST_LEAF_SO_DLL_TEST_LIB1_API BOOST_LEAF_SYMBOL_VISIBLE
#endif

namespace boost { namespace leaf {
    template <class T> class result;
} }

BOOST_LEAF_SO_DLL_TEST_LIB1_API boost::leaf::result<void> hidden_result1();

#ifndef BOOST_LEAF_NO_EXCEPTIONS
BOOST_LEAF_SO_DLL_TEST_LIB1_API void hidden_throw1();
#endif

#endif

