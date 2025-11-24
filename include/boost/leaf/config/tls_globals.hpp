#ifndef BOOST_LEAF_CONFIG_TLS_GLOBALS_HPP_INCLUDED
#define BOOST_LEAF_CONFIG_TLS_GLOBALS_HPP_INCLUDED

// Copyright 2018-2025 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This header implements the TLS API specified in tls.hpp using globals, which
// is suitable for single thread environments.

#include <cstdint>

namespace boost { namespace leaf {

namespace detail
{
    using atomic_unsigned_int = unsigned int;

    template <class=void>
    struct BOOST_LEAF_SYMBOL_VISIBLE id_factory
    {
        static atomic_unsigned_int counter;
    };

    template <class T>
    atomic_unsigned_int id_factory<T>::counter = 1;

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

    BOOST_LEAF_ALWAYS_INLINE unsigned read_current_error_id() noexcept
    {
        return detail::current_error_id_storage<>::x;
    }

    template <class T>
    BOOST_LEAF_ALWAYS_INLINE void reserve_ptr()
    {
    }

    template <class T>
    BOOST_LEAF_ALWAYS_INLINE void write_ptr( T * p ) noexcept
    {
        detail::ptr<T>::p = p;
    }

    template <class T>
    BOOST_LEAF_ALWAYS_INLINE T * read_ptr() noexcept
    {
        return detail::ptr<T>::p;
    }
} // namespace tls

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_CONFIG_TLS_GLOBALS_HPP_INCLUDED
