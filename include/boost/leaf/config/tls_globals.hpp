#ifndef BOOST_LEAF_CONFIG_TLS_GLOBALS_HPP_INCLUDED
#define BOOST_LEAF_CONFIG_TLS_GLOBALS_HPP_INCLUDED

// Copyright 2018-2024 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// LEAF requires thread local storage support for pointers and for uin32_t values.

// This header implements "thread local" storage for pointers and for unsigned int
// values using globals, which is suitable for single thread environments.

#include <cstdint>

namespace boost { namespace leaf {

namespace detail
{
    using atomic_unsigned_int = unsigned int;
}

namespace tls
{
    template <class T>
    struct BOOST_LEAF_SYMBOL_VISIBLE ptr
    {
        static T * p;
    };

    template <class T>
    T * ptr<T>::p;

    template <class T>
    T * read_ptr() noexcept
    {
        return ptr<T>::p;
    }

    template <class T>
    void alloc_write_ptr( T * p ) noexcept
    {
        ptr<T>::p = p;
    }

    template <class T>
    void write_ptr( T * p ) noexcept
    {
        ptr<T>::p = p;
    }

    ////////////////////////////////////////

    inline unsigned read_current_error_id() noexcept
    {
        static unsigned x = 0;
        return x;
    }

    inline void write_current_error_id( unsigned v ) noexcept
    {
        static unsigned x = 0;
        x = v;
    }
}

} }

#endif // BOOST_LEAF_CONFIG_TLS_GLOBALS_HPP_INCLUDED
