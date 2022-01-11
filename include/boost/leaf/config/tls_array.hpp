#ifndef BOOST_LEAF_CONFIG_TLS_ARRAY_HPP_INCLUDED
#define BOOST_LEAF_CONFIG_TLS_ARRAY_HPP_INCLUDED

// Copyright 2018-2022 Emil Dotchevski and Reverge Studios, Inc.
// Copyright (c) 2022 Khalil Estell

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// LEAF requires thread local storage support for pointers and for uin32_t values.

// This header implements thread local storage for pointers and for uint32_t
// values for platforms that support thread local pointers by index.

////////////////////////////////////////

#ifndef BOOST_LEAF_TLS_INDEX_TYPE
#   define BOOST_LEAF_TLS_INDEX_TYPE int
#endif

#ifndef BOOST_LEAF_TLS_ARRAY_START_INDEX
#   define BOOST_LEAF_TLS_ARRAY_START_INDEX 0
#endif

static_assert(BOOST_LEAF_TLS_ARRAY_START_INDEX >= 0,
    "Bad BOOST_LEAF_TLS_ARRAY_START_INDEX");

#ifdef BOOST_LEAF_TLS_ARRAY_SIZE
static_assert(BOOST_LEAF_TLS_ARRAY_SIZE > 0,
    "Bad BOOST_LEAF_TLS_ARRAY_SIZE");
static_assert(BOOST_LEAF_TLS_ARRAY_START_INDEX < BOOST_LEAF_TLS_ARRAY_SIZE,
    "Bad BOOST_LEAF_TLS_ARRAY_START_INDEX");
#endif

#include <atomic>
#include <cstdint>

////////////////////////////////////////

namespace boost { namespace leaf {

namespace tls
{
    // The TLS support defined in this header requires the following two
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
    class BOOST_LEAF_SYMBOL_VISIBLE index_counter
    {
        static BOOST_LEAF_TLS_INDEX_TYPE c;

    public:

        template <class T>
        static BOOST_LEAF_TLS_INDEX_TYPE get_index() noexcept
        {
            BOOST_LEAF_TLS_INDEX_TYPE idx = c++;
#ifdef BOOST_LEAF_TLS_ARRAY_SIZE
            BOOST_LEAF_ASSERT(idx < BOOST_LEAF_TLS_ARRAY_SIZE);
#endif
            // Set breakpoint here to inspect TLS index assignment.
            return idx; 
        }
    };

    template <class T>
    BOOST_LEAF_TLS_INDEX_TYPE index_counter<T>::c = BOOST_LEAF_TLS_ARRAY_START_INDEX;

    template <class T>
    struct BOOST_LEAF_SYMBOL_VISIBLE index
    {
        static BOOST_LEAF_TLS_INDEX_TYPE const idx;
    };

    template <class T>
    BOOST_LEAF_TLS_INDEX_TYPE const index<T>::idx = index_counter<>::get_index<T>();

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
        static_assert(sizeof(std::intptr_t) >= sizeof(std::uint32_t), "Incompatible tls_array implementation");
        return (std::uint32_t) (std::intptr_t) (void *) read_ptr<Tag>();
    }

    template <class Tag>
    void write_uint32( std::uint32_t x ) noexcept
    {
        static_assert(sizeof(std::intptr_t) >= sizeof(std::uint32_t), "Incompatible tls_array implementation");
        write_ptr<Tag>((Tag *) (void *) (std::intptr_t) x);
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

#endif
