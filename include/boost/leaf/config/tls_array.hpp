#ifndef BOOST_LEAF_CONFIG_TLS_ARRAY_HPP_INCLUDED
#define BOOST_LEAF_CONFIG_TLS_ARRAY_HPP_INCLUDED

/// Copyright (c) 2018-2021 Emil Dotchevski and Reverge Studios, Inc.

/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// LEAF requires thread local storage support for pointers and for uin32_t values.

// This header implements thread local storage for pointers and for uint32_t
// values for platforms that support thread local pointers by index.

#ifndef BOOST_LEAF_ENABLE_WARNINGS ///
#   if defined(_MSC_VER) ///
#       pragma warning(push,1) ///
#   elif defined(__clang__) ///
#       pragma clang system_header ///
#   elif (__GNUC__*100+__GNUC_MINOR__>301) ///
#       pragma GCC system_header ///
#   endif ///
#endif ///

#ifndef BOOST_LEAF_TLS_INDEX_TYPE
#   define BOOST_LEAF_TLS_INDEX_TYPE int
#endif

#ifndef BOOST_LEAF_TLS_ARRAY_START_INDEX
#   define BOOST_LEAF_TLS_ARRAY_START_INDEX 0
#endif

#include <atomic>
#include <cstdint>

namespace boost { namespace leaf {

namespace tls
{
    // The TLS implementation defined in this header requires the following two
    // functions to be defined elsewhere:
    void * read_void_ptr( int tls_index ) noexcept;
    void write_void_ptr( int tls_index, void * ) noexcept;
}

} }

////////////////////////////////////////

namespace boost { namespace leaf {

namespace leaf_detail
{
    using atomic_unsigned_int = std::atomic<unsigned int>;
}

namespace tls
{
    template <class=void>
    struct BOOST_LEAF_SYMBOL_VISIBLE index_counter
    {
        static BOOST_LEAF_TLS_INDEX_TYPE c;
    };

    template <class T>
    BOOST_LEAF_TLS_INDEX_TYPE index_counter<T>::c = BOOST_LEAF_TLS_ARRAY_START_INDEX;

#if 0
    template <class T>
    int get_counter()
    {
        int c = index_counter<>::c++;
        return c;
    }
#endif

    template <class T>
    struct BOOST_LEAF_SYMBOL_VISIBLE index
    {
        inline static BOOST_LEAF_TLS_INDEX_TYPE const idx = index_counter<>::c++; // get_counter<T>();
    };

    ////////////////////////////////////////

    template <class T>
    T * read_ptr() noexcept
    {
        int const tls_index = index<T>::idx;
        BOOST_LEAF_ASSERT(tls_index >= BOOST_LEAF_TLS_ARRAY_START_INDEX);
#ifdef BOOST_LEAF_TLS_ARRAY_SIZE
        BOOST_LEAF_ASSERT(tls_index < BOOST_LEAF_TLS_ARRAY_SIZE);
#endif
        return reinterpret_cast<T *>(read_void_ptr(tls_index));
    }

    template <class T>
    void write_ptr( T * p ) noexcept
    {
        int const tls_index = index<T>::idx;
        BOOST_LEAF_ASSERT(tls_index >= BOOST_LEAF_TLS_ARRAY_START_INDEX);
#ifdef BOOST_LEAF_TLS_ARRAY_SIZE
        BOOST_LEAF_ASSERT(tls_index < BOOST_LEAF_TLS_ARRAY_SIZE);
#endif
        write_void_ptr(tls_index, p);
        BOOST_LEAF_ASSERT(read_void_ptr(tls_index) == p);
    }

    ////////////////////////////////////////

    template <class Tag>
    std::uint32_t read_uint32() noexcept
    {
        Tag * p = read_ptr<Tag>();
        return (std::intptr_t)(void *)p;
    }

    template <class Tag>
    void write_uint32( std::uint32_t x ) noexcept
    {
        write_ptr<Tag>((Tag *)x);
    }

    template <class Tag>
    void uint32_increment() noexcept
    {
        write_uint32<Tag>(read_uint32<Tag>() + 1);
    }

    template <class Tag>
    void uint32_decrement() noexcept
    {
        write_uint32<Tag>(read_uint32<Tag>() - 1);
    }
}

} }

#if defined(_MSC_VER) && !defined(BOOST_LEAF_ENABLE_WARNINGS) ///
#pragma warning(pop) ///
#endif ///

#endif
