#ifndef BOOST_LEAF_CONFIG_TLS_GLOBALS_HPP_INCLUDED
#define BOOST_LEAF_CONFIG_TLS_GLOBALS_HPP_INCLUDED

/// Copyright (c) 2018-2021 Emil Dotchevski and Reverge Studios, Inc.

/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// LEAF requires thread local storage support for pointers and for uin32_t values.

// This header implements "thread local" storage for pointers and for uint32_t
// values using globals, which is suitable for single thread environments.

#ifndef BOOST_LEAF_ENABLE_WARNINGS ///
#   if defined(_MSC_VER) ///
#       pragma warning(push,1) ///
#   elif defined(__clang__) ///
#       pragma clang system_header ///
#   elif (__GNUC__*100+__GNUC_MINOR__>301) ///
#       pragma GCC system_header ///
#   endif ///
#endif ///

#include <cstdint>

namespace boost { namespace leaf {

namespace leaf_detail
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
    void write_ptr( T * p ) noexcept
    {
        ptr<T>::p = p;
    }

    ////////////////////////////////////////

    template <class Tag>
    struct BOOST_LEAF_SYMBOL_VISIBLE tagged_uint32
    {
        static std::uint32_t x;
    };

    template <class Tag>
    std::uint32_t tagged_uint32<Tag>::x;

    template <class Tag>
    std::uint32_t read_uint32() noexcept
    {
        return tagged_uint32<Tag>::x;
    }

    template <class Tag>
    void write_uint32( std::uint32_t x ) noexcept
    {
        tagged_uint32<Tag>::x = x;
    }

    template <class Tag>
    void uint32_increment() noexcept
    {
        ++tagged_uint32<Tag>::x;
    }

    template <class Tag>
    void uint32_decrement() noexcept
    {
        --tagged_uint32<Tag>::x;
    }
}

} }

#if defined(_MSC_VER) && !defined(BOOST_LEAF_ENABLE_WARNINGS) ///
#pragma warning(pop) ///
#endif ///

#endif
