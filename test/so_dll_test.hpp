#ifndef SO_DLL_TEST_HPP_INCLUDED
#define SO_DLL_TEST_HPP_INCLUDED

// Copyright 2018-2024 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>

#if defined(_WIN32) && BOOST_LEAF_CFG_WIN32 < 2
#   error This test requires BOOST_LEAF_CFG_WIN32 >= 2
#endif

// Note, under BOOST_LEAF_CFG_WIN32==2 (using Win32 TLS) we do not need
// import/export for error types. We still need default visibility on POSIX.
template <int Tag>
struct [[gnu::visibility("default")]] my_info
{
    int value;

    template <class CharT, class Traits>
    friend std::ostream & operator<<( std::basic_ostream<CharT, Traits> & os, my_info const & x )
    {
        return os << "Test my_info<" << Tag << ">::value = " << x.value;
    }
};

#endif

