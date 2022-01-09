#ifndef BOOST_LEAF_CONFIG_TLS_FREERTOS_HPP_INCLUDED
#define BOOST_LEAF_CONFIG_TLS_FREERTOS_HPP_INCLUDED

/// Copyright 2018-2022 Emil Dotchevski and Reverge Studios, Inc.
/// Copyright (c) 2022 Khalil Estell

/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// LEAF requires thread local storage support for pointers and for uin32_t values.

// This header implements thread local storage for pointers and for uint32_t
// values via the FreeRTOS API, as an alternative to the default implementation
// (found in tls_cpp11.hpp) which relies on C++11 thread_local support.

// See https://www.freertos.org/thread-local-storage-pointers.html.

#ifndef BOOST_LEAF_ENABLE_WARNINGS ///
#   if defined(_MSC_VER) ///
#       pragma warning(push,1) ///
#   elif defined(__clang__) ///
#       pragma clang system_header ///
#   elif (__GNUC__*100+__GNUC_MINOR__>301) ///
#       pragma GCC system_header ///
#   endif ///
#endif ///

#include <task.h>

#ifndef BOOST_LEAF_TLS_ARRAY
#   define BOOST_LEAF_TLS_ARRAY
#endif

#ifndef BOOST_LEAF_TLS_INDEX_TYPE
#   define BOOST_LEAF_TLS_INDEX_TYPE BaseType_t
#endif

#ifndef BOOST_LEAF_TLS_ARRAY_SIZE
#   define BOOST_LEAF_TLS_ARRAY_SIZE configNUM_THREAD_LOCAL_STORAGE_POINTERS
#endif

static_assert(
    BOOST_LEAF_TLS_ARRAY_SIZE <= configNUM_THREAD_LOCAL_STORAGE_POINTERS,
        "Bad BOOST_LEAF_TLS_ARRAY_SIZE");

namespace boost { namespace leaf {

namespace tls
{
    inline void * read_void_ptr( int tls_index ) noexcept
    {
        return pvTaskGetThreadLocalStoragePointer(0, tls_index);
    }

    inline void write_void_ptr( int tls_index, void * p ) noexcept
    {
        vTaskSetThreadLocalStoragePointer(0, tls_index, p);
    }
}

} }

#if defined(_MSC_VER) && !defined(BOOST_LEAF_ENABLE_WARNINGS) ///
#pragma warning(pop) ///
#endif ///

#endif
