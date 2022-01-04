#ifndef BOOST_LEAF_TLS_FREERTOS_HPP_INCLUDED
#define BOOST_LEAF_TLS_FREERTOS_HPP_INCLUDED

/// Copyright (c) 2018-2021 Emil Dotchevski and Reverge Studios, Inc.

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

#include <task.h> // From FreeRTOS
#define BOOST_LEAF_TLS_INDEX_TYPE BaseType_t
#include <boost/leaf/detail/tls_index.hpp>
#undef BOOST_LEAF_TLS_INDEX_TYPE
#include <atomic>
#include <cstdint>

namespace boost { namespace leaf {

namespace leaf_detail
{
    using atomic_unsigned_int = std::atomic<unsigned int>;
}

namespace tls
{
    template <class T>
    T * ptr_read() noexcept
    {
        return reinterpret_cast<T *>(pvTaskGetThreadLocalStoragePointer(0, index<T>::idx));
    }

    template <class T>
    void ptr_write( T * p ) noexcept
    {
        vTaskSetThreadLocalStoragePointer(0, index<T>::idx, p);
    }

    ////////////////////////////////////////

    template <class Tag>
    std::uint32_t uint32_read() noexcept
    {
        static_assert(sizeof(Tag *) == sizeof(std::uint32_t), "LEAF FreeRTOS support requires 32-bit pointers");
        Tag * p = ptr_read<Tag>();
        return (std::uint32_t)p;
    }

    template <class Tag>
    void uint32_write( std::uint32_t x ) noexcept
    {
        static_assert(sizeof(Tag *) == sizeof(std::uint32_t), "LEAF FreeRTOS support requires 32-bit pointers");
        ptr_write<Tag>((Tag *)x);
    }

    template <class Tag>
    void uint32_increment() noexcept
    {
        uint32_write<Tag>(uint32_read<Tag>() + 1);
    }

    template <class Tag>
    void uint32_decrement() noexcept
    {
        uint32_write<Tag>(uint32_read<Tag>() - 1);
    }
}

} }

#if defined(_MSC_VER) && !defined(BOOST_LEAF_ENABLE_WARNINGS) ///
#pragma warning(pop) ///
#endif ///

#endif
