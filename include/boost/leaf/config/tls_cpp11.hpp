#ifndef BOOST_LEAF_CONFIG_TLS_CPP11_HPP_INCLUDED
#define BOOST_LEAF_CONFIG_TLS_CPP11_HPP_INCLUDED

// Copyright 2018-2024 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// LEAF requires thread local storage support for pointers and for uin32_t values.

// This header implements thread local storage for pointers and for unsigned int
// values using the C++11 built-in thread_local storage class specifier.

#include <cstdint>

namespace boost { namespace leaf {

namespace detail
{
    using atomic_unsigned_int = std::atomic<unsigned int>;
}

namespace tls
{
    template <class T>
    struct BOOST_LEAF_SYMBOL_VISIBLE ptr
    {
        static thread_local T * p;
    };

    template <class T>
    thread_local T * ptr<T>::p;

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

    struct current_error_id_storage
    {
        static thread_local unsigned x;
    };

    thread_local unsigned current_error_id_storage::x;

    inline unsigned read_current_error_id() noexcept
    {
        return current_error_id_storage::x;
    }

    inline void write_current_error_id( unsigned x ) noexcept
    {
        current_error_id_storage::x = x;
    }
}

} }

#endif // BOOST_LEAF_CONFIG_TLS_CPP11_HPP_INCLUDED
