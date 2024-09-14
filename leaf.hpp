#ifndef BOOST_LEAF_HPP_INCLUDED
#define BOOST_LEAF_HPP_INCLUDED

// Boost LEAF single header distribution. Do not edit.
// Generated on Sep 14, 2024 from https://github.com/boostorg/leaf/tree/5991052.

// Latest published version of this file: https://raw.githubusercontent.com/boostorg/leaf/gh-pages/leaf.hpp.

// Copyright 2018-2024 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// >>> #include <boost/leaf/config.hpp>
#ifndef BOOST_LEAF_CONFIG_HPP_INCLUDED
#define BOOST_LEAF_CONFIG_HPP_INCLUDED

// #line 8 "boost/leaf/config.hpp"
#ifdef BOOST_LEAF_TLS_FREERTOS
#   ifndef BOOST_LEAF_EMBEDDED
#       define BOOST_LEAF_EMBEDDED
#   endif
#endif

#ifdef BOOST_LEAF_EMBEDDED
#   ifndef BOOST_LEAF_CFG_DIAGNOSTICS
#       define BOOST_LEAF_CFG_DIAGNOSTICS 0
#   endif
#   ifndef BOOST_LEAF_CFG_STD_SYSTEM_ERROR
#       define BOOST_LEAF_CFG_STD_SYSTEM_ERROR 0
#   endif
#   ifndef BOOST_LEAF_CFG_STD_STRING
#       define BOOST_LEAF_CFG_STD_STRING 0
#   endif
#   ifndef BOOST_LEAF_CFG_CAPTURE
#       define BOOST_LEAF_CFG_CAPTURE 0
#   endif
#endif

////////////////////////////////////////

#ifndef BOOST_LEAF_ASSERT
#   include <cassert>
#   define BOOST_LEAF_ASSERT assert
#endif

////////////////////////////////////////

#ifndef BOOST_LEAF_CFG_DIAGNOSTICS
#   define BOOST_LEAF_CFG_DIAGNOSTICS 1
#endif

#ifndef BOOST_LEAF_CFG_STD_SYSTEM_ERROR
#   define BOOST_LEAF_CFG_STD_SYSTEM_ERROR 1
#endif

#ifndef BOOST_LEAF_CFG_STD_STRING
#   define BOOST_LEAF_CFG_STD_STRING 1
#endif

#ifndef BOOST_LEAF_CFG_CAPTURE
#   define BOOST_LEAF_CFG_CAPTURE 1
#endif

#ifndef BOOST_LEAF_CFG_WIN32
#   define BOOST_LEAF_CFG_WIN32 0
#endif

#ifndef BOOST_LEAF_CFG_GNUC_STMTEXPR
#   ifdef __GNUC__
#   	define BOOST_LEAF_CFG_GNUC_STMTEXPR 1
#   else
#   	define BOOST_LEAF_CFG_GNUC_STMTEXPR 0
#   endif
#endif

#ifndef BOOST_LEAF_CFG_DIAGNOSTICS_FIRST_DELIMITER
#   define BOOST_LEAF_CFG_DIAGNOSTICS_FIRST_DELIMITER "\n    "
#endif

#ifndef BOOST_LEAF_CFG_DIAGNOSTICS_DELIMITER
#   define BOOST_LEAF_CFG_DIAGNOSTICS_DELIMITER "\n    "
#endif

#if BOOST_LEAF_CFG_DIAGNOSTICS != 0 && BOOST_LEAF_CFG_DIAGNOSTICS != 1
#   error BOOST_LEAF_CFG_DIAGNOSTICS must be 0 or 1.
#endif

#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR != 0 && BOOST_LEAF_CFG_STD_SYSTEM_ERROR != 1
#   error BOOST_LEAF_CFG_STD_SYSTEM_ERROR must be 0 or 1.
#endif

#if BOOST_LEAF_CFG_STD_STRING != 0 && BOOST_LEAF_CFG_STD_STRING != 1
#   error BOOST_LEAF_CFG_STD_STRING must be 0 or 1.
#endif

#if BOOST_LEAF_CFG_CAPTURE != 0 && BOOST_LEAF_CFG_CAPTURE != 1
#   error BOOST_LEAF_CFG_CAPTURE must be 0 or 1.
#endif

#if BOOST_LEAF_CFG_WIN32 != 0 && BOOST_LEAF_CFG_WIN32 != 1
#   error BOOST_LEAF_CFG_WIN32 must be 0 or 1.
#endif

#if BOOST_LEAF_CFG_GNUC_STMTEXPR != 0 && BOOST_LEAF_CFG_GNUC_STMTEXPR != 1
#   error BOOST_LEAF_CFG_GNUC_STMTEXPR must be 0 or 1.
#endif

#if BOOST_LEAF_CFG_DIAGNOSTICS && !BOOST_LEAF_CFG_STD_STRING
#   error BOOST_LEAF_CFG_DIAGNOSTICS requires BOOST_LEAF_CFG_STD_STRING, which has been disabled.
#endif

#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR && !BOOST_LEAF_CFG_STD_STRING
#   error BOOST_LEAF_CFG_STD_SYSTEM_ERROR requires BOOST_LEAF_CFG_STD_STRING, which has been disabled.
#endif

////////////////////////////////////////

#ifndef BOOST_LEAF_PRETTY_FUNCTION
#   if defined(_MSC_VER) && !defined(__clang__) && !defined(__GNUC__)
#       define BOOST_LEAF_PRETTY_FUNCTION __FUNCSIG__
#   else
#       define BOOST_LEAF_PRETTY_FUNCTION __PRETTY_FUNCTION__
#   endif
#endif

////////////////////////////////////////

#ifndef BOOST_LEAF_NO_EXCEPTIONS
// The following is based in part on Boost Config.
// (C) Copyright John Maddock 2001 - 2003.
// (C) Copyright Martin Wille 2003.
// (C) Copyright Guillaume Melquiond 2003.
#   if defined(__clang__) && !defined(__ibmxl__)
//  Clang C++ emulates GCC, so it has to appear early.
#       if !__has_feature(cxx_exceptions)
#           define BOOST_LEAF_NO_EXCEPTIONS
#       endif
#   elif defined(__GNUC__) && !defined(__ibmxl__)
//  GNU C++:
#       if !defined(__EXCEPTIONS)
#           define BOOST_LEAF_NO_EXCEPTIONS
#       endif
#   elif defined(__CODEGEARC__)
//  CodeGear - must be checked for before Borland
#       if !defined(_CPPUNWIND) && !defined(__EXCEPTIONS)
#           define BOOST_LEAF_NO_EXCEPTIONS
#       endif
#   elif defined(__IBMCPP__) && defined(__COMPILER_VER__) && defined(__MVS__)
//  IBM z/OS XL C/C++
#       if !defined(_CPPUNWIND) && !defined(__EXCEPTIONS)
#           define BOOST_LEAF_NO_EXCEPTIONS
#       endif
#   elif defined(__ibmxl__)
//  IBM XL C/C++ for Linux (Little Endian)
#       if !__has_feature(cxx_exceptions)
#           define BOOST_LEAF_NO_EXCEPTIONS
#       endif
#   elif defined(_MSC_VER)
//  Microsoft Visual C++
//  Must remain the last #elif since some other vendors (Metrowerks, for
//  example) also #define _MSC_VER
#       if !_CPPUNWIND
#           define BOOST_LEAF_NO_EXCEPTIONS
#       endif
#   endif

#endif

////////////////////////////////////////

#ifdef _MSC_VER
#   define BOOST_LEAF_ALWAYS_INLINE __forceinline
#else
#   define BOOST_LEAF_ALWAYS_INLINE __attribute__((always_inline)) inline
#endif

////////////////////////////////////////

#if defined(__has_attribute) && defined(__SUNPRO_CC) && (__SUNPRO_CC > 0x5130)
#   if __has_attribute(nodiscard)
#       define BOOST_LEAF_ATTRIBUTE_NODISCARD [[nodiscard]]
#   endif
#elif defined(__has_cpp_attribute)
// require c++17 regardless of compiler
#   if __has_cpp_attribute(nodiscard) && __cplusplus >= 201703L
#       define BOOST_LEAF_ATTRIBUTE_NODISCARD [[nodiscard]]
#   endif
#endif
#ifndef BOOST_LEAF_ATTRIBUTE_NODISCARD
#   define BOOST_LEAF_ATTRIBUTE_NODISCARD
#endif

////////////////////////////////////////

#ifndef BOOST_LEAF_CONSTEXPR
#   if __cplusplus > 201402L
#       define BOOST_LEAF_CONSTEXPR constexpr
#   else
#       define BOOST_LEAF_CONSTEXPR
#   endif
#endif

////////////////////////////////////////

#ifndef BOOST_LEAF_DEPRECATED
#   if __cplusplus > 201402L
#       define BOOST_LEAF_DEPRECATED(msg) [[deprecated(msg)]]
#   else
#       define BOOST_LEAF_DEPRECATED(msg)
#   endif
#endif

////////////////////////////////////////

#ifndef BOOST_LEAF_NO_EXCEPTIONS
#   include <exception>
#   if (defined(__cpp_lib_uncaught_exceptions) && __cpp_lib_uncaught_exceptions >= 201411L) || (defined(_MSC_VER) && _MSC_VER >= 1900)
#       define BOOST_LEAF_STD_UNCAUGHT_EXCEPTIONS 1
#   else
#       define BOOST_LEAF_STD_UNCAUGHT_EXCEPTIONS 0
#   endif
#endif

////////////////////////////////////////

#ifdef __GNUC__
#   define BOOST_LEAF_SYMBOL_VISIBLE [[gnu::visibility("default")]]
#else
#   define BOOST_LEAF_SYMBOL_VISIBLE
#endif

////////////////////////////////////////

#if defined(__GNUC__) && !(defined(__clang__) || defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC) || defined(__ECC)) && (__GNUC__ * 100 + __GNUC_MINOR__) < 409
#   ifndef BOOST_LEAF_NO_CXX11_REF_QUALIFIERS
#       define BOOST_LEAF_NO_CXX11_REF_QUALIFIERS
#   endif
#endif

////////////////////////////////////////

// Configure TLS access
// >>> #include <boost/leaf/config/tls.hpp>
#ifndef BOOST_LEAF_CONFIG_TLS_HPP_INCLUDED
#define BOOST_LEAF_CONFIG_TLS_HPP_INCLUDED

// #line 8 "boost/leaf/config/tls.hpp"
#if defined(BOOST_LEAF_TLS_FREERTOS)
// >>> #   include <boost/leaf/config/tls_freertos.hpp>
#ifndef BOOST_LEAF_CONFIG_TLS_FREERTOS_HPP_INCLUDED
#define BOOST_LEAF_CONFIG_TLS_FREERTOS_HPP_INCLUDED

// #line 8 "boost/leaf/config/tls_freertos.hpp"
// Copyright (c) 2022 Khalil Estell

// LEAF requires thread local storage support for pointers and for uin32_t values.

// This header implements "thread local" storage via FreeTOS functions
// pvTaskGetThreadLocalStoragePointer / pvTaskSetThreadLocalStoragePointer

#include <task.h>

#ifndef BOOST_LEAF_USE_TLS_ARRAY
#   define BOOST_LEAF_USE_TLS_ARRAY
#endif

#ifndef BOOST_LEAF_CFG_TLS_ARRAY_SIZE
#   define BOOST_LEAF_CFG_TLS_ARRAY_SIZE configNUM_THREAD_LOCAL_STORAGE_POINTERS
#endif

static_assert((BOOST_LEAF_CFG_TLS_ARRAY_SIZE) <= configNUM_THREAD_LOCAL_STORAGE_POINTERS,
    "Bad BOOST_LEAF_CFG_TLS_ARRAY_SIZE");

namespace boost { namespace leaf {

namespace tls
{
    // See https://www.freertos.org/thread-local-storage-pointers.html.

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

#endif // BOOST_LEAF_CONFIG_TLS_FREERTOS_HPP_INCLUDED
// <<< #   include <boost/leaf/config/tls_freertos.hpp>
// #line 10 "boost/leaf/config/tls.hpp"
#endif

#ifndef BOOST_LEAF_USE_TLS_ARRAY
#	ifdef BOOST_LEAF_CFG_TLS_INDEX_TYPE
#		warning "BOOST_LEAF_CFG_TLS_INDEX_TYPE" is ignored if BOOST_LEAF_USE_TLS_ARRAY is not defined.
#	endif
#	ifdef BOOST_LEAF_CFG_TLS_ARRAY_SIZE
#		warning "BOOST_LEAF_CFG_TLS_ARRAY_SIZE" is ignored if BOOST_LEAF_USE_TLS_ARRAY is not defined.
#	endif
#	ifdef BOOST_LEAF_CFG_TLS_ARRAY_START_INDEX
#		warning "BOOST_LEAF_CFG_TLS_ARRAY_START_INDEX" is ignored if BOOST_LEAF_USE_TLS_ARRAY is not defined.
#	endif
#endif

#if defined BOOST_LEAF_USE_TLS_ARRAY
// >>> #   include <boost/leaf/config/tls_array.hpp>
#ifndef BOOST_LEAF_CONFIG_TLS_ARRAY_HPP_INCLUDED
#define BOOST_LEAF_CONFIG_TLS_ARRAY_HPP_INCLUDED

// #line 8 "boost/leaf/config/tls_array.hpp"
// Copyright (c) 2022 Khalil Estell

// LEAF requires thread local storage support for pointers and for uin32_t values.

// This header implements thread local storage for pointers and for unsigned int
// values for platforms that support thread local pointers by index.

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

#include <limits>
#include <atomic>
#include <cstdint>
#include <type_traits>

#ifndef BOOST_LEAF_CFG_TLS_INDEX_TYPE
#   define BOOST_LEAF_CFG_TLS_INDEX_TYPE unsigned char
#endif

#ifndef BOOST_LEAF_CFG_TLS_ARRAY_START_INDEX
#   define BOOST_LEAF_CFG_TLS_ARRAY_START_INDEX 0
#endif

static_assert((BOOST_LEAF_CFG_TLS_ARRAY_START_INDEX) >= 0,
    "Bad BOOST_LEAF_CFG_TLS_ARRAY_START_INDEX");

#ifdef BOOST_LEAF_CFG_TLS_ARRAY_SIZE
    static_assert((BOOST_LEAF_CFG_TLS_ARRAY_SIZE) > (BOOST_LEAF_CFG_TLS_ARRAY_START_INDEX),
        "Bad BOOST_LEAF_CFG_TLS_ARRAY_SIZE");
    static_assert((BOOST_LEAF_CFG_TLS_ARRAY_SIZE) - 1 <= std::numeric_limits<BOOST_LEAF_CFG_TLS_INDEX_TYPE>::max(),
        "Bad BOOST_LEAF_CFG_TLS_ARRAY_SIZE");
#endif

////////////////////////////////////////

namespace boost { namespace leaf {

namespace detail
{
    using atomic_unsigned_int = std::atomic<unsigned int>;
}

namespace tls
{
    template <class=void>
    class BOOST_LEAF_SYMBOL_VISIBLE index_counter
    {
        static int c_;

        static BOOST_LEAF_CFG_TLS_INDEX_TYPE next_() noexcept
        {
            int idx = ++c_;
            BOOST_LEAF_ASSERT(idx > (BOOST_LEAF_CFG_TLS_ARRAY_START_INDEX));
            BOOST_LEAF_ASSERT(idx < (BOOST_LEAF_CFG_TLS_ARRAY_SIZE));
            return idx;
        }

    public:

        template <class T>
        static BOOST_LEAF_CFG_TLS_INDEX_TYPE next() noexcept
        {
            return next_(); // Set breakpoint here to monitor TLS index allocation for T.
        }
    };

    template <class T>
    struct BOOST_LEAF_SYMBOL_VISIBLE tls_index
    {
        static BOOST_LEAF_CFG_TLS_INDEX_TYPE idx;
    };

    template <class T>
    struct BOOST_LEAF_SYMBOL_VISIBLE alloc_tls_index
    {
        static BOOST_LEAF_CFG_TLS_INDEX_TYPE const idx;
    };

    template <class T>
    int index_counter<T>::c_ = BOOST_LEAF_CFG_TLS_ARRAY_START_INDEX;

    template <class T>
    BOOST_LEAF_CFG_TLS_INDEX_TYPE tls_index<T>::idx = BOOST_LEAF_CFG_TLS_ARRAY_START_INDEX;

    template <class T>
    BOOST_LEAF_CFG_TLS_INDEX_TYPE const alloc_tls_index<T>::idx = tls_index<T>::idx = index_counter<>::next<T>();

    ////////////////////////////////////////

    template <class T>
    T * read_ptr() noexcept
    {
        int tls_idx = tls_index<T>::idx;
        if( tls_idx == (BOOST_LEAF_CFG_TLS_ARRAY_START_INDEX) )
            return nullptr;
        --tls_idx;
        return reinterpret_cast<T *>(read_void_ptr(tls_idx));
    }

    template <class T>
    void write_ptr( T * p ) noexcept
    {
        int tls_idx = alloc_tls_index<T>::idx;
        --tls_idx;
        write_void_ptr(tls_idx, p);
        BOOST_LEAF_ASSERT(read_void_ptr(tls_idx) == p);
    }

    ////////////////////////////////////////

    template <class Tag>
    unsigned read_uint() noexcept
    {
        static_assert(sizeof(std::intptr_t) >= sizeof(unsigned), "Incompatible tls_array implementation");
        return (unsigned) (std::intptr_t) (void *) read_ptr<Tag>();
    }

    template <class Tag>
    void write_uint( unsigned x ) noexcept
    {
        static_assert(sizeof(std::intptr_t) >= sizeof(unsigned), "Incompatible tls_array implementation");
        write_ptr<Tag>((Tag *) (void *) (std::intptr_t) x);
    }
}

} }

#endif // BOOST_LEAF_CONFIG_TLS_ARRAY_HPP_INCLUDED
// <<< #   include <boost/leaf/config/tls_array.hpp>
// #line 26 "boost/leaf/config/tls.hpp"
#elif defined(BOOST_LEAF_NO_THREADS)
// >>> #   include <boost/leaf/config/tls_globals.hpp>
#ifndef BOOST_LEAF_CONFIG_TLS_GLOBALS_HPP_INCLUDED
#define BOOST_LEAF_CONFIG_TLS_GLOBALS_HPP_INCLUDED

// #line 8 "boost/leaf/config/tls_globals.hpp"
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
    void write_ptr( T * p ) noexcept
    {
        ptr<T>::p = p;
    }

    ////////////////////////////////////////

    template <class Tag>
    struct BOOST_LEAF_SYMBOL_VISIBLE tagged_uint
    {
        static unsigned x;
    };

    template <class Tag>
    unsigned tagged_uint<Tag>::x;

    template <class Tag>
    unsigned read_uint() noexcept
    {
        return tagged_uint<Tag>::x;
    }

    template <class Tag>
    void write_uint( unsigned x ) noexcept
    {
        tagged_uint<Tag>::x = x;
    }
}

} }

#endif // BOOST_LEAF_CONFIG_TLS_GLOBALS_HPP_INCLUDED
// <<< #   include <boost/leaf/config/tls_globals.hpp>
// #line 28 "boost/leaf/config/tls.hpp"
#else
// >>> #   include <boost/leaf/config/tls_cpp11.hpp>
#ifndef BOOST_LEAF_CONFIG_TLS_CPP11_HPP_INCLUDED
#define BOOST_LEAF_CONFIG_TLS_CPP11_HPP_INCLUDED

// #line 8 "boost/leaf/config/tls_cpp11.hpp"
// LEAF requires thread local storage support for pointers and for uin32_t values.

// This header implements thread local storage for pointers and for unsigned int
// values using the C++11 built-in thread_local storage class specifier.

#include <cstdint>
#include <atomic>

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
    void write_ptr( T * p ) noexcept
    {
        ptr<T>::p = p;
    }

    ////////////////////////////////////////

    template <class Tag>
    struct BOOST_LEAF_SYMBOL_VISIBLE tagged_uint
    {
        static thread_local unsigned x;
    };

    template <class Tag>
    thread_local unsigned tagged_uint<Tag>::x;

    template <class Tag>
    unsigned read_uint() noexcept
    {
        return tagged_uint<Tag>::x;
    }

    template <class Tag>
    void write_uint( unsigned x ) noexcept
    {
        tagged_uint<Tag>::x = x;
    }
}

} }

#endif // BOOST_LEAF_CONFIG_TLS_CPP11_HPP_INCLUDED
// <<< #   include <boost/leaf/config/tls_cpp11.hpp>
// #line 30 "boost/leaf/config/tls.hpp"
#endif

#endif // BOOST_LEAF_CONFIG_TLS_HPP_INCLUDED
// <<< #include <boost/leaf/config/tls.hpp>
// #line 234 "boost/leaf/config.hpp"

#endif // BOOST_LEAF_CONFIG_HPP_INCLUDED
// >>> #include <boost/leaf/common.hpp>
#ifndef BOOST_LEAF_COMMON_HPP_INCLUDED
#define BOOST_LEAF_COMMON_HPP_INCLUDED

// #line 8 "boost/leaf/common.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 14
// >>> #include <boost/leaf/detail/demangle.hpp>
#ifndef BOOST_LEAF_DETAIL_DEMANGLE_HPP_INCLUDED
#define BOOST_LEAF_DETAIL_DEMANGLE_HPP_INCLUDED

// #line 8 "boost/leaf/detail/demangle.hpp"
// This file is based on boost::core::demangle
//
// Copyright 2014 Peter Dimov
// Copyright 2014 Andrey Semashev
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt

// #include <boost/leaf/config.hpp> // Expanded at line 14
#include <iosfwd>
#include <cstdlib>

#if BOOST_LEAF_CFG_DIAGNOSTICS

// __has_include is currently supported by GCC and Clang. However GCC 4.9 may have issues and
// returns 1 for 'defined( __has_include )', while '__has_include' is actually not supported:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=63662
#if defined(__has_include) && (!defined(__GNUC__) || defined(__clang__) || (__GNUC__ + 0) >= 5)
#   if __has_include(<cxxabi.h>)
#       define BOOST_LEAF_HAS_CXXABI_H
#   endif
#elif defined(__GLIBCXX__) || defined(__GLIBCPP__)
#   define BOOST_LEAF_HAS_CXXABI_H
#endif

#if defined(BOOST_LEAF_HAS_CXXABI_H)
#   include <cxxabi.h>
//  For some archtectures (mips, mips64, x86, x86_64) cxxabi.h in Android NDK is implemented by gabi++ library
//  (https://android.googlesource.com/platform/ndk/+/master/sources/cxx-stl/gabi++/), which does not implement
//  abi::__cxa_demangle(). We detect this implementation by checking the include guard here.
#   if defined(__GABIXX_CXXABI_H__)
#       undef BOOST_LEAF_HAS_CXXABI_H
#   endif
#endif

#endif

namespace boost { namespace leaf {

namespace detail
{
    // The functions below are C++11 constexpr, but we use BOOST_LEAF_ALWAYS_INLINE to control object file
    // section count / template bleat. Evidently this makes a difference on gcc / windows at least.

    template <int S1, int S2, int I, bool = S1 >= S2>
    struct cpp11_prefix
    {
        BOOST_LEAF_ALWAYS_INLINE constexpr static bool check(char const (&)[S1], char const (&)[S2]) noexcept
        {
            return false;
        }
    };
    template <int S1, int S2, int I>
    struct cpp11_prefix<S1, S2, I, true>
    {
        BOOST_LEAF_ALWAYS_INLINE constexpr static bool check(char const (&str)[S1], char const (&prefix)[S2]) noexcept
        {
            return str[I] == prefix[I] && cpp11_prefix<S1, S2, I - 1>::check(str, prefix);
        }
    };
    template <int S1, int S2>
    struct cpp11_prefix<S1, S2, 0, true>
    {
        BOOST_LEAF_ALWAYS_INLINE constexpr static bool check(char const (&str)[S1], char const (&prefix)[S2]) noexcept
        {
            return str[0] == prefix[0];
        }
    };
    template <int S1, int S2>
    BOOST_LEAF_ALWAYS_INLINE constexpr int check_prefix(char const (&str)[S1], char const (&prefix)[S2]) noexcept
    {
        return cpp11_prefix<S1, S2, S2 - 2>::check(str, prefix) ? S2 - 1 : 0;
    }

    ////////////////////////////////////////

    template <int S1, int S2, int I1, int I2, bool = S1 >= S2>
    struct cpp11_suffix
    {
        BOOST_LEAF_ALWAYS_INLINE constexpr static bool check(char const (&)[S1], char const (&)[S2]) noexcept
        {
            return false;
        }
    };
    template <int S1, int S2, int I1, int I2>
    struct cpp11_suffix<S1, S2, I1, I2, true>
    {
        BOOST_LEAF_ALWAYS_INLINE constexpr static bool check(char const (&str)[S1], char const (&suffix)[S2]) noexcept
        {
            return str[I1] == suffix[I2] && cpp11_suffix<S1, S2, I1 - 1, I2 - 1>::check(str, suffix);
        }
    };
    template <int S1, int S2, int I1>
    struct cpp11_suffix<S1, S2, I1, 0, true>
    {
        BOOST_LEAF_ALWAYS_INLINE constexpr static bool check(char const (&str)[S1], char const (&suffix)[S2]) noexcept
        {
            return str[I1] == suffix[0];
        }
    };
    template <int S1, int S2>
    BOOST_LEAF_ALWAYS_INLINE constexpr int check_suffix(char const (&str)[S1], char const (&suffix)[S2]) noexcept
    {
        return cpp11_suffix<S1, S2, S1 - 2, S2 - 2>::check(str, suffix) ? S1 - S2 : 0;
    }
}

namespace n
{
    struct r
    {
        char const * name;
        int len;
        r(char const * name, int len) noexcept:
            name(name),
            len(len)
        {
        }
        template <class CharT, class Traits>
        friend std::ostream & operator<<(std::basic_ostream<CharT, Traits> & os, r const & pn)
        {
            return os.write(pn.name, pn.len);
        }
    };

    template <class T>
    BOOST_LEAF_ALWAYS_INLINE r p()
    {
        // C++11 compile-time parsing of __PRETTY_FUNCTION__/__FUNCSIG__. The sizeof hacks are a
        // workaround for older GCC versions, where __PRETTY_FUNCTION__ is not constexpr, which triggers
        // compile errors when used in constexpr expressinos, yet evaluating a sizeof exrpession works.

        // We don't try to recognize the compiler based on compiler-specific macros. Any compiler/version
        // is supported as long as it uses one of the formats we recognize.

        // Unrecognized __PRETTY_FUNCTION__/__FUNCSIG__ formats will result in compiler diagnostics.
        // In that case, please file an issue on https://github.com/boostorg/leaf.

#define BOOST_LEAF_P(P) (sizeof(char[1 + detail::check_prefix(BOOST_LEAF_PRETTY_FUNCTION, P)]) - 1)
        // clang style:
        int const p01 = BOOST_LEAF_P("r boost::leaf::n::p() [T = ");
        // old clang style:
        int const p02 = BOOST_LEAF_P("boost::leaf::n::r boost::leaf::n::p() [T = ");
        // gcc style:
        int const p03 = BOOST_LEAF_P("boost::leaf::n::r boost::leaf::n::p() [with T = ");
        // msvc style, struct:
        int const p04 = BOOST_LEAF_P("struct boost::leaf::n::r __cdecl boost::leaf::n::p<struct ");
        int const p05 = BOOST_LEAF_P("struct boost::leaf::n::r __stdcall boost::leaf::n::p<struct ");
        int const p06 = BOOST_LEAF_P("struct boost::leaf::n::r __fastcall boost::leaf::n::p<struct ");
        // msvc style, class:
        int const p07 = BOOST_LEAF_P("struct boost::leaf::n::r __cdecl boost::leaf::n::p<class ");
        int const p08 = BOOST_LEAF_P("struct boost::leaf::n::r __stdcall boost::leaf::n::p<class ");
        int const p09 = BOOST_LEAF_P("struct boost::leaf::n::r __fastcall boost::leaf::n::p<class ");
        // msvc style, enum:
        int const p10 = BOOST_LEAF_P("struct boost::leaf::n::r __cdecl boost::leaf::n::p<enum ");
        int const p11 = BOOST_LEAF_P("struct boost::leaf::n::r __stdcall boost::leaf::n::p<enum ");
        int const p12 = BOOST_LEAF_P("struct boost::leaf::n::r __fastcall boost::leaf::n::p<enum ");
        // msvc style, built-in type:
        int const p13 = BOOST_LEAF_P("struct boost::leaf::n::r __cdecl boost::leaf::n::p<");
        int const p14 = BOOST_LEAF_P("struct boost::leaf::n::r __stdcall boost::leaf::n::p<");
        int const p15 = BOOST_LEAF_P("struct boost::leaf::n::r __fastcall boost::leaf::n::p<");
#undef BOOST_LEAF_P

#define BOOST_LEAF_S(S) (sizeof(char[1 + detail::check_suffix(BOOST_LEAF_PRETTY_FUNCTION, S)]) - 1)
        // clang/gcc style:
        int const s01 = BOOST_LEAF_S("]");
        // msvc style:
        int const s02 = BOOST_LEAF_S(">(void)");
#undef BOOST_LEAF_S

        char static_assert_unrecognized_pretty_function_format_please_file_github_issue[sizeof(
            char[
                (s01 && (1 == (!!p01 + !!p02 + !!p03)))
                ||
                (s02 && (1 == (!!p04 + !!p05 + !!p06 + !!p07 + !!p08 + !!p09 + !!p10 + !!p11 + !!p12)))
                ||
                (s02 && (1 == (!!p13 + !!p14 + !!p15)))
            ]
        ) * 2 - 1];
        (void) static_assert_unrecognized_pretty_function_format_please_file_github_issue;

        if( int const p = sizeof(char[1 + !!s01 * (p01 + p02 + p03)]) - 1 )
            return { BOOST_LEAF_PRETTY_FUNCTION + p, s01 - p };

        if( int const p = sizeof(char[1 + !!s02 * (p04 + p05 + p06 + p07 + p08 + p09 + p10 + p11 + p12)]) - 1 )
            return { BOOST_LEAF_PRETTY_FUNCTION + p, s02 - p };

        int const p = sizeof(char[1 + !!s02 * (p13 + p14 + p15)]) - 1; // p is not zero, we've static asserted the hell out of it
        return { BOOST_LEAF_PRETTY_FUNCTION + p, s02 - p };
    }
}

using parsed = n::r;

template <class T>
parsed parse()
{
    return n::p<T>();
}

} }

////////////////////////////////////////

namespace boost { namespace leaf {

namespace detail
{
    template <class CharT, class Traits>
    std::ostream & demangle_and_print(std::basic_ostream<CharT, Traits> & os, char const * mangled_name)
    {
        BOOST_LEAF_ASSERT(mangled_name);
#if defined(BOOST_LEAF_CFG_DIAGNOSTICS) && defined(BOOST_LEAF_HAS_CXXABI_H)
        struct raii
        {
            char * demangled_name;
            raii(char const * mangled_name) noexcept 
            {
                int status = 0;
                demangled_name = abi::__cxa_demangle(mangled_name, nullptr, nullptr, &status);
            }
            ~raii() noexcept
            {
                std::free(demangled_name);
            }
        } d(mangled_name);
        if( d.demangled_name )
            return os << d.demangled_name;
#endif
        return os << mangled_name;
    }
}

} }

#endif // BOOST_LEAF_DETAIL_DEMANGLE_HPP_INCLUDED
// <<< #include <boost/leaf/detail/demangle.hpp>
// #line 10 "boost/leaf/common.hpp"

#include <iosfwd>
#include <cerrno>
#include <cstring>

#if BOOST_LEAF_CFG_STD_STRING
#   include <string>
#endif

#if BOOST_LEAF_CFG_WIN32
#   include <windows.h>
#   include <cstring>
#   ifdef min
#       undef min
#   endif
#   ifdef max
#       undef max
#   endif
#endif

namespace boost { namespace leaf {

struct BOOST_LEAF_SYMBOL_VISIBLE e_api_function { char const * value; };

#if BOOST_LEAF_CFG_STD_STRING

struct BOOST_LEAF_SYMBOL_VISIBLE e_file_name
{
    std::string value;
};

#else

struct BOOST_LEAF_SYMBOL_VISIBLE e_file_name
{
    constexpr static char const * const value = "<unavailable>";
    BOOST_LEAF_CONSTEXPR explicit e_file_name( char const * ) { }
};

#endif

struct BOOST_LEAF_SYMBOL_VISIBLE e_errno
{
    int value;

    explicit e_errno(int val=errno): value(val) { }

    template <class CharT, class Traits>
    friend std::ostream & operator<<(std::basic_ostream<CharT, Traits> & os, e_errno const & err)
    {
        return os << err.value << ", \"" << std::strerror(err.value) << '"';
    }
};

struct BOOST_LEAF_SYMBOL_VISIBLE e_type_info_name { char const * value; };

struct BOOST_LEAF_SYMBOL_VISIBLE e_at_line { int value; };

namespace windows
{
    struct e_LastError
    {
        unsigned value;

        explicit e_LastError(unsigned val): value(val) { }

#if BOOST_LEAF_CFG_WIN32
        e_LastError(): value(GetLastError()) { }

        template <class CharT, class Traits>
        friend std::ostream & operator<<(std::basic_ostream<CharT, Traits> & os, e_LastError const & err)
        {
            struct msg_buf
            {
                LPVOID * p;
                msg_buf(): p(nullptr) { }
                ~msg_buf() noexcept { if(p) LocalFree(p); }
            };
            msg_buf mb;
            if( FormatMessageA(
                FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr,
                err.value,
                MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
                (LPSTR)&mb.p,
                0,
                nullptr) )
            {
                BOOST_LEAF_ASSERT(mb.p != nullptr);
                char * z = std::strchr((LPSTR)mb.p,0);
                if( z[-1] == '\n' )
                    *--z = 0;
                if( z[-1] == '\r' )
                    *--z = 0;
                return os << err.value << ", \"" << (LPCSTR)mb.p << '"';
            }
            return os;
        }
#endif
    };
}

} }

#endif // BOOST_LEAF_COMMON_HPP_INCLUDED
// >>> #include <boost/leaf/context.hpp>
#ifndef BOOST_LEAF_CONTEXT_HPP_INCLUDED
#define BOOST_LEAF_CONTEXT_HPP_INCLUDED

// #line 8 "boost/leaf/context.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 14
// >>> #include <boost/leaf/error.hpp>
#ifndef BOOST_LEAF_ERROR_HPP_INCLUDED
#define BOOST_LEAF_ERROR_HPP_INCLUDED

// #line 8 "boost/leaf/error.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 14
// >>> #include <boost/leaf/detail/optional.hpp>
#ifndef BOOST_LEAF_DETAIL_OPTIONAL_HPP_INCLUDED
#define BOOST_LEAF_DETAIL_OPTIONAL_HPP_INCLUDED

// #line 8 "boost/leaf/detail/optional.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 14

#include <utility>
#include <new>

namespace boost { namespace leaf {

namespace detail
{
    template <class T>
    class optional
    {
        int key_;
        union { T value_; };

    public:

        typedef T value_type;

        BOOST_LEAF_CONSTEXPR optional() noexcept:
            key_(0)
        {
        }

        BOOST_LEAF_CONSTEXPR optional( optional const & x ):
            key_(x.key_)
        {
            if( x.key_ )
                (void) new (&value_) T( x.value_ );
        }

        BOOST_LEAF_CONSTEXPR optional( optional && x ) noexcept:
            key_(x.key_)
        {
            if( x.key_ )
            {
                (void) new (&value_) T( std::move(x.value_) );
                x.reset();
            }
        }

        BOOST_LEAF_CONSTEXPR optional( int key, T const & v ):
            key_(key),
            value_(v)
        {
            BOOST_LEAF_ASSERT(!empty());
        }

        BOOST_LEAF_CONSTEXPR optional( int key, T && v ) noexcept:
            key_(key),
            value_(std::move(v))
        {
            BOOST_LEAF_ASSERT(!empty());
        }

        BOOST_LEAF_CONSTEXPR optional & operator=( optional const & x )
        {
            reset();
            if( int key = x.key() )
            {
                load(key, x.value_);
                key_ = key;
            }
            return *this;
        }

        BOOST_LEAF_CONSTEXPR optional & operator=( optional && x ) noexcept
        {
            reset();
            if( int key = x.key() )
            {
                load(key, std::move(x.value_));
                x.reset();
            }
            return *this;
        }

        ~optional() noexcept
        {
            reset();
        }

        BOOST_LEAF_CONSTEXPR bool empty() const noexcept
        {
            return key_ == 0;
        }

        BOOST_LEAF_CONSTEXPR int key() const noexcept
        {
            return key_;
        }

        BOOST_LEAF_CONSTEXPR void reset() noexcept
        {
            if( key_ )
            {
                value_.~T();
                key_=0;
            }
        }

        BOOST_LEAF_CONSTEXPR T & load( int key )
        {
            BOOST_LEAF_ASSERT(key);
            reset();
            (void) new(&value_) T;
            key_=key;
            return value_;
        }

        BOOST_LEAF_CONSTEXPR T & load( int key, T const & v )
        {
            BOOST_LEAF_ASSERT(key);
            reset();
            (void) new(&value_) T(v);
            key_=key;
            return value_;
        }

        BOOST_LEAF_CONSTEXPR T & load( int key, T && v ) noexcept
        {
            BOOST_LEAF_ASSERT(key);
            reset();
            (void) new(&value_) T(std::move(v));
            key_=key;
            return value_;
        }

        BOOST_LEAF_CONSTEXPR T const * has_value_any_key() const noexcept
        {
            return key_ ? &value_ : nullptr;
        }

        BOOST_LEAF_CONSTEXPR T * has_value_any_key() noexcept
        {
            return key_ ? &value_ : nullptr;
        }

        BOOST_LEAF_CONSTEXPR T const * has_value(int key) const noexcept
        {
            BOOST_LEAF_ASSERT(key);
            return key_ == key ? &value_ : nullptr;
        }

        BOOST_LEAF_CONSTEXPR T * has_value(int key) noexcept
        {
            BOOST_LEAF_ASSERT(key);
            return key_ == key ? &value_ : nullptr;
        }

        BOOST_LEAF_CONSTEXPR T const & value(int key) const & noexcept
        {
            BOOST_LEAF_ASSERT(has_value(key) != 0);
            (void) key;
            return value_;
        }

        BOOST_LEAF_CONSTEXPR T & value(int key) & noexcept
        {
            BOOST_LEAF_ASSERT(has_value(key) != 0);
            (void) key;
            return value_;
        }

        BOOST_LEAF_CONSTEXPR T const && value(int key) const && noexcept
        {
            BOOST_LEAF_ASSERT(has_value(key) != 0);
            (void) key;
            return value_;
        }

        BOOST_LEAF_CONSTEXPR T value(int key) && noexcept
        {
            BOOST_LEAF_ASSERT(has_value(key) != 0);
            (void) key;
            T tmp(std::move(value_));
            reset();
            return tmp;
        }

        BOOST_LEAF_CONSTEXPR T & value_or_default(int key) noexcept
        {
            if( T * v = has_value(key) )
                return *v;
            else
                return load(key);
        }
    };

}

} }

#endif // BOOST_LEAF_DETAIL_OPTIONAL_HPP_INCLUDED
// <<< #include <boost/leaf/detail/optional.hpp>
// #line 10 "boost/leaf/error.hpp"
// >>> #include <boost/leaf/detail/function_traits.hpp>
#ifndef BOOST_LEAF_DETAIL_FUNCTION_TRAITS_HPP_INCLUDED
#define BOOST_LEAF_DETAIL_FUNCTION_TRAITS_HPP_INCLUDED

// #line 8 "boost/leaf/detail/function_traits.hpp"
// >>> #include <boost/leaf/detail/mp11.hpp>
#ifndef BOOST_LEAF_DETAIL_MP11_HPP_INCLUDED
#define BOOST_LEAF_DETAIL_MP11_HPP_INCLUDED

//  Copyright 2015-2017 Peter Dimov.
// #line 12 "boost/leaf/detail/mp11.hpp"
#include <type_traits>
#include <cstddef>

namespace boost { namespace leaf { namespace leaf_detail_mp11 {

// mp_list<T...>
template<class... T> struct mp_list
{
};

// mp_identity
template<class T> struct mp_identity
{
    using type = T;
};

// mp_inherit
template<class... T> struct mp_inherit: T... {};

// mp_if, mp_if_c
namespace detail
{

template<bool C, class T, class... E> struct mp_if_c_impl
{
};

template<class T, class... E> struct mp_if_c_impl<true, T, E...>
{
    using type = T;
};

template<class T, class E> struct mp_if_c_impl<false, T, E>
{
    using type = E;
};

} // namespace detail

template<bool C, class T, class... E> using mp_if_c = typename detail::mp_if_c_impl<C, T, E...>::type;
template<class C, class T, class... E> using mp_if = typename detail::mp_if_c_impl<static_cast<bool>(C::value), T, E...>::type;

// mp_bool
template<bool B> using mp_bool = std::integral_constant<bool, B>;

using mp_true = mp_bool<true>;
using mp_false = mp_bool<false>;

// mp_to_bool
template<class T> using mp_to_bool = mp_bool<static_cast<bool>( T::value )>;

// mp_not<T>
template<class T> using mp_not = mp_bool< !T::value >;

// mp_int
template<int I> using mp_int = std::integral_constant<int, I>;

// mp_size_t
template<std::size_t N> using mp_size_t = std::integral_constant<std::size_t, N>;

// mp_set_contains<S, V>
namespace detail
{

template<class S, class V> struct mp_set_contains_impl;

template<template<class...> class L, class... T, class V> struct mp_set_contains_impl<L<T...>, V>
{
    using type = mp_to_bool<std::is_base_of<mp_identity<V>, mp_inherit<mp_identity<T>...> > >;
};

} // namespace detail

template<class S, class V> using mp_set_contains = typename detail::mp_set_contains_impl<S, V>::type;

// mp_set_push_back<S, T...>
namespace detail
{

template<class S, class... T> struct mp_set_push_back_impl;

template<template<class...> class L, class... U> struct mp_set_push_back_impl<L<U...>>
{
    using type = L<U...>;
};

template<template<class...> class L, class... U, class T1, class... T> struct mp_set_push_back_impl<L<U...>, T1, T...>
{
    using S = mp_if<mp_set_contains<L<U...>, T1>, L<U...>, L<U..., T1>>;
    using type = typename mp_set_push_back_impl<S, T...>::type;
};

} // namespace detail

template<class S, class... T> using mp_set_push_back = typename detail::mp_set_push_back_impl<S, T...>::type;

// mp_unique<L>
namespace detail
{

template<class L> struct mp_unique_impl;

template<template<class...> class L, class... T> struct mp_unique_impl<L<T...>>
{
    using type = mp_set_push_back<L<>, T...>;
};

} // namespace detail

template<class L> using mp_unique = typename detail::mp_unique_impl<L>::type;

// mp_append<L...>

namespace detail
{

template<class... L> struct mp_append_impl;

template<> struct mp_append_impl<>
{
    using type = mp_list<>;
};

template<template<class...> class L, class... T> struct mp_append_impl<L<T...>>
{
    using type = L<T...>;
};

template<template<class...> class L1, class... T1, template<class...> class L2, class... T2, class... Lr> struct mp_append_impl<L1<T1...>, L2<T2...>, Lr...>
{
    using type = typename mp_append_impl<L1<T1..., T2...>, Lr...>::type;
};

}

template<class... L> using mp_append = typename detail::mp_append_impl<L...>::type;

// mp_front<L>
namespace detail
{

template<class L> struct mp_front_impl
{
// An error "no type named 'type'" here means that the argument to mp_front
// is either not a list, or is an empty list
};

template<template<class...> class L, class T1, class... T> struct mp_front_impl<L<T1, T...>>
{
    using type = T1;
};

} // namespace detail

template<class L> using mp_front = typename detail::mp_front_impl<L>::type;

// mp_pop_front<L>
namespace detail
{

template<class L> struct mp_pop_front_impl
{
// An error "no type named 'type'" here means that the argument to mp_pop_front
// is either not a list, or is an empty list
};

template<template<class...> class L, class T1, class... T> struct mp_pop_front_impl<L<T1, T...>>
{
    using type = L<T...>;
};

} // namespace detail

template<class L> using mp_pop_front = typename detail::mp_pop_front_impl<L>::type;

// mp_first<L>
template<class L> using mp_first = mp_front<L>;

// mp_rest<L>
template<class L> using mp_rest = mp_pop_front<L>;

// mp_remove_if<L, P>
namespace detail
{

template<class L, template<class...> class P> struct mp_remove_if_impl;

template<template<class...> class L, class... T, template<class...> class P> struct mp_remove_if_impl<L<T...>, P>
{
    template<class U> using _f = mp_if<P<U>, mp_list<>, mp_list<U>>;
    using type = mp_append<L<>, _f<T>...>;
};

} // namespace detail

template<class L, template<class...> class P> using mp_remove_if = typename detail::mp_remove_if_impl<L, P>::type;

// integer_sequence
template<class T, T... I> struct integer_sequence
{
};

// detail::make_integer_sequence_impl
namespace detail
{

// iseq_if_c
template<bool C, class T, class E> struct iseq_if_c_impl;

template<class T, class E> struct iseq_if_c_impl<true, T, E>
{
    using type = T;
};

template<class T, class E> struct iseq_if_c_impl<false, T, E>
{
    using type = E;
};

template<bool C, class T, class E> using iseq_if_c = typename iseq_if_c_impl<C, T, E>::type;

// iseq_identity
template<class T> struct iseq_identity
{
    using type = T;
};

template<class S1, class S2> struct append_integer_sequence;

template<class T, T... I, T... J> struct append_integer_sequence<integer_sequence<T, I...>, integer_sequence<T, J...>>
{
    using type = integer_sequence< T, I..., ( J + sizeof...(I) )... >;
};

template<class T, T N> struct make_integer_sequence_impl;

template<class T, T N> struct make_integer_sequence_impl_
{
private:

    static_assert( N >= 0, "make_integer_sequence<T, N>: N must not be negative" );

    static T const M = N / 2;
    static T const R = N % 2;

    using S1 = typename make_integer_sequence_impl<T, M>::type;
    using S2 = typename append_integer_sequence<S1, S1>::type;
    using S3 = typename make_integer_sequence_impl<T, R>::type;
    using S4 = typename append_integer_sequence<S2, S3>::type;

public:

    using type = S4;
};

template<class T, T N> struct make_integer_sequence_impl: iseq_if_c<N == 0, iseq_identity<integer_sequence<T>>, iseq_if_c<N == 1, iseq_identity<integer_sequence<T, 0>>, make_integer_sequence_impl_<T, N> > >
{
};

} // namespace detail

// make_integer_sequence
template<class T, T N> using make_integer_sequence = typename detail::make_integer_sequence_impl<T, N>::type;

// index_sequence
template<std::size_t... I> using index_sequence = integer_sequence<std::size_t, I...>;

// make_index_sequence
template<std::size_t N> using make_index_sequence = make_integer_sequence<std::size_t, N>;

// index_sequence_for
template<class... T> using index_sequence_for = make_integer_sequence<std::size_t, sizeof...(T)>;

// implementation by Bruno Dutra (by the name is_evaluable)
namespace detail
{

template<template<class...> class F, class... T> struct mp_valid_impl
{
    template<template<class...> class G, class = G<T...>> static mp_true check(int);
    template<template<class...> class> static mp_false check(...);

    using type = decltype(check<F>(0));
};

} // namespace detail

template<template<class...> class F, class... T> using mp_valid = typename detail::mp_valid_impl<F, T...>::type;

} } }

#endif // BOOST_LEAF_DETAIL_MP11_HPP_INCLUDED
// <<< #include <boost/leaf/detail/mp11.hpp>
// #line 9 "boost/leaf/detail/function_traits.hpp"

#include <tuple>

namespace boost { namespace leaf {

namespace detail
{
    template <class T> struct remove_noexcept { using type = T; };
    template <class R, class... A>  struct remove_noexcept<R(*)(A...) noexcept> { using type = R(*)(A...); };
    template <class C, class R, class... A>  struct remove_noexcept<R(C::*)(A...) noexcept> { using type = R(C::*)(A...); };
    template <class C, class R, class... A>  struct remove_noexcept<R(C::*)(A...) const noexcept> { using type = R(C::*)(A...) const; };

    template<class...>
    struct gcc49_workaround //Thanks Glen Fernandes
    {
        using type = void;
    };

    template<class... T>
    using void_t = typename gcc49_workaround<T...>::type;

    template<class F,class V=void>
    struct function_traits_impl
    {
        constexpr static int arity = -1;
    };

    template<class F>
    struct function_traits_impl<F, void_t<decltype(&F::operator())>>
    {
    private:

        using tr = function_traits_impl<typename remove_noexcept<decltype(&F::operator())>::type>;

    public:

        using return_type = typename tr::return_type;
        static constexpr int arity = tr::arity - 1;

        using mp_args = typename leaf_detail_mp11::mp_rest<typename tr::mp_args>;

        template <int I>
        struct arg:
            tr::template arg<I+1>
        {
        };
    };

    template<class R, class... A>
    struct function_traits_impl<R(A...)>
    {
        using return_type = R;
        static constexpr int arity = sizeof...(A);

        using mp_args = leaf_detail_mp11::mp_list<A...>;

        template <int I>
        struct arg
        {
            static_assert(I < arity, "I out of range");
            using type = typename std::tuple_element<I,std::tuple<A...>>::type;
        };
    };

    template<class F> struct function_traits_impl<F&> : function_traits_impl<F> { };
    template<class F> struct function_traits_impl<F&&> : function_traits_impl<F> { };
    template<class R, class... A> struct function_traits_impl<R(*)(A...)> : function_traits_impl<R(A...)> { };
    template<class R, class... A> struct function_traits_impl<R(* &)(A...)> : function_traits_impl<R(A...)> { };
    template<class R, class... A> struct function_traits_impl<R(* const &)(A...)> : function_traits_impl<R(A...)> { };
    template<class C, class R, class... A> struct function_traits_impl<R(C::*)(A...)> : function_traits_impl<R(C&,A...)> { };
    template<class C, class R, class... A> struct function_traits_impl<R(C::*)(A...) const> : function_traits_impl<R(C const &,A...)> { };
    template<class C, class R> struct function_traits_impl<R(C::*)> : function_traits_impl<R(C&)> { };

    template <class F>
    struct function_traits: function_traits_impl<typename remove_noexcept<F>::type>
    {
    };

    template <class F>
    using fn_return_type = typename function_traits<F>::return_type;

    template <class F, int I>
    using fn_arg_type = typename function_traits<F>::template arg<I>::type;

    template <class F>
    using fn_mp_args = typename function_traits<F>::mp_args;
}

} }

#endif // BOOST_LEAF_DETAIL_FUNCTION_TRAITS_HPP_INCLUDED
// <<< #include <boost/leaf/detail/function_traits.hpp>
// #line 11 "boost/leaf/error.hpp"
// >>> #include <boost/leaf/detail/capture_list.hpp>
#ifndef BOOST_LEAF_DETAIL_CAPTURE_LIST_HPP_INCLUDED
#define BOOST_LEAF_DETAIL_CAPTURE_LIST_HPP_INCLUDED

// #line 8 "boost/leaf/detail/capture_list.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 14
// >>> #include <boost/leaf/detail/print.hpp>
#ifndef BOOST_LEAF_DETAIL_PRINT_HPP_INCLUDED
#define BOOST_LEAF_DETAIL_PRINT_HPP_INCLUDED

// #line 8 "boost/leaf/detail/print.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 14
// #include <boost/leaf/detail/demangle.hpp> // Expanded at line 616

#include <type_traits>
#include <exception>
#include <iosfwd>
#include <cstring>

namespace boost { namespace leaf {

template <class E>
struct show_in_diagnostics: std::true_type
{
};

namespace detail
{
    template <class T, class E = void>
    struct is_printable: std::false_type
    {
    };

    template <class T>
    struct is_printable<T, decltype(std::declval<std::ostream&>()<<std::declval<T const &>(), void())>: show_in_diagnostics<T>
    {
    };

    ////////////////////////////////////////

    template <class T, class E = void>
    struct has_printable_member_value: std::false_type
    {
    };

    template <class T>
    struct has_printable_member_value<T, decltype(std::declval<std::ostream&>()<<std::declval<T const &>().value, void())>: show_in_diagnostics<T>
    {
    };

    ////////////////////////////////////////

    template <class T, class CharT, class Traits>
    void print_name(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter)
    {
        static_assert(show_in_diagnostics<T>::value, "show_in_diagnostics violation");
        BOOST_LEAF_ASSERT(delimiter);
        char const * p = prefix;
        prefix = nullptr;
        os << (p ? p : delimiter) << parse<T>();
    }

    template <class T, class PrintableInfo, class CharT, class Traits>
    bool print_impl(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter, char const * mid, PrintableInfo const & x)
    {
        print_name<T>(os, prefix, delimiter);
        if( mid )
            os << mid << x;
        return true;
    }

    template <class T, class PrintableInfo, class CharT, class Traits>
    bool print_impl(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter, char const * mid, PrintableInfo const * x)
    {
        print_name<T>(os, prefix, delimiter);
        if( mid )
        {
            os << mid;
            if( x )
                os << x;
            else
                os << "<nullptr>";
        }
        return true;
    }

    ////////////////////////////////////////

    template <
        class Wrapper,
        bool ShowInDiagnostics = show_in_diagnostics<Wrapper>::value,
        bool WrapperPrintable = is_printable<Wrapper>::value,
        bool ValuePrintable = has_printable_member_value<Wrapper>::value,
        bool IsException = std::is_base_of<std::exception,Wrapper>::value,
        bool IsEnum = std::is_enum<Wrapper>::value>
    struct diagnostic;

    template <class Wrapper, bool WrapperPrintable, bool ValuePrintable, bool IsException, bool IsEnum>
    struct diagnostic<Wrapper, false, WrapperPrintable, ValuePrintable, IsException, IsEnum>
    {
        template <class CharT, class Traits>
        static bool print(std::basic_ostream<CharT, Traits> &, char const * &, char const *, Wrapper const & x) noexcept
        {
            return false;
        }
    };

    template <class Wrapper, bool ValuePrintable, bool IsEnum>
    struct diagnostic<Wrapper, true, true, ValuePrintable, false, IsEnum>
    {
        template <class CharT, class Traits>
        static bool print(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter, Wrapper const & x)
        {
            return print_impl<Wrapper>(os, prefix, delimiter, ": ", x);
        }
    };

    template <class Wrapper>
    struct diagnostic<Wrapper, true, false, true, false, false>
    {
        template <class CharT, class Traits>
        static bool print(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter, Wrapper const & x)
        {
            return print_impl<Wrapper>(os, prefix, delimiter, ": ", x.value);
        }
    };

    template <class Exception, bool WrapperPrintable, bool ValuePrintable>
    struct diagnostic<Exception, true, WrapperPrintable, ValuePrintable, true, false>
    {
        template <class CharT, class Traits>
        static bool print(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter, Exception const & ex)
        {
            if( print_impl<Exception>(os, prefix, delimiter, ": \"", static_cast<std::exception const &>(ex).what()) )
            {
                os << '"';
                return true;
            }
            return false;
        }
    };

    template <class Wrapper>
    struct diagnostic<Wrapper, true, false, false, false, false>
    {
        template <class CharT, class Traits>
        static bool print(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter, Wrapper const &)
        {
            return print_impl<Wrapper>(os, prefix, delimiter, nullptr, 0);
        }
    };

    template <class Enum>
    struct diagnostic<Enum, true, false, false, false, true>
    {
        template <class CharT, class Traits>
        static bool print(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter, Enum const & enum_)
        {
            return print_impl<Enum>(os, prefix, delimiter, ": ", static_cast<typename std::underlying_type<Enum>::type>(enum_));
        }
    };
}

} }

#endif // BOOST_LEAF_DETAIL_PRINT_HPP_INCLUDED
// <<< #include <boost/leaf/detail/print.hpp>
// #line 10 "boost/leaf/detail/capture_list.hpp"

#if BOOST_LEAF_CFG_CAPTURE

#include <iosfwd>

namespace boost { namespace leaf {

class error_id;

namespace detail
{
    struct BOOST_LEAF_SYMBOL_VISIBLE tls_tag_id_factory_current_id;

    class capture_list
    {
        capture_list( capture_list const & ) = delete;
        capture_list & operator=( capture_list const & ) = delete;

    protected:

        class node
        {
            friend class capture_list;

            virtual void unload( int err_id ) = 0;
#if BOOST_LEAF_CFG_DIAGNOSTICS
            virtual void print(std::ostream &, error_id const & to_print, char const * & prefix) const = 0;
#endif

        protected:

            virtual ~node() noexcept
            {
            };

            node * next_;

            BOOST_LEAF_CONSTEXPR explicit node( node * * & last ) noexcept:
                next_(nullptr)
            {
                BOOST_LEAF_ASSERT(last != nullptr);
                *last = this;
                last = &next_;
            }
        } * first_;

        template <class F>
        BOOST_LEAF_CONSTEXPR void for_each( F f ) const
        {
            for( node * p=first_; p; p=p->next_ )
                f(*p);
        }

    public:

        BOOST_LEAF_CONSTEXPR explicit capture_list( node * first ) noexcept:
            first_(first)
        {
        }

        BOOST_LEAF_CONSTEXPR capture_list( capture_list && other ) noexcept:
            first_(other.first_)
        {
            other.first_ = nullptr;
        }

        ~capture_list() noexcept
        {
            for( node const * p = first_; p; )
            {
                node const * n = p -> next_;
                delete p;
                p = n;
            }
        }

        void unload( int const err_id )
        {
            capture_list moved(first_);
            first_ = nullptr;
            tls::write_uint<detail::tls_tag_id_factory_current_id>(unsigned(err_id));
            moved.for_each(
                [err_id]( node & n )
                {
                    n.unload(err_id); // last node may throw
                } );
        }

        template <class CharT, class Traits>
        void print(std::basic_ostream<CharT, Traits> & os, error_id const & to_print, char const * & prefix) const
        {
#if BOOST_LEAF_CFG_DIAGNOSTICS
            if( first_ )
            {
                for_each(
                    [&os, &to_print, &prefix]( node const & n )
                    {
                        n.print(os, to_print, prefix);
                    } );
            }
#else
            (void) os;
            (void) prefix;
            (void) to_print;
#endif
        }
    };

}

} }

#endif

#endif // BOOST_LEAF_DETAIL_CAPTURE_LIST_HPP_INCLUDED
// <<< #include <boost/leaf/detail/capture_list.hpp>
// #line 12 "boost/leaf/error.hpp"

#if BOOST_LEAF_CFG_DIAGNOSTICS
#   include <ostream>
#endif

#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
#   include <system_error>
#endif

#define BOOST_LEAF_TOKEN_PASTE(x, y) x ## y
#define BOOST_LEAF_TOKEN_PASTE2(x, y) BOOST_LEAF_TOKEN_PASTE(x, y)
#define BOOST_LEAF_TMP BOOST_LEAF_TOKEN_PASTE2(boost_leaf_tmp_, __LINE__)

#define BOOST_LEAF_ASSIGN(v,r)\
    auto && BOOST_LEAF_TMP = r;\
    static_assert(::boost::leaf::is_result_type<typename std::decay<decltype(BOOST_LEAF_TMP)>::type>::value,\
        "BOOST_LEAF_ASSIGN/BOOST_LEAF_AUTO requires a result object as the second argument (see is_result_type)");\
    if( !BOOST_LEAF_TMP )\
        return BOOST_LEAF_TMP.error();\
    v = std::forward<decltype(BOOST_LEAF_TMP)>(BOOST_LEAF_TMP).value()

#define BOOST_LEAF_AUTO(v, r)\
    BOOST_LEAF_ASSIGN(auto v, r)

#if BOOST_LEAF_CFG_GNUC_STMTEXPR

#define BOOST_LEAF_CHECK(r)\
    ({\
        auto && BOOST_LEAF_TMP = (r);\
        static_assert(::boost::leaf::is_result_type<typename std::decay<decltype(BOOST_LEAF_TMP)>::type>::value,\
            "BOOST_LEAF_CHECK requires a result object (see is_result_type)");\
        if( !BOOST_LEAF_TMP )\
            return BOOST_LEAF_TMP.error();\
        std::move(BOOST_LEAF_TMP);\
    }).value()

#else

#define BOOST_LEAF_CHECK(r)\
    {\
        auto && BOOST_LEAF_TMP = (r);\
        static_assert(::boost::leaf::is_result_type<typename std::decay<decltype(BOOST_LEAF_TMP)>::type>::value,\
            "BOOST_LEAF_CHECK requires a result object (see is_result_type)");\
        if( !BOOST_LEAF_TMP )\
            return BOOST_LEAF_TMP.error();\
    }

#endif

#define BOOST_LEAF_NEW_ERROR ::boost::leaf::detail::inject_loc{__FILE__,__LINE__,__FUNCTION__}+::boost::leaf::new_error

namespace boost { namespace leaf {

struct BOOST_LEAF_SYMBOL_VISIBLE e_source_location
{
    char const * file;
    int line;
    char const * function;

    template <class CharT, class Traits>
    friend std::ostream & operator<<(std::basic_ostream<CharT, Traits> & os, e_source_location const & x)
    {
        return os << x.file << '(' << x.line << ") in function " << x.function;
    }
};

template <>
struct show_in_diagnostics<e_source_location>: std::false_type
{
};

////////////////////////////////////////

class BOOST_LEAF_SYMBOL_VISIBLE error_id;

namespace detail
{
    class BOOST_LEAF_SYMBOL_VISIBLE exception_base
    {
    public:
        virtual error_id get_error_id() const noexcept = 0;
#if BOOST_LEAF_CFG_DIAGNOSTICS && !defined(BOOST_LEAF_NO_EXCEPTIONS)
        virtual void print_type_name(std::ostream &) const = 0;
#endif
    protected:
        exception_base() noexcept { }
        ~exception_base() noexcept { }
    };
}

////////////////////////////////////////

namespace detail
{
    template <class E>
    class BOOST_LEAF_SYMBOL_VISIBLE slot:
        optional<E>
    {
        slot( slot const & ) = delete;
        slot & operator=( slot const & ) = delete;

        using impl = optional<E>;
        slot<E> * prev_;

    public:

        BOOST_LEAF_CONSTEXPR slot() noexcept:
            prev_(nullptr)
        {
        }

        BOOST_LEAF_CONSTEXPR slot( slot && x ) noexcept:
            optional<E>(std::move(x)),
            prev_(nullptr)
        {
            BOOST_LEAF_ASSERT(x.prev_ == nullptr);
        }

        ~slot() noexcept
        {
            BOOST_LEAF_ASSERT(tls::read_ptr<slot<E>>() != this);
        }

        void activate() noexcept
        {
            prev_ = tls::read_ptr<slot<E>>();
            tls::write_ptr<slot<E>>(this);
        }

        void deactivate() const noexcept
        {
            tls::write_ptr<slot<E>>(prev_);
        }

        void unload( int err_id ) noexcept(!BOOST_LEAF_CFG_CAPTURE);

        template <class CharT, class Traits, class ErrorID>
        void print(std::basic_ostream<CharT, Traits> & os, ErrorID to_print, char const * & prefix) const
        {
            if( int k = this->key() )
            {
                if( to_print && to_print.value() != k )
                    return;
                if( diagnostic<E>::print(os, prefix, BOOST_LEAF_CFG_DIAGNOSTICS_DELIMITER, value(k)) && !to_print )
                    os << '(' << k/4 << ')';
            }
        }

        using impl::load;
        using impl::has_value;
        using impl::has_value_any_key;
        using impl::value;
        using impl::value_or_default;
    };
}

////////////////////////////////////////

#if BOOST_LEAF_CFG_CAPTURE

namespace detail
{
    class BOOST_LEAF_SYMBOL_VISIBLE dynamic_allocator:
        capture_list
    {
        dynamic_allocator( dynamic_allocator const & ) = delete;
        dynamic_allocator & operator=( dynamic_allocator const & ) = delete;

        class capturing_node:
            public capture_list::node
        {
        protected:
            BOOST_LEAF_CONSTEXPR explicit capturing_node( capture_list::node * * & last ) noexcept:
                node(last)
            {
                BOOST_LEAF_ASSERT(last == &next_);
                BOOST_LEAF_ASSERT(next_ == nullptr);
            }
        public:
            virtual void deactivate() const noexcept = 0;
        };

        template <class E>
        class capturing_slot_node:
            public capturing_node,
            public slot<E>
        {
            using impl = slot<E>;
            capturing_slot_node( capturing_slot_node const & ) = delete;
            capturing_slot_node & operator=( capturing_slot_node const & ) = delete;
            void deactivate() const noexcept final override
            {
                impl::deactivate();
            }
            void unload( int err_id ) final override
            {
                impl::unload(err_id);
            }
#if BOOST_LEAF_CFG_DIAGNOSTICS
            void print(std::ostream & os, error_id const & to_print, char const * & prefix) const final override
            {
                impl::print(os, to_print, prefix);
            }
#endif
        public:
            template <class T>
            BOOST_LEAF_CONSTEXPR capturing_slot_node( capture_list::node * * & last, int err_id, T && e ):
                capturing_node(last)
            {
                BOOST_LEAF_ASSERT(last == &next_);
                BOOST_LEAF_ASSERT(next_ == nullptr);
                impl::load(err_id, std::forward<T>(e));
            }
        };

#ifndef BOOST_LEAF_NO_EXCEPTIONS
        class capturing_exception_node:
            public capturing_node
        {
            capturing_exception_node( capturing_exception_node const & ) = delete;
            capturing_exception_node & operator=( capturing_exception_node const & ) = delete;
            void deactivate() const noexcept final override
            {
                BOOST_LEAF_ASSERT(0);
            }
            void unload( int ) final override
            {
                std::rethrow_exception(ex_);
            }
#if BOOST_LEAF_CFG_DIAGNOSTICS
            void print(std::ostream &, error_id const &, char const * &) const final override
            {
            }
#endif
            std::exception_ptr const ex_;
        public:
            capturing_exception_node( capture_list::node * * & last, std::exception_ptr && ex ) noexcept:
                capturing_node(last),
                ex_(std::move(ex))
            {
                BOOST_LEAF_ASSERT(last == &next_);
                BOOST_LEAF_ASSERT(ex_);
            }
        };
#endif

        node * * last_;

    public:

        dynamic_allocator() noexcept:
            capture_list(nullptr),
            last_(&first_)
        {
            BOOST_LEAF_ASSERT(first_ == nullptr);
        }

        dynamic_allocator( dynamic_allocator && other ) noexcept:
            capture_list(std::move(other)),
            last_(other.last_ == &other.first_? &first_ : other.last_)
        {
            BOOST_LEAF_ASSERT(last_ != nullptr);
            BOOST_LEAF_ASSERT(*last_ == nullptr);
            BOOST_LEAF_ASSERT(other.first_ == nullptr);
            other.last_ = &other.first_;
        }

        template <class E>
        typename std::decay<E>::type & dynamic_load(int err_id, E && e)
        {
            using T = typename std::decay<E>::type;
            BOOST_LEAF_ASSERT(last_ != nullptr);
            BOOST_LEAF_ASSERT(*last_ == nullptr);
            BOOST_LEAF_ASSERT(tls::read_ptr<slot<T>>() == nullptr);
            capturing_slot_node<T> * csn = new capturing_slot_node<T>(last_, err_id, std::forward<E>(e));
            csn->activate();
            return csn->value(err_id);
        }

        void deactivate() const noexcept
        {
            for_each(
                []( capture_list::node const & n )
                {
                    static_cast<capturing_node const &>(n).deactivate();
                } );
        }

        template <class LeafResult>
        LeafResult extract_capture_list(int err_id)
        {
#ifndef BOOST_LEAF_NO_EXCEPTIONS
            if( std::exception_ptr ex = std::current_exception() )
                (void) new capturing_exception_node(last_, std::move(ex));
#endif
            detail::capture_list::node * const f = first_;
            first_ = nullptr;
            last_ = &first_;
            return { err_id, capture_list(f) };
        }

        using capture_list::unload;
        using capture_list::print;
    };

    template <>
    inline void slot<dynamic_allocator>::deactivate() const noexcept
    {
        if( dynamic_allocator const * c = this->has_value_any_key() )
            c->deactivate();
        tls::write_ptr<slot<dynamic_allocator>>(prev_);
    }

    template <>
    inline void slot<dynamic_allocator>::unload( int err_id ) noexcept(false)
    {
        BOOST_LEAF_ASSERT(err_id);
        if( dynamic_allocator * da1 = this->has_value_any_key() )
            da1->unload(err_id);
    }

    template <class E>
    inline void dynamic_load_( int err_id, E && e )
    {
        if( slot<dynamic_allocator> * sl = tls::read_ptr<slot<dynamic_allocator>>() )
        {
            if( dynamic_allocator * c = sl->has_value_any_key() )
                c->dynamic_load(err_id, std::forward<E>(e));
            else
                sl->load(err_id).dynamic_load(err_id, std::forward<E>(e));
        }
    }

    template <class E, class F>
    inline void dynamic_accumulate_( int err_id, F && f )
    {
        if( slot<dynamic_allocator> * sl = tls::read_ptr<slot<dynamic_allocator>>() )
        {
            if( dynamic_allocator * c = sl->has_value(err_id) )
                (void) std::forward<F>(f)(c->dynamic_load(err_id, E{}));
            else
                (void) std::forward<F>(f)(sl->load(err_id).dynamic_load(err_id, E{}));
        }
    }

    template <bool OnError, class E>
    inline void dynamic_load( int err_id, E && e  ) noexcept(OnError)
    {
        if( OnError )
        {
#ifndef BOOST_LEAF_NO_EXCEPTIONS
            try
            {
#endif
                dynamic_load_(err_id, std::forward<E>(e));
#ifndef BOOST_LEAF_NO_EXCEPTIONS
            }
            catch(...)
            {
            }
#endif
        }
        else
            dynamic_load_(err_id, std::forward<E>(e));
    }

    template <bool OnError, class E, class F>
    inline void dynamic_load_accumulate( int err_id, F && f  ) noexcept(OnError)
    {
        if( OnError )
        {
#ifndef BOOST_LEAF_NO_EXCEPTIONS
            try
            {
#endif
                dynamic_accumulate_<E>(err_id, std::forward<F>(f));
#ifndef BOOST_LEAF_NO_EXCEPTIONS
            }
            catch(...)
            {
            }
#endif
        }
        else
            dynamic_accumulate_<E>(err_id, std::forward<F>(f));
    }
}

template <>
struct show_in_diagnostics<detail::dynamic_allocator>: std::false_type
{
};

#endif

////////////////////////////////////////

namespace detail
{
    template <class E>
    inline void slot<E>::unload( int err_id ) noexcept(!BOOST_LEAF_CFG_CAPTURE)
    {
        BOOST_LEAF_ASSERT(err_id);
        if( this->key() != err_id )
            return;
        if( impl * p = tls::read_ptr<slot<E>>() )
        {
            if( !p->has_value(err_id) )
                *p = std::move(*this);
        }
#if BOOST_LEAF_CFG_CAPTURE
        else
            dynamic_load<false>(err_id, std::move(*this).value(err_id));
#endif
    }

    template <bool OnError, class E>
    BOOST_LEAF_CONSTEXPR inline int load_slot( int err_id, E && e ) noexcept(OnError)
    {
        using T = typename std::decay<E>::type;
        static_assert(!std::is_pointer<E>::value, "Error objects of pointer types are not allowed");
        static_assert(!std::is_same<T, error_id>::value, "Error objects of type error_id are not allowed");
        BOOST_LEAF_ASSERT((err_id&3) == 1);
        if( slot<T> * p = tls::read_ptr<slot<T>>() )
        {
            if( !OnError || !p->has_value(err_id) )
                (void) p->load(err_id, std::forward<E>(e));
        }
#if BOOST_LEAF_CFG_CAPTURE
        else
            dynamic_load<OnError>(err_id, std::forward<E>(e));
#endif        
        return 0;
    }

    template <bool OnError, class F>
    BOOST_LEAF_CONSTEXPR inline int load_slot_deferred( int err_id, F && f ) noexcept(OnError)
    {
        using E = typename function_traits<F>::return_type;
        using T = typename std::decay<E>::type;
        static_assert(!std::is_pointer<E>::value, "Error objects of pointer types are not allowed");
        static_assert(!std::is_same<T, error_id>::value, "Error objects of type error_id are not allowed");
        BOOST_LEAF_ASSERT((err_id&3) == 1);
        if( slot<T> * p = tls::read_ptr<slot<T>>() )
        {
            if( !OnError || !p->has_value(err_id) )
                (void) p->load(err_id, std::forward<F>(f)());
        }
#if BOOST_LEAF_CFG_CAPTURE
        else
            dynamic_load<OnError>(err_id, std::forward<F>(f)());
#endif        
        return 0;
    }

    template <bool OnError, class F>
    BOOST_LEAF_CONSTEXPR inline int load_slot_accumulate( int err_id, F && f ) noexcept(OnError)
    {
        static_assert(function_traits<F>::arity == 1, "Lambdas passed to accumulate must take a single e-type argument by reference");
        using E = typename std::decay<fn_arg_type<F,0>>::type;
        static_assert(!std::is_pointer<E>::value, "Error objects of pointer types are not allowed");
        BOOST_LEAF_ASSERT((err_id&3) == 1);
        if( auto sl = tls::read_ptr<slot<E>>() )
        {
            if( auto v = sl->has_value(err_id) )
                (void) std::forward<F>(f)(*v);
            else
                (void) std::forward<F>(f)(sl->load(err_id,E()));
        }
#if BOOST_LEAF_CFG_CAPTURE
        else
            dynamic_load_accumulate<OnError, E>(err_id, std::forward<F>(f));
#endif
        return 0;
    }
}

////////////////////////////////////////

namespace detail
{
    template <class T, int Arity = function_traits<T>::arity>
    struct load_item
    {
        static_assert(Arity == 0 || Arity == 1, "If a functions is passed to new_error or load, it must take zero or one argument");
    };

    template <class E>
    struct load_item<E, -1>
    {
        BOOST_LEAF_CONSTEXPR static int load_( int err_id, E && e ) noexcept
        {
            return load_slot<false>(err_id, std::forward<E>(e));
        }
    };

    template <class F>
    struct load_item<F, 0>
    {
        BOOST_LEAF_CONSTEXPR static int load_( int err_id, F && f ) noexcept
        {
            return load_slot_deferred<false>(err_id, std::forward<F>(f));
        }
    };

    template <class F>
    struct load_item<F, 1>
    {
        BOOST_LEAF_CONSTEXPR static int load_( int err_id, F && f ) noexcept
        {
            return load_slot_accumulate<false>(err_id, std::forward<F>(f));
        }
    };
}

////////////////////////////////////////

namespace detail
{
    struct BOOST_LEAF_SYMBOL_VISIBLE tls_tag_id_factory_current_id;

    template <class=void>
    struct BOOST_LEAF_SYMBOL_VISIBLE id_factory
    {
        static atomic_unsigned_int counter;

        BOOST_LEAF_CONSTEXPR static unsigned generate_next_id() noexcept
        {
            auto id = (counter+=4);
            BOOST_LEAF_ASSERT((id&3) == 1);
            return id;
        }
    };

    template <class T>
    atomic_unsigned_int id_factory<T>::counter(1);

    inline int current_id() noexcept
    {
        unsigned id = tls::read_uint<tls_tag_id_factory_current_id>();
        BOOST_LEAF_ASSERT(id == 0 || (id&3) == 1);
        return int(id);
    }

    inline int new_id() noexcept
    {
        unsigned id = id_factory<>::generate_next_id();
        tls::write_uint<tls_tag_id_factory_current_id>(id);
        return int(id);
    }

    struct inject_loc
    {
        char const * file;
        int line;
        char const * fn;

        template <class T>
        friend T operator+( inject_loc loc, T && x ) noexcept
        {
            x.load_source_location_(loc.file, loc.line, loc.fn);
            return std::move(x);
        }
    };
}

#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR

namespace detail
{
    class leaf_category final: public std::error_category
    {
        bool equivalent( int,  std::error_condition const & ) const noexcept final override { return false; }
        bool equivalent( std::error_code const &, int ) const noexcept final override { return false; }
        char const * name() const noexcept final override { return "LEAF error"; }
        std::string message( int ) const final override { return name(); }
    public:
        ~leaf_category() noexcept final override { }
    };

    template <class=void>
    struct get_error_category
    {
        static leaf_category cat;
    };

    template <class T>
    leaf_category get_error_category<T>::cat;

    inline int import_error_code( std::error_code const & ec ) noexcept
    {
        if( int err_id = ec.value() )
        {
            std::error_category const & cat = get_error_category<>::cat;
            if( &ec.category() == &cat )
            {
                BOOST_LEAF_ASSERT((err_id&3) == 1);
                return (err_id&~3)|1;
            }
            else
            {
                err_id = new_id();
                (void) load_slot<false>(err_id, ec);
                return (err_id&~3)|1;
            }
        }
        else
            return 0;
    }
}

inline bool is_error_id( std::error_code const & ec ) noexcept
{
    bool res = (&ec.category() == &detail::get_error_category<>::cat);
    BOOST_LEAF_ASSERT(!res || !ec.value() || ((ec.value()&3) == 1));
    return res;
}

#endif

////////////////////////////////////////

namespace detail
{
    BOOST_LEAF_CONSTEXPR error_id make_error_id(int) noexcept;
}

class BOOST_LEAF_SYMBOL_VISIBLE error_id
{
    friend error_id BOOST_LEAF_CONSTEXPR detail::make_error_id(int) noexcept;

    int value_;

    BOOST_LEAF_CONSTEXPR explicit error_id( int value ) noexcept:
        value_(value)
    {
        BOOST_LEAF_ASSERT(value_ == 0 || ((value_&3) == 1));
    }

public:

    BOOST_LEAF_CONSTEXPR error_id() noexcept:
        value_(0)
    {
    }

#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
    error_id( std::error_code const & ec ) noexcept:
        value_(detail::import_error_code(ec))
    {
        BOOST_LEAF_ASSERT(!value_ || ((value_&3) == 1));
    }

    template <class Enum>
    error_id( Enum e, typename std::enable_if<std::is_error_code_enum<Enum>::value, Enum>::type * = 0 ) noexcept:
        value_(detail::import_error_code(e))
    {
    }

    operator std::error_code() const noexcept
    {
        return std::error_code(value_, detail::get_error_category<>::cat);
    }
#endif

    BOOST_LEAF_CONSTEXPR error_id load() const noexcept
    {
        return *this;
    }

    template <class Item>
    BOOST_LEAF_CONSTEXPR error_id load(Item && item) const noexcept
    {
        if (int err_id = value())
        {
            int const unused[] = { 42, detail::load_item<Item>::load_(err_id, std::forward<Item>(item)) };
            (void)unused;
        }
        return *this;
    }

    template <class... Item>
    BOOST_LEAF_CONSTEXPR error_id load( Item && ... item ) const noexcept
    {
        if( int err_id = value() )
        {
            int const unused[] = { 42, detail::load_item<Item>::load_(err_id, std::forward<Item>(item))... };
            (void) unused;
        }
        return *this;
    }

    BOOST_LEAF_CONSTEXPR int value() const noexcept
    {
        BOOST_LEAF_ASSERT(value_ == 0 || ((value_&3) == 1));
        return value_;
    }

    BOOST_LEAF_CONSTEXPR explicit operator bool() const noexcept
    {
        return value_ != 0;
    }

    BOOST_LEAF_CONSTEXPR friend bool operator==( error_id a, error_id b ) noexcept
    {
        return a.value_ == b.value_;
    }

    BOOST_LEAF_CONSTEXPR friend bool operator!=( error_id a, error_id b ) noexcept
    {
        return !(a == b);
    }

    BOOST_LEAF_CONSTEXPR friend bool operator<( error_id a, error_id b ) noexcept
    {
        return a.value_ < b.value_;
    }

    template <class CharT, class Traits>
    friend std::ostream & operator<<( std::basic_ostream<CharT, Traits> & os, error_id x )
    {
        return os << (x.value_ / 4);
    }

    BOOST_LEAF_CONSTEXPR void load_source_location_( char const * file, int line, char const * function ) const noexcept
    {
        BOOST_LEAF_ASSERT(file&&*file);
        BOOST_LEAF_ASSERT(line>0);
        BOOST_LEAF_ASSERT(function&&*function);
        BOOST_LEAF_ASSERT(value_);
        (void) load(e_source_location {file,line,function});
    }
};

namespace detail
{
    BOOST_LEAF_CONSTEXPR inline error_id make_error_id( int err_id ) noexcept
    {
        BOOST_LEAF_ASSERT(err_id == 0 || (err_id&3) == 1);
        return error_id((err_id&~3)|1);
    }
}

inline error_id new_error() noexcept
{
    return detail::make_error_id(detail::new_id());
}

template <class... Item>
inline error_id new_error( Item && ... item ) noexcept
{
    return detail::make_error_id(detail::new_id()).load(std::forward<Item>(item)...);
}

inline error_id current_error() noexcept
{
    return detail::make_error_id(detail::current_id());
}

////////////////////////////////////////

template <class R>
struct is_result_type: std::false_type
{
};

template <class R>
struct is_result_type<R const>: is_result_type<R>
{
};

} }

#endif // BOOST_LEAF_ERROR_HPP_INCLUDED
// <<< #include <boost/leaf/error.hpp>
// #line 10 "boost/leaf/context.hpp"

#if !defined(BOOST_LEAF_NO_THREADS) && !defined(NDEBUG)
#   include <thread>
#endif

namespace boost { namespace leaf {

class error_info;
class diagnostic_info;
class diagnostic_details;

template <class>
struct is_predicate: std::false_type
{
};

namespace detail
{
    template <class T>
    struct is_exception: std::is_base_of<std::exception, typename std::decay<T>::type>
    {
    };

    template <class E>
    struct handler_argument_traits;

    template <class E, bool IsPredicate = is_predicate<E>::value>
    struct handler_argument_traits_defaults;

    template <class E>
    struct handler_argument_traits_defaults<E, false>
    {
        using error_type = typename std::decay<E>::type;
        using context_types = leaf_detail_mp11::mp_list<error_type>;
        constexpr static bool always_available = false;

        template <class Tup>
        BOOST_LEAF_CONSTEXPR static error_type const * check( Tup const &, error_info const & ) noexcept;

        template <class Tup>
        BOOST_LEAF_CONSTEXPR static error_type * check( Tup &, error_info const & ) noexcept;

        template <class Tup>
        BOOST_LEAF_CONSTEXPR static E get( Tup & tup, error_info const & ei ) noexcept
        {
            return *check(tup, ei);
        }

        static_assert(!is_predicate<error_type>::value, "Handlers must take predicate arguments by value");
        static_assert(!std::is_same<E, error_info>::value, "Handlers must take leaf::error_info arguments by const &");
        static_assert(!std::is_same<E, diagnostic_info>::value, "Handlers must take leaf::diagnostic_info arguments by const &");
        static_assert(!std::is_same<E, diagnostic_details>::value, "Handlers must take leaf::diagnostic_details arguments by const &");
    };

    template <class Pred>
    struct handler_argument_traits_defaults<Pred, true>: handler_argument_traits<typename Pred::error_type>
    {
        using base = handler_argument_traits<typename Pred::error_type>;
        static_assert(!base::always_available, "Predicates can't use types that are always_available");

        template <class Tup>
        BOOST_LEAF_CONSTEXPR static bool check( Tup const & tup, error_info const & ei ) noexcept
        {
            auto e = base::check(tup, ei);
            return e && Pred::evaluate(*e);
        }

        template <class Tup>
        BOOST_LEAF_CONSTEXPR static Pred get( Tup const & tup, error_info const & ei ) noexcept
        {
            return Pred{*base::check(tup, ei)};
        }
    };

    template <class... E>
    struct handler_argument_always_available
    {
        using context_types = leaf_detail_mp11::mp_list<E...>;
        constexpr static bool always_available = true;

        template <class Tup>
        BOOST_LEAF_CONSTEXPR static bool check( Tup &, error_info const & ) noexcept
        {
            return true;
        }
    };

    template <class E>
    struct handler_argument_traits: handler_argument_traits_defaults<E>
    {
    };

    template <>
    struct handler_argument_traits<void>
    {
        using context_types = leaf_detail_mp11::mp_list<>;
        constexpr static bool always_available = false;

        template <class Tup>
        BOOST_LEAF_CONSTEXPR static std::exception const * check( Tup const &, error_info const & ) noexcept;
    };

    template <class E>
    struct handler_argument_traits<E &&>
    {
        static_assert(sizeof(E) == 0, "Error handlers may not take rvalue ref arguments");
    };

    template <class E>
    struct handler_argument_traits<E *>: handler_argument_always_available<typename std::remove_const<E>::type>
    {
        template <class Tup>
        BOOST_LEAF_CONSTEXPR static E * get( Tup & tup, error_info const & ei) noexcept
        {
            return handler_argument_traits_defaults<E>::check(tup, ei);
        }
    };

    template <class E>
    struct handler_argument_traits_require_by_value
    {
        static_assert(sizeof(E) == 0, "Error handlers must take this type by value");
    };
}

////////////////////////////////////////

namespace detail
{
    template <class T>
    struct get_dispatch
    {
        static BOOST_LEAF_CONSTEXPR T const * get(T const * x) noexcept
        {
            return x;
        }
        static BOOST_LEAF_CONSTEXPR T const * get(void const *) noexcept
        {
            return nullptr;
        }
    };

    template <class T>
    BOOST_LEAF_CONSTEXPR inline T * find_in_tuple(std::tuple<> const &)
    {
        return nullptr;
    }

    template <class T, int I = 0, class... Tp>
    BOOST_LEAF_CONSTEXPR inline typename std::enable_if<I == sizeof...(Tp) - 1, T>::type const *
    find_in_tuple(std::tuple<Tp...> const & t) noexcept
    {
        return get_dispatch<T>::get(&std::get<I>(t));
    }

    template<class T, int I = 0, class... Tp>
    BOOST_LEAF_CONSTEXPR inline typename std::enable_if<I < sizeof...(Tp) - 1, T>::type const *
    find_in_tuple(std::tuple<Tp...> const & t) noexcept
    {
        if( T const * x = get_dispatch<T>::get(&std::get<I>(t)) )
            return x;
        else
            return find_in_tuple<T, I+1, Tp...>(t);
    }
}

////////////////////////////////////////

namespace detail
{
    template <int I, class Tup>
    struct tuple_for_each
    {
        BOOST_LEAF_CONSTEXPR static void activate( Tup & tup ) noexcept
        {
            static_assert(!std::is_same<error_info, typename std::decay<decltype(std::get<I-1>(tup))>::type>::value, "Bug in LEAF: context type deduction");
            tuple_for_each<I-1,Tup>::activate(tup);
            std::get<I-1>(tup).activate();
        }

        BOOST_LEAF_CONSTEXPR static void deactivate( Tup & tup ) noexcept
        {
            static_assert(!std::is_same<error_info, typename std::decay<decltype(std::get<I-1>(tup))>::type>::value, "Bug in LEAF: context type deduction");
            std::get<I-1>(tup).deactivate();
            tuple_for_each<I-1,Tup>::deactivate(tup);
        }

        BOOST_LEAF_CONSTEXPR static void unload( Tup & tup, int err_id ) noexcept
        {
            static_assert(!std::is_same<error_info, typename std::decay<decltype(std::get<I-1>(tup))>::type>::value, "Bug in LEAF: context type deduction");
            BOOST_LEAF_ASSERT(err_id != 0);
            auto & sl = std::get<I-1>(tup);
            sl.unload(err_id);
            tuple_for_each<I-1,Tup>::unload(tup, err_id);
        }

        template <class CharT, class Traits>
        static void print(std::basic_ostream<CharT, Traits> & os, void const * tup, error_id to_print, char const * & prefix)
        {
            BOOST_LEAF_ASSERT(tup != nullptr);
            tuple_for_each<I-1,Tup>::print(os, tup, to_print, prefix);
            std::get<I-1>(*static_cast<Tup const *>(tup)).print(os, to_print, prefix);
        }
    };

    template <class Tup>
    struct tuple_for_each<0, Tup>
    {
        BOOST_LEAF_CONSTEXPR static void activate( Tup & ) noexcept { }
        BOOST_LEAF_CONSTEXPR static void deactivate( Tup & ) noexcept { }
        BOOST_LEAF_CONSTEXPR static void unload( Tup &, int ) noexcept { }
        template <class CharT, class Traits>
        BOOST_LEAF_CONSTEXPR static void print(std::basic_ostream<CharT, Traits> &, void const *, error_id, char const * &) { }
    };

    template <class Tup, class CharT, class Traits>
    BOOST_LEAF_CONSTEXPR void print_tuple_contents(std::basic_ostream<CharT, Traits> & os, void const * tup, error_id to_print, char const * & prefix)
    {
        tuple_for_each<std::tuple_size<Tup>::value, Tup>::print(os, tup, to_print, prefix);
    }
}

////////////////////////////////////////

namespace detail
{
    template <class T> struct does_not_participate_in_context_deduction: std::is_abstract<T> { };
    template <> struct does_not_participate_in_context_deduction<error_id>: std::true_type { };

    template <class L>
    struct deduce_e_type_list;

    template <template<class...> class L, class... T>
    struct deduce_e_type_list<L<T...>>
    {
        using type =
            leaf_detail_mp11::mp_remove_if<
                leaf_detail_mp11::mp_unique<
                    leaf_detail_mp11::mp_append<typename handler_argument_traits<T>::context_types...>
                >,
                does_not_participate_in_context_deduction
            >;
    };

    template <class L>
    struct deduce_e_tuple_impl;

    template <template <class...> class L, class... E>
    struct deduce_e_tuple_impl<L<E...>>
    {
        using type = std::tuple<slot<E>...>;
    };

    template <class... E>
    using deduce_e_tuple = typename deduce_e_tuple_impl<typename deduce_e_type_list<leaf_detail_mp11::mp_list<E...>>::type>::type;
}

////////////////////////////////////////

template <class... E>
class context
{
    context( context const & ) = delete;
    context & operator=( context const & ) = delete;

    using Tup = detail::deduce_e_tuple<E...>;
    Tup tup_;
    bool is_active_;

#if !defined(BOOST_LEAF_NO_THREADS) && !defined(NDEBUG)
    std::thread::id thread_id_;
#endif

    class raii_deactivator
    {
        raii_deactivator( raii_deactivator const & ) = delete;
        raii_deactivator & operator=( raii_deactivator const & ) = delete;
        context * ctx_;
    public:
        explicit BOOST_LEAF_CONSTEXPR BOOST_LEAF_ALWAYS_INLINE raii_deactivator(context & ctx) noexcept:
            ctx_(ctx.is_active() ? nullptr : &ctx)
        {
            if( ctx_ )
                ctx_->activate();
        }
        BOOST_LEAF_CONSTEXPR BOOST_LEAF_ALWAYS_INLINE raii_deactivator( raii_deactivator && x ) noexcept:
            ctx_(x.ctx_)
        {
            x.ctx_ = nullptr;
        }
        BOOST_LEAF_ALWAYS_INLINE ~raii_deactivator() noexcept
        {
            if( ctx_ && ctx_->is_active() )
                ctx_->deactivate();
        }
    };

public:

    BOOST_LEAF_CONSTEXPR context( context && x ) noexcept:
        tup_(std::move(x.tup_)),
        is_active_(false)
    {
        BOOST_LEAF_ASSERT(!x.is_active());
    }

    BOOST_LEAF_CONSTEXPR context() noexcept:
        is_active_(false)
    {
    }

    ~context() noexcept
    {
        BOOST_LEAF_ASSERT(!is_active());
    }

    BOOST_LEAF_CONSTEXPR Tup const & tup() const noexcept
    {
        return tup_;
    }

    BOOST_LEAF_CONSTEXPR Tup & tup() noexcept
    {
        return tup_;
    }

    BOOST_LEAF_CONSTEXPR void activate() noexcept
    {
        using namespace detail;
        BOOST_LEAF_ASSERT(!is_active());
        tuple_for_each<std::tuple_size<Tup>::value,Tup>::activate(tup_);
#if !defined(BOOST_LEAF_NO_THREADS) && !defined(NDEBUG)
        thread_id_ = std::this_thread::get_id();
#endif
        is_active_ = true;
    }

    BOOST_LEAF_CONSTEXPR void deactivate() noexcept
    {
        using namespace detail;
        BOOST_LEAF_ASSERT(is_active());
        is_active_ = false;
#if !defined(BOOST_LEAF_NO_THREADS) && !defined(NDEBUG)
        BOOST_LEAF_ASSERT(std::this_thread::get_id() == thread_id_);
        thread_id_ = std::thread::id();
#endif
        tuple_for_each<std::tuple_size<Tup>::value,Tup>::deactivate(tup_);
    }

    BOOST_LEAF_CONSTEXPR void unload(error_id id) noexcept
    {
        BOOST_LEAF_ASSERT(!is_active());
        detail::tuple_for_each<std::tuple_size<Tup>::value,Tup>::unload(tup_, id.value());
    }

    BOOST_LEAF_CONSTEXPR bool is_active() const noexcept
    {
        return is_active_;
    }

    template <class CharT, class Traits>
    void print( std::basic_ostream<CharT, Traits> & os ) const
    {
        char const * prefix = "Contents:";
        detail::print_tuple_contents<Tup>(os, &tup_, error_id(), prefix);
    }

    template <class CharT, class Traits>
    friend std::ostream & operator<<( std::basic_ostream<CharT, Traits> & os, context const & ctx )
    {
        ctx.print(os);
        return os << '\n';
    }

    template <class T>
    BOOST_LEAF_CONSTEXPR T const * get(error_id err) const noexcept
    {
        detail::slot<T> const * e = detail::find_in_tuple<detail::slot<T>>(tup_);
        return e ? e->has_value(err.value()) : nullptr;
    }

    template <class R, class... H>
    BOOST_LEAF_CONSTEXPR R handle_error( error_id, H && ... ) const;

    template <class R, class... H>
    BOOST_LEAF_CONSTEXPR R handle_error( error_id, H && ... );

    friend BOOST_LEAF_CONSTEXPR BOOST_LEAF_ALWAYS_INLINE raii_deactivator activate_context(context & ctx) noexcept
    {
        return raii_deactivator(ctx);
    }
};

////////////////////////////////////////

namespace detail
{
    template <class TypeList>
    struct deduce_context_impl;

    template <template <class...> class L, class... E>
    struct deduce_context_impl<L<E...>>
    {
        using type = context<E...>;
    };

    template <class TypeList>
    using deduce_context = typename deduce_context_impl<TypeList>::type;

    template <class H>
    struct fn_mp_args_fwd
    {
        using type = fn_mp_args<H>;
    };

    template <class... H>
    struct fn_mp_args_fwd<std::tuple<H...> &>: fn_mp_args_fwd<std::tuple<H...>> { };

    template <class... H>
    struct fn_mp_args_fwd<std::tuple<H...> const &>: fn_mp_args_fwd<std::tuple<H...>> { };

    template <class... H>
    struct fn_mp_args_fwd<std::tuple<H...>>
    {
        using type = leaf_detail_mp11::mp_append<typename fn_mp_args_fwd<H>::type...>;
    };

    template <class... H>
    struct context_type_from_handlers_impl
    {
        using type = deduce_context<leaf_detail_mp11::mp_append<typename fn_mp_args_fwd<H>::type...>>;
    };
}

template <class... H>
using context_type_from_handlers = typename detail::context_type_from_handlers_impl<H...>::type;

////////////////////////////////////////

template <class...  H>
BOOST_LEAF_CONSTEXPR inline context_type_from_handlers<H...> make_context() noexcept
{
    return { };
}

template <class...  H>
BOOST_LEAF_CONSTEXPR inline context_type_from_handlers<H...> make_context( H && ... ) noexcept
{
    return { };
}

} }

#endif // BOOST_LEAF_CONTEXT_HPP_INCLUDED
// >>> #include <boost/leaf/diagnostics.hpp>
#ifndef BOOST_LEAF_DIAGNOSTICS_HPP_INCLUDED
#define BOOST_LEAF_DIAGNOSTICS_HPP_INCLUDED

// #line 8 "boost/leaf/diagnostics.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 14
// #include <boost/leaf/context.hpp> // Expanded at line 965
// >>> #include <boost/leaf/handle_errors.hpp>
#ifndef BOOST_LEAF_HANDLE_ERRORS_HPP_INCLUDED
#define BOOST_LEAF_HANDLE_ERRORS_HPP_INCLUDED

// #line 8 "boost/leaf/handle_errors.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 14
// #include <boost/leaf/context.hpp> // Expanded at line 965
#include <typeinfo>

namespace boost { namespace leaf {

template <class T>
class BOOST_LEAF_SYMBOL_VISIBLE result;

////////////////////////////////////////

#ifndef BOOST_LEAF_NO_EXCEPTIONS

namespace detail
{
    inline error_id unpack_error_id(std::exception const & ex) noexcept
    {
        if( detail::exception_base const * eb = dynamic_cast<detail::exception_base const *>(&ex) )
            return eb->get_error_id();
        if( error_id const * err_id = dynamic_cast<error_id const *>(&ex) )
            return *err_id;
        return current_error();
    }
}

#endif

////////////////////////////////////////

class BOOST_LEAF_SYMBOL_VISIBLE error_info
{
    error_info & operator=( error_info const & ) = delete;

    error_id const err_id_;
#ifndef BOOST_LEAF_NO_EXCEPTIONS
    std::exception * const ex_;
#endif
    e_source_location const * const loc_;

protected:

    error_info( error_info const & ) noexcept = default;

public:

    BOOST_LEAF_CONSTEXPR error_info(error_id id, std::exception * ex, e_source_location const * loc) noexcept:
        err_id_(id),
#ifndef BOOST_LEAF_NO_EXCEPTIONS
        ex_(ex),
#endif
        loc_(loc)
    {
        (void) ex;
    }

    BOOST_LEAF_CONSTEXPR error_id error() const noexcept
    {
        return err_id_;
    }

    BOOST_LEAF_CONSTEXPR std::exception * exception() const noexcept
    {
#ifdef BOOST_LEAF_NO_EXCEPTIONS
        return nullptr;
#else
        return ex_;
#endif
    }

    BOOST_LEAF_CONSTEXPR e_source_location const * source_location() const noexcept
    {
        return loc_;
    }

    template <class CharT, class Traits>
    void print_error_info(std::basic_ostream<CharT, Traits> & os) const
    {
        os << "Error with serial #" << err_id_;
        if( loc_ )
            os << " reported at " << *loc_;
#ifndef BOOST_LEAF_NO_EXCEPTIONS
        if( ex_ )
        {
            os << "\nCaught:" BOOST_LEAF_CFG_DIAGNOSTICS_FIRST_DELIMITER;
#if BOOST_LEAF_CFG_DIAGNOSTICS
            if( auto eb = dynamic_cast<detail::exception_base const *>(ex_) )
                eb->print_type_name(os);
            else
#endif
                detail::demangle_and_print(os, typeid(*ex_).name());
            os << ": \"" << ex_->what() << '"';
        }
#endif
    }

    template <class CharT, class Traits>
    friend std::ostream & operator<<(std::basic_ostream<CharT, Traits> & os, error_info const & x)
    {
        x.print_error_info(os);
        return os << '\n';
    }
};

namespace detail
{
    template <>
    struct handler_argument_traits<error_info const &>: handler_argument_always_available<>
    {
        template <class Tup>
        BOOST_LEAF_CONSTEXPR static error_info const & get(Tup const &, error_info const & ei) noexcept
        {
            return ei;
        }
    };
}

////////////////////////////////////////

namespace detail
{
    template <class T, class... List>
    struct type_index;

    template <class T, class... Cdr>
    struct type_index<T, T, Cdr...>
    {
        constexpr static int value = 0;
    };

    template <class T, class Car, class... Cdr>
    struct type_index<T, Car, Cdr...>
    {
        constexpr static int value = 1 + type_index<T,Cdr...>::value;
    };

    template <class T, class Tup>
    struct tuple_type_index;

    template <class T, class... TupleTypes>
    struct tuple_type_index<T,std::tuple<TupleTypes...>>
    {
        constexpr static int value = type_index<T,TupleTypes...>::value;
    };

#ifndef BOOST_LEAF_NO_EXCEPTIONS

    template <class E, bool = std::is_class<E>::value>
    struct peek_exception;

    template <>
    struct peek_exception<std::exception const, true>
    {
        BOOST_LEAF_CONSTEXPR static std::exception const * peek( error_info const & ei ) noexcept
        {
            return ei.exception();
        }
    };

    template <>
    struct peek_exception<std::exception, true>
    {
        BOOST_LEAF_CONSTEXPR static std::exception * peek( error_info const & ei ) noexcept
        {
            return ei.exception();
        }
    };

#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
    template <>
    struct peek_exception<std::error_code const, true>
    {
        static std::error_code const * peek( error_info const & ei ) noexcept
        {
            auto const ex = ei.exception();
            if( std::system_error * se = dynamic_cast<std::system_error *>(ex) )
                return &se->code();
            else if( std::error_code * ec = dynamic_cast<std::error_code *>(ex) )
                return ec;
            else
                return nullptr;
        }
    };

    template <>
    struct peek_exception<std::error_code, true>
    {
        static std::error_code * peek( error_info const & ei ) noexcept
        {
            auto const ex = ei.exception();
            if( std::system_error * se = dynamic_cast<std::system_error *>(ex) )
                return const_cast<std::error_code *>(&se->code());
            else if( std::error_code * ec = dynamic_cast<std::error_code *>(ex) )
                return ec;
            else
                return nullptr;
        }
    };
#endif

    template <class E>
    struct peek_exception<E, true>
    {
        static E * peek( error_info const & ei ) noexcept
        {
            return dynamic_cast<E *>(ei.exception());
        }
    };

    template <class E>
    struct peek_exception<E, false>
    {
        BOOST_LEAF_CONSTEXPR static E * peek( error_info const & ) noexcept
        {
            return nullptr;
        }
    };

#endif

    template <class E, bool = does_not_participate_in_context_deduction<E>::value>
    struct peek_tuple;

    template <class E>
    struct peek_tuple<E, true>
    {
        template <class SlotsTuple>
        BOOST_LEAF_CONSTEXPR static E const * peek( SlotsTuple const &, error_id const & ) noexcept
        {
            return nullptr;
        }
        
        template <class SlotsTuple>
        BOOST_LEAF_CONSTEXPR static E * peek( SlotsTuple &, error_id const & ) noexcept
        {
            return nullptr;
        }
    };

    template <class E>
    struct peek_tuple<E, false>
    {
        template <class SlotsTuple>
        BOOST_LEAF_CONSTEXPR static E const * peek( SlotsTuple const & tup, error_id const & err ) noexcept
        {
            return std::get<tuple_type_index<slot<E>,SlotsTuple>::value>(tup).has_value(err.value());
        }

        template <class SlotsTuple>
        BOOST_LEAF_CONSTEXPR static E * peek( SlotsTuple & tup, error_id const & err ) noexcept
        {
            return std::get<tuple_type_index<slot<E>,SlotsTuple>::value>(tup).has_value(err.value());
        }
    };

    template <class E, class SlotsTuple>
    BOOST_LEAF_CONSTEXPR inline
    E const *
    peek( SlotsTuple const & tup, error_info const & ei ) noexcept
    {
        if( error_id err = ei.error() )
        {
            if( E const * e = peek_tuple<E>::peek(tup, err) )
                return e;
#ifndef BOOST_LEAF_NO_EXCEPTIONS
            else
                return peek_exception<E const>::peek(ei);
#endif
        }
        return nullptr;
    }

    template <class E, class SlotsTuple>
    BOOST_LEAF_CONSTEXPR inline
    E *
    peek( SlotsTuple & tup, error_info const & ei ) noexcept
    {
        if( error_id err = ei.error() )
        {
            if( E * e = peek_tuple<E>::peek(tup, err) )
                return e;
#ifndef BOOST_LEAF_NO_EXCEPTIONS
            else
                return peek_exception<E>::peek(ei);
#endif
        }
        return nullptr;
    }
}

////////////////////////////////////////

namespace detail
{
    template <class A>
    template <class Tup>
    BOOST_LEAF_CONSTEXPR inline
    typename handler_argument_traits_defaults<A, false>::error_type const *
    handler_argument_traits_defaults<A, false>::
    check( Tup const & tup, error_info const & ei ) noexcept
    {
        return peek<typename std::decay<A>::type>(tup, ei);
    }

    template <class A>
    template <class Tup>
    BOOST_LEAF_CONSTEXPR inline
    typename handler_argument_traits_defaults<A, false>::error_type *
    handler_argument_traits_defaults<A, false>::
    check( Tup & tup, error_info const & ei ) noexcept
    {
        return peek<typename std::decay<A>::type>(tup, ei);
    }

    template <class Tup>
    BOOST_LEAF_CONSTEXPR inline
    std::exception const *
    handler_argument_traits<void>::
    check( Tup const &, error_info const & ei ) noexcept
    {
        return ei.exception();
    }

    template <class Tup, class... List>
    struct check_arguments;

    template <class Tup>
    struct check_arguments<Tup>
    {
        BOOST_LEAF_CONSTEXPR static bool check( Tup const &, error_info const & )
        {
            return true;
        }
    };

    template <class Tup, class Car, class... Cdr>
    struct check_arguments<Tup, Car, Cdr...>
    {
        BOOST_LEAF_CONSTEXPR static bool check( Tup & tup, error_info const & ei ) noexcept
        {
            return handler_argument_traits<Car>::check(tup, ei) && check_arguments<Tup, Cdr...>::check(tup, ei);
        }
    };
}

////////////////////////////////////////

namespace detail
{
    template <class>
    struct handler_matches_any_error: std::false_type
    {
    };

    template <template<class...> class L>
    struct handler_matches_any_error<L<>>: std::true_type
    {
    };

    template <template<class...> class L, class Car, class... Cdr>
    struct handler_matches_any_error<L<Car, Cdr...>>
    {
        constexpr static bool value = handler_argument_traits<Car>::always_available && handler_matches_any_error<L<Cdr...>>::value;
    };
}

////////////////////////////////////////

namespace detail
{
    template <class Tup, class... A>
    BOOST_LEAF_CONSTEXPR inline bool check_handler_( Tup & tup, error_info const & ei, leaf_detail_mp11::mp_list<A...> ) noexcept
    {
        return check_arguments<Tup, A...>::check(tup, ei);
    }

    template <class R, class F, bool IsResult = is_result_type<R>::value, class FReturnType = fn_return_type<F>>
    struct handler_caller
    {
        template <class Tup, class... A>
        BOOST_LEAF_CONSTEXPR static R call( Tup & tup, error_info const & ei, F && f, leaf_detail_mp11::mp_list<A...> )
        {
            return std::forward<F>(f)( handler_argument_traits<A>::get(tup, ei)... );
        }
    };

    template <template <class...> class Result, class... E, class F>
    struct handler_caller<Result<void, E...>, F, true, void>
    {
        using R = Result<void, E...>;

        template <class Tup, class... A>
        BOOST_LEAF_CONSTEXPR static R call( Tup & tup, error_info const & ei, F && f, leaf_detail_mp11::mp_list<A...> )
        {
            std::forward<F>(f)( handler_argument_traits<A>::get(tup, ei)... );
            return { };
        }
    };

    template <class T>
    struct is_tuple: std::false_type { };

    template <class... T>
    struct is_tuple<std::tuple<T...>>: std::true_type { };

    template <class... T>
    struct is_tuple<std::tuple<T...> &>: std::true_type { };

    template <class R, class Tup, class H>
    BOOST_LEAF_CONSTEXPR inline
    typename std::enable_if<!is_tuple<typename std::decay<H>::type>::value, R>::type
    handle_error_( Tup & tup, error_info const & ei, H && h )
    {
        static_assert( handler_matches_any_error<fn_mp_args<H>>::value, "The last handler passed to handle_all must match any error." );
        return handler_caller<R, H>::call( tup, ei, std::forward<H>(h), fn_mp_args<H>{ } );
    }

    template <class R, class Tup, class Car, class... Cdr>
    BOOST_LEAF_CONSTEXPR inline
    typename std::enable_if<!is_tuple<typename std::decay<Car>::type>::value, R>::type
    handle_error_( Tup & tup, error_info const & ei, Car && car, Cdr && ... cdr )
    {
        if( handler_matches_any_error<fn_mp_args<Car>>::value || check_handler_( tup, ei, fn_mp_args<Car>{ } ) )
            return handler_caller<R, Car>::call( tup, ei, std::forward<Car>(car), fn_mp_args<Car>{ } );
        else
            return handle_error_<R>( tup, ei, std::forward<Cdr>(cdr)...);
    }

    template <class R, class Tup, class HTup, size_t ... I>
    BOOST_LEAF_CONSTEXPR inline
    R
    handle_error_tuple_( Tup & tup, error_info const & ei, leaf_detail_mp11::index_sequence<I...>, HTup && htup )
    {
        return handle_error_<R>(tup, ei, std::get<I>(std::forward<HTup>(htup))...);
    }

    template <class R, class Tup, class HTup, class... Cdr, size_t ... I>
    BOOST_LEAF_CONSTEXPR inline
    R
    handle_error_tuple_( Tup & tup, error_info const & ei, leaf_detail_mp11::index_sequence<I...>, HTup && htup, Cdr && ... cdr )
    {
        return handle_error_<R>(tup, ei, std::get<I>(std::forward<HTup>(htup))..., std::forward<Cdr>(cdr)...);
    }

    template <class R, class Tup, class H>
    BOOST_LEAF_CONSTEXPR inline
    typename std::enable_if<is_tuple<typename std::decay<H>::type>::value, R>::type
    handle_error_( Tup & tup, error_info const & ei, H && h )
    {
        return handle_error_tuple_<R>(
            tup,
            ei,
            leaf_detail_mp11::make_index_sequence<std::tuple_size<typename std::decay<H>::type>::value>(),
            std::forward<H>(h));
    }

    template <class R, class Tup, class Car, class... Cdr>
    BOOST_LEAF_CONSTEXPR inline
    typename std::enable_if<is_tuple<typename std::decay<Car>::type>::value, R>::type
    handle_error_( Tup & tup, error_info const & ei, Car && car, Cdr && ... cdr )
    {
        return handle_error_tuple_<R>(
            tup,
            ei,
            leaf_detail_mp11::make_index_sequence<std::tuple_size<typename std::decay<Car>::type>::value>(),
            std::forward<Car>(car),
            std::forward<Cdr>(cdr)...);
    }
}

////////////////////////////////////////

template <class... E>
template <class R, class... H>
BOOST_LEAF_CONSTEXPR BOOST_LEAF_ALWAYS_INLINE
R
context<E...>::
handle_error( error_id id, H && ... h ) const
{
    BOOST_LEAF_ASSERT(!is_active());
    return detail::handle_error_<R>(tup(), error_info(id, nullptr, this->get<e_source_location>(id)), std::forward<H>(h)...);
}

template <class... E>
template <class R, class... H>
BOOST_LEAF_CONSTEXPR BOOST_LEAF_ALWAYS_INLINE
R
context<E...>::
handle_error( error_id id, H && ... h )
{
    BOOST_LEAF_ASSERT(!is_active());
    return detail::handle_error_<R>(tup(), error_info(id, nullptr, this->get<e_source_location>(id)), std::forward<H>(h)...);
}

////////////////////////////////////////

namespace detail
{
    template <class T>
    void unload_result( result<T> * r )
    {
        (void) r->unload();
    }

    inline void unload_result( void * )
    {
    }
}

////////////////////////////////////////

#ifdef BOOST_LEAF_NO_EXCEPTIONS

template <class TryBlock, class... H>
BOOST_LEAF_CONSTEXPR inline
typename std::decay<decltype(std::declval<TryBlock>()().value())>::type
try_handle_all( TryBlock && try_block, H && ... h ) noexcept
{
    static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The return type of the try_block passed to a try_handle_all function must be registered with leaf::is_result_type");
    context_type_from_handlers<H...> ctx;
    auto active_context = activate_context(ctx);
    if( auto r = std::forward<TryBlock>(try_block)() )
        return std::move(r).value();
    else
    {
        detail::unload_result(&r);
        error_id id = r.error();
        ctx.deactivate();
        using R = typename std::decay<decltype(std::declval<TryBlock>()().value())>::type;
        return ctx.template handle_error<R>(std::move(id), std::forward<H>(h)...);
    }
}

template <class TryBlock, class... H>
BOOST_LEAF_ATTRIBUTE_NODISCARD BOOST_LEAF_CONSTEXPR inline
typename std::decay<decltype(std::declval<TryBlock>()())>::type
try_handle_some( TryBlock && try_block, H && ... h ) noexcept
{
    static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The return type of the try_block passed to a try_handle_some function must be registered with leaf::is_result_type");
    context_type_from_handlers<H...> ctx;
    auto active_context = activate_context(ctx);
    if( auto r = std::forward<TryBlock>(try_block)() )
        return r;
    else
    {
        detail::unload_result(&r);
        error_id id = r.error();
        ctx.deactivate();
        using R = typename std::decay<decltype(std::declval<TryBlock>()())>::type;
        auto rr = ctx.template handle_error<R>(std::move(id), std::forward<H>(h)..., [&r]()->R { return std::move(r); });
        if( !rr )
            ctx.unload(rr.error());
        return rr;
    }
}

template <class TryBlock, class... H>
BOOST_LEAF_CONSTEXPR inline
decltype(std::declval<TryBlock>()())
try_catch( TryBlock && try_block, H && ... ) noexcept
{
    static_assert(sizeof(context_type_from_handlers<H...>) > 0,
        "When exceptions are disabled, try_catch can't fail and has no use for the handlers, but this ensures that the supplied H... types are compatible.");
    return std::forward<TryBlock>(try_block)();
}

#else

namespace detail
{
    template <class Ctx, class TryBlock, class... H>
    decltype(std::declval<TryBlock>()())
    try_catch_( Ctx & ctx, TryBlock && try_block, H && ... h )
    {
        using namespace detail;
        BOOST_LEAF_ASSERT(ctx.is_active());
        using R = decltype(std::declval<TryBlock>()());
        try
        {
            auto r = std::forward<TryBlock>(try_block)();
            unload_result(&r);
            return std::move(r);
        }
        catch( std::exception & ex )
        {
            ctx.deactivate();
            error_id id = detail::unpack_error_id(ex);
            return handle_error_<R>(ctx.tup(), error_info(id, &ex, ctx.template get<e_source_location>(id)), std::forward<H>(h)...,
                [&]() -> R
                {
                    ctx.unload(id);
                    throw;
                } );
        }
        catch(...)
        {
            ctx.deactivate();
            error_id id = current_error();
            return handle_error_<R>(ctx.tup(), error_info(id, nullptr, ctx.template get<e_source_location>(id)), std::forward<H>(h)...,
                [&]() -> R
                {
                    ctx.unload(id);
                    throw;
                } );
        }
    }
}

template <class TryBlock, class... H>
inline
typename std::decay<decltype(std::declval<TryBlock>()().value())>::type
try_handle_all( TryBlock && try_block, H && ... h )
{
    static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The return type of the try_block passed to try_handle_all must be registered with leaf::is_result_type");
    context_type_from_handlers<H...> ctx;
    auto active_context = activate_context(ctx);
    if( auto r = detail::try_catch_(ctx, std::forward<TryBlock>(try_block), std::forward<H>(h)...) )
        return std::move(r).value();
    else
    {
        BOOST_LEAF_ASSERT(ctx.is_active());
        detail::unload_result(&r);
        error_id id = r.error();
        ctx.deactivate();
        using R = typename std::decay<decltype(std::declval<TryBlock>()().value())>::type;
        return ctx.template handle_error<R>(std::move(id), std::forward<H>(h)...);
    }
}

template <class TryBlock, class... H>
BOOST_LEAF_ATTRIBUTE_NODISCARD inline
typename std::decay<decltype(std::declval<TryBlock>()())>::type
try_handle_some( TryBlock && try_block, H && ... h )
{
    static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The return type of the try_block passed to try_handle_some must be registered with leaf::is_result_type");
    context_type_from_handlers<H...> ctx;
    auto active_context = activate_context(ctx);
    if( auto r = detail::try_catch_(ctx, std::forward<TryBlock>(try_block), std::forward<H>(h)...) )
        return r;
    else if( ctx.is_active() )
    {
        detail::unload_result(&r);
        error_id id = r.error();
        ctx.deactivate();
        using R = typename std::decay<decltype(std::declval<TryBlock>()())>::type;
        auto rr = ctx.template handle_error<R>(std::move(id), std::forward<H>(h)...,
            [&r]()->R
            {
                return std::move(r);
            });
        if( !rr )
            ctx.unload(rr.error());
        return rr;
    }
    else
    {
        ctx.unload(r.error());
        return r;
    }
}

template <class TryBlock, class... H>
inline
decltype(std::declval<TryBlock>()())
try_catch( TryBlock && try_block, H && ... h )
{
    context_type_from_handlers<H...> ctx;
    auto active_context = activate_context(ctx);
    using R = decltype(std::declval<TryBlock>()());
    try
    {
        return std::forward<TryBlock>(try_block)();
    }
    catch( std::exception & ex )
    {
        ctx.deactivate();
        error_id id = detail::unpack_error_id(ex);
        return detail::handle_error_<R>(ctx.tup(), error_info(id, &ex, ctx.template get<e_source_location>(id)), std::forward<H>(h)...,
            [&]() -> R
            {
                ctx.unload(id);
                throw;
            } );
    }
    catch(...)
    {
        ctx.deactivate();
        error_id id = current_error();
        return detail::handle_error_<R>(ctx.tup(), error_info(id, nullptr, ctx.template get<e_source_location>(id)), std::forward<H>(h)...,
            [&]() -> R
            {
                ctx.unload(id);
                throw;
            } );
    }
}

#endif

#if BOOST_LEAF_CFG_CAPTURE

namespace detail
{
    template <class LeafResult>
    struct try_capture_all_dispatch_non_void
    {
        using leaf_result = LeafResult;

        template <class TryBlock>
        inline
        static
        leaf_result
        try_capture_all_( TryBlock && try_block ) noexcept
        {
            detail::slot<detail::dynamic_allocator> sl;
            sl.activate();
#ifndef BOOST_LEAF_NO_EXCEPTIONS
            try
#endif
            {
                if( leaf_result r = std::forward<TryBlock>(try_block)() )
                {
                    sl.deactivate();
                    return r;
                }
                else
                {
                    sl.deactivate();
                    int const err_id = error_id(r.error()).value();
                    return leaf_result(sl.value_or_default(err_id).template extract_capture_list<leaf_result>(err_id));
                }
            }
#ifndef BOOST_LEAF_NO_EXCEPTIONS
            catch( std::exception & ex )
            {
                sl.deactivate();
                int err_id = unpack_error_id(ex).value();
                return sl.value_or_default(err_id).template extract_capture_list<leaf_result>(err_id);
            }
            catch(...)
            {
                sl.deactivate();
                int err_id = current_error().value();
                return sl.value_or_default(err_id).template extract_capture_list<leaf_result>(err_id);
            }
#endif
        }
    };

    template <class R, bool IsVoid = std::is_same<void, R>::value, bool IsResultType = is_result_type<R>::value>
    struct try_capture_all_dispatch;

    template <class R>
    struct try_capture_all_dispatch<R, false, true>:
        try_capture_all_dispatch_non_void<::boost::leaf::result<typename std::decay<decltype(std::declval<R>().value())>::type>>
    {
    };

    template <class R>
    struct try_capture_all_dispatch<R, false, false>:
        try_capture_all_dispatch_non_void<::boost::leaf::result<typename std::remove_reference<R>::type>>
    {
    };

    template <class R>
    struct try_capture_all_dispatch<R, true, false>
    {
        using leaf_result = ::boost::leaf::result<R>;

        template <class TryBlock>
        inline
        static
        leaf_result
        try_capture_all_( TryBlock && try_block ) noexcept
        {
            detail::slot<detail::dynamic_allocator> sl;
            sl.activate();
#ifndef BOOST_LEAF_NO_EXCEPTIONS
            try
#endif
            {
                std::forward<TryBlock>(try_block)();
                return {};
            }
#ifndef BOOST_LEAF_NO_EXCEPTIONS
            catch( std::exception & ex )
            {
                sl.deactivate();
                int err_id = unpack_error_id(ex).value();
                return sl.value_or_default(err_id).template extract_capture_list<leaf_result>(err_id);
            }
            catch(...)
            {
                sl.deactivate();
                int err_id = current_error().value();
                return sl.value_or_default(err_id).template extract_capture_list<leaf_result>(err_id);
            }
#endif
        }
    };
}

template <class TryBlock>
inline
typename detail::try_capture_all_dispatch<decltype(std::declval<TryBlock>()())>::leaf_result
try_capture_all( TryBlock && try_block ) noexcept
{
    return detail::try_capture_all_dispatch<decltype(std::declval<TryBlock>()())>::try_capture_all_(std::forward<TryBlock>(try_block));
}
#endif

} }

// Boost Exception Integration

namespace boost { class exception; }
namespace boost { template <class Tag,class T> class error_info; }
namespace boost { namespace exception_detail { template <class ErrorInfo> struct get_info; } }

namespace boost { namespace leaf {

namespace detail
{
    template <class T>
    struct match_enum_type;

    template <class Tag, class T>
    struct match_enum_type<boost::error_info<Tag, T>>
    {
        using type = T;
    };

    template <class Ex>
    BOOST_LEAF_CONSTEXPR inline Ex * get_exception( error_info const & ei )
    {
        return dynamic_cast<Ex *>(ei.exception());
    }

    template <class, class T>
    struct dependent_type { using type = T; };

    template <class Dep, class T>
    using dependent_type_t = typename dependent_type<Dep, T>::type;

    template <class Tag, class T>
    struct handler_argument_traits<boost::error_info<Tag, T>>
    {
        using context_types = leaf_detail_mp11::mp_list<>;
        constexpr static bool always_available = false;

        template <class Tup>
        BOOST_LEAF_CONSTEXPR static T * check( Tup &, error_info const & ei ) noexcept
        {
            using boost_exception = dependent_type_t<T, boost::exception>;
            if( auto * be = get_exception<boost_exception>(ei) )
                return exception_detail::get_info<boost::error_info<Tag, T>>::get(*be);
            else
                return nullptr;
        }

        template <class Tup>
        BOOST_LEAF_CONSTEXPR static boost::error_info<Tag, T> get( Tup const & tup, error_info const & ei ) noexcept
        {
            return boost::error_info<Tag, T>(*check(tup, ei));
        }
    };

    template <class Tag, class T> struct handler_argument_traits<boost::error_info<Tag, T> const &>: handler_argument_traits_require_by_value<boost::error_info<Tag, T>> { };
    template <class Tag, class T> struct handler_argument_traits<boost::error_info<Tag, T> const *>: handler_argument_traits_require_by_value<boost::error_info<Tag, T>> { };
    template <class Tag, class T> struct handler_argument_traits<boost::error_info<Tag, T> &>: handler_argument_traits_require_by_value<boost::error_info<Tag, T>> { };
    template <class Tag, class T> struct handler_argument_traits<boost::error_info<Tag, T> *>: handler_argument_traits_require_by_value<boost::error_info<Tag, T>> { };
}

} }

#endif // BOOST_LEAF_HANDLE_ERRORS_HPP_INCLUDED
// <<< #include <boost/leaf/handle_errors.hpp>
// #line 11 "boost/leaf/diagnostics.hpp"

namespace boost { namespace leaf {

#if BOOST_LEAF_CFG_DIAGNOSTICS

class diagnostic_info: public error_info
{
    void const * tup_;
    void (*print_tuple_contents_)(std::ostream &, void const * tup, error_id to_print, char const * & prefix);

protected:

    diagnostic_info( diagnostic_info const & ) noexcept = default;

    template <class Tup>
    BOOST_LEAF_CONSTEXPR diagnostic_info( error_info const & ei, Tup const & tup ) noexcept:
        error_info(ei),
        tup_(&tup),
        print_tuple_contents_(&detail::print_tuple_contents<Tup>)
    {
    }

    template <class CharT, class Traits>
    void print_diagnostic_info(std::basic_ostream<CharT, Traits> & os) const
    {
        print_error_info(os);
        char const * prefix = exception() ? nullptr : "\nCaught:" BOOST_LEAF_CFG_DIAGNOSTICS_FIRST_DELIMITER;
        print_tuple_contents_(os, tup_, error(), prefix);
    }

    template <class CharT, class Traits>
    friend std::ostream & operator<<( std::basic_ostream<CharT, Traits> & os, diagnostic_info const & x )
    {
        x.print_diagnostic_info(os);
        return os << '\n';
    }
};

namespace detail
{
    struct diagnostic_info_: diagnostic_info
    {
        template <class Tup>
        BOOST_LEAF_CONSTEXPR diagnostic_info_( error_info const & ei, Tup const & tup ) noexcept:
            diagnostic_info(ei, tup)
        {
        }
    };

    template <>
    struct handler_argument_traits<diagnostic_info const &>: handler_argument_always_available<e_source_location>
    {
        template <class Tup>
        BOOST_LEAF_CONSTEXPR static diagnostic_info_ get( Tup const & tup, error_info const & ei ) noexcept
        {
            return diagnostic_info_(ei, tup);
        }
    };
}

#else

class diagnostic_info: public error_info
{
protected:

    diagnostic_info( diagnostic_info const & ) noexcept = default;

    BOOST_LEAF_CONSTEXPR diagnostic_info( error_info const & ei ) noexcept:
        error_info(ei)
    {
    }

    template <class CharT, class Traits>
    void print_diagnostic_info( std::basic_ostream<CharT, Traits> & os ) const
    {
        print_error_info(os);
        os << "\nboost::leaf::diagnostic_info N/A due to BOOST_LEAF_CFG_DIAGNOSTICS=0";
    }

    template <class CharT, class Traits>
    friend std::ostream & operator<<( std::basic_ostream<CharT, Traits> & os, diagnostic_info const & x )
    {
        x.print_diagnostic_info(os);
        return os << "\n";
    }
};

namespace detail
{
    struct diagnostic_info_: diagnostic_info
    {
        BOOST_LEAF_CONSTEXPR diagnostic_info_( error_info const & ei ) noexcept:
            diagnostic_info(ei)
        {
        }
    };

    template <>
    struct handler_argument_traits<diagnostic_info const &>: handler_argument_always_available<e_source_location>
    {
        template <class Tup>
        BOOST_LEAF_CONSTEXPR static diagnostic_info_ get( Tup const &, error_info const & ei ) noexcept
        {
            return diagnostic_info_(ei);
        }
    };
}

#endif

////////////////////////////////////////

#if BOOST_LEAF_CFG_DIAGNOSTICS

#if BOOST_LEAF_CFG_CAPTURE

class diagnostic_details: public diagnostic_info
{
    detail::dynamic_allocator const * const da_;

protected:

    diagnostic_details( diagnostic_details const & ) noexcept = default;

    template <class Tup>
    BOOST_LEAF_CONSTEXPR diagnostic_details( error_info const & ei, Tup const & tup, detail::dynamic_allocator const * da ) noexcept:
        diagnostic_info(ei, tup),
        da_(da)
    {
    }

    template <class CharT, class Traits>
    void print_diagnostic_details( std::basic_ostream<CharT, Traits> & os) const
    {
        print_diagnostic_info(os);
        if( da_ )
        {
            char const * prefix = "\nDiagnostic details:" BOOST_LEAF_CFG_DIAGNOSTICS_FIRST_DELIMITER;
            da_->print(os, error(), prefix);
        }
    }

    template <class CharT, class Traits>
    friend std::ostream & operator<<( std::basic_ostream<CharT, Traits> & os, diagnostic_details const & x )
    {
        x.print_diagnostic_details(os);
        return os << '\n';
    }
};

namespace detail
{
    struct diagnostic_details_: diagnostic_details
    {
        template <class Tup>
        BOOST_LEAF_CONSTEXPR diagnostic_details_( error_info const & ei, Tup const & tup, dynamic_allocator const * da ) noexcept:
            diagnostic_details(ei, tup, da)
        {
        }
    };

    template <>
    struct handler_argument_traits<diagnostic_details const &>: handler_argument_always_available<e_source_location, dynamic_allocator>
    {
        template <class Tup>
        BOOST_LEAF_CONSTEXPR static diagnostic_details_ get( Tup const & tup, error_info const & ei ) noexcept
        {
            slot<dynamic_allocator> const * da = find_in_tuple<slot<dynamic_allocator>>(tup);
            return diagnostic_details_(ei, tup, da ? da->has_value_any_key() : nullptr );
        }
    };
}

#else

class diagnostic_details: public diagnostic_info
{
protected:

    diagnostic_details( diagnostic_details const & ) noexcept = default;

    template <class Tup>
    BOOST_LEAF_CONSTEXPR diagnostic_details( error_info const & ei, Tup const & tup ) noexcept:
        diagnostic_info(ei, tup)
    {
    }

    template <class CharT, class Traits>
    void print_diagnostic_details( std::basic_ostream<CharT, Traits> & os ) const
    {
        print_diagnostic_info(os);
        os << "\nboost::leaf::diagnostic_details N/A due to BOOST_LEAF_CFG_CAPTURE=0";
    }

    template <class CharT, class Traits>
    friend std::ostream & operator<<( std::basic_ostream<CharT, Traits> & os, diagnostic_details const & x )
    {
        x.print_diagnostic_details(os);
        return os << "\n";
    }
};

namespace detail
{
    struct diagnostic_details_: diagnostic_details
    {
        template <class Tup>
        BOOST_LEAF_CONSTEXPR diagnostic_details_( error_info const & ei, Tup const & tup ) noexcept:
            diagnostic_details(ei, tup)
        {
        }
    };

    template <>
    struct handler_argument_traits<diagnostic_details const &>: handler_argument_always_available<e_source_location>
    {
        template <class Tup>
        BOOST_LEAF_CONSTEXPR static diagnostic_details_ get( Tup const & tup, error_info const & ei ) noexcept
        {
            return diagnostic_details_(ei, tup);
        }
    };
}

#endif

#else

class diagnostic_details: public diagnostic_info
{
protected:

    diagnostic_details( diagnostic_details const & ) noexcept = default;

    BOOST_LEAF_CONSTEXPR diagnostic_details( error_info const & ei ) noexcept:
        diagnostic_info(ei)
    {
    }

    template <class CharT, class Traits>
    void print_diagnostic_details( std::basic_ostream<CharT, Traits> & os ) const
    {
        print_error_info(os);
        os << "\nboost::leaf::diagnostic_details N/A due to BOOST_LEAF_CFG_DIAGNOSTICS=0";
    }

    template <class CharT, class Traits>
    friend std::ostream & operator<<( std::basic_ostream<CharT, Traits> & os, diagnostic_details const & x )
    {
        x.print_diagnostic_details(os);
        return os << "\n";
    }
};

namespace detail
{
    struct diagnostic_details_: diagnostic_details
    {
        BOOST_LEAF_CONSTEXPR diagnostic_details_( error_info const & ei ) noexcept:
            diagnostic_details(ei)
        {
        }
    };

    template <>
    struct handler_argument_traits<diagnostic_details const &>: handler_argument_always_available<e_source_location>
    {
        template <class Tup>
        BOOST_LEAF_CONSTEXPR static diagnostic_details_ get( Tup const &, error_info const & ei ) noexcept
        {
            return diagnostic_details_(ei);
        }
    };
}

#endif

using verbose_diagnostic_info = diagnostic_details;

} }

#endif // BOOST_LEAF_DIAGNOSTICS_HPP_INCLUDED
// #include <boost/leaf/error.hpp> // Expanded at line 971
// >>> #include <boost/leaf/exception.hpp>
#ifndef BOOST_LEAF_EXCEPTION_HPP_INCLUDED
#define BOOST_LEAF_EXCEPTION_HPP_INCLUDED

// #line 8 "boost/leaf/exception.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 14
// #include <boost/leaf/error.hpp> // Expanded at line 971
#include <exception>
#include <typeinfo>

#ifdef BOOST_LEAF_NO_EXCEPTIONS

namespace boost
{
    [[noreturn]] void throw_exception( std::exception const & ); // user defined
}

namespace boost { namespace leaf {

namespace detail
{
    template <class T>
    [[noreturn]] void throw_exception_impl( T && e )
    {
        ::boost::throw_exception(std::move(e));
    }
}

} }

#else

namespace boost { namespace leaf {

namespace detail
{
    template <class T>
    [[noreturn]] void throw_exception_impl( T && e )
    {
        throw std::move(e);
    }
}

} }

#endif

////////////////////////////////////////

#define BOOST_LEAF_THROW_EXCEPTION ::boost::leaf::detail::throw_with_loc{__FILE__,__LINE__,__FUNCTION__}+::boost::leaf::detail::make_exception

namespace boost { namespace leaf {

namespace detail
{
    struct throw_with_loc
    {
        char const * const file;
        int const line;
        char const * const fn;

        template <class Ex>
        [[noreturn]] friend void operator+( throw_with_loc loc, Ex && ex )
        {
            ex.load_source_location_(loc.file, loc.line, loc.fn);
            ::boost::leaf::detail::throw_exception_impl(std::move(ex));
        }
    };
}

////////////////////////////////////////

namespace detail
{
    inline void enforce_std_exception( std::exception const & ) noexcept { }

    template <class Ex>
    class BOOST_LEAF_SYMBOL_VISIBLE exception:
        public Ex,
        public exception_base,
        public error_id
    {
        mutable bool clear_current_error_;

        bool is_current_exception() const noexcept
        {
            return tls::read_uint<detail::tls_tag_id_factory_current_id>() == unsigned(error_id::value());
        }

        error_id get_error_id() const noexcept final override
        {
            clear_current_error_ = false;
            return *this;
        }

#if BOOST_LEAF_CFG_DIAGNOSTICS && !defined(BOOST_LEAF_NO_EXCEPTIONS)
        void print_type_name(std::ostream & os) const final override
        {
            detail::demangle_and_print(os, typeid(Ex).name());
        }
#endif

    public:

        exception( exception const & other ):
            Ex(other),
            exception_base(other),
            error_id(other),
            clear_current_error_(other.clear_current_error_)
        {
            other.clear_current_error_ = false;
        }

        exception( exception && other ) noexcept:
            Ex(std::move(other)),
            exception_base(std::move(other)),
            error_id(std::move(other)),
            clear_current_error_(std::move(other.clear_current_error_))
        {
            other.clear_current_error_ = false;
        }

        exception( error_id id, Ex const & ex ) noexcept:
            Ex(ex),
            error_id(id),
            clear_current_error_(true)
        {
            enforce_std_exception(*this);
        }

        exception( error_id id, Ex && ex ) noexcept:
            Ex(std::move(ex)),
            error_id(id),
            clear_current_error_(true)
        {
            enforce_std_exception(*this);
        }

        explicit exception( error_id id ) noexcept:
            error_id(id),
            clear_current_error_(true)
        {
            enforce_std_exception(*this);
        }

        ~exception() noexcept
        {
            if( clear_current_error_ && is_current_exception() )
                tls::write_uint<detail::tls_tag_id_factory_current_id>(0);
        }
    };

    template <class... T>
    struct at_least_one_derives_from_std_exception;

    template <>
    struct at_least_one_derives_from_std_exception<>: std::false_type { };

    template <class T, class... Rest>
    struct at_least_one_derives_from_std_exception<T, Rest...>
    {
        constexpr static const bool value = std::is_base_of<std::exception,typename std::remove_reference<T>::type>::value || at_least_one_derives_from_std_exception<Rest...>::value;
    };

    template <class Ex, class... E>
    inline
    typename std::enable_if<std::is_base_of<std::exception,typename std::remove_reference<Ex>::type>::value, exception<typename std::remove_reference<Ex>::type>>::type
    make_exception( error_id err, Ex && ex, E && ... e ) noexcept
    {
        static_assert(!at_least_one_derives_from_std_exception<E...>::value, "Error objects passed to leaf::exception may not derive from std::exception");
        return exception<typename std::remove_reference<Ex>::type>( err.load(std::forward<E>(e)...), std::forward<Ex>(ex) );
    }

    template <class E1, class... E>
    inline
    typename std::enable_if<!std::is_base_of<std::exception,typename std::remove_reference<E1>::type>::value, exception<std::exception>>::type
    make_exception( error_id err, E1 && car, E && ... cdr ) noexcept
    {
        static_assert(!at_least_one_derives_from_std_exception<E...>::value, "Error objects passed to leaf::exception may not derive from std::exception");
        return exception<std::exception>( err.load(std::forward<E1>(car), std::forward<E>(cdr)...) );
    }

    inline exception<std::exception> make_exception( error_id err ) noexcept
    {
        return exception<std::exception>(err);
    }

    template <class Ex, class... E>
    inline
    typename std::enable_if<std::is_base_of<std::exception,typename std::remove_reference<Ex>::type>::value, exception<typename std::remove_reference<Ex>::type>>::type
    make_exception( Ex && ex, E && ... e ) noexcept
    {
        static_assert(!at_least_one_derives_from_std_exception<E...>::value, "Error objects passed to leaf::exception may not derive from std::exception");
        return exception<typename std::remove_reference<Ex>::type>( new_error().load(std::forward<E>(e)...), std::forward<Ex>(ex) );
    }

    template <class E1, class... E>
    inline
    typename std::enable_if<!std::is_base_of<std::exception,typename std::remove_reference<E1>::type>::value, exception<std::exception>>::type
    make_exception( E1 && car, E && ... cdr ) noexcept
    {
        static_assert(!at_least_one_derives_from_std_exception<E...>::value, "Error objects passed to leaf::exception may not derive from std::exception");
        return exception<std::exception>( new_error().load(std::forward<E1>(car), std::forward<E>(cdr)...) );
    }

    inline exception<std::exception> make_exception() noexcept
    {
        return exception<std::exception>(leaf::new_error());
    }
}

template <class... E>
[[noreturn]] void throw_exception( E && ... e )
{
    // Warning: setting a breakpoint here will not intercept exceptions thrown
    // via BOOST_LEAF_THROW_EXCEPTION or originating in the few other throw
    // points elsewhere in LEAF. To intercept all of those exceptions as well,
    // set a breakpoint inside boost::leaf::detail::throw_exception_impl.
    detail::throw_exception_impl(detail::make_exception(std::forward<E>(e)...));
}

////////////////////////////////////////

#ifndef BOOST_LEAF_NO_EXCEPTIONS

template <class T>
class BOOST_LEAF_SYMBOL_VISIBLE result;

namespace detail
{
    inline error_id catch_exceptions_helper( std::exception const &, leaf_detail_mp11::mp_list<> )
    {
        return leaf::new_error(std::current_exception());
    }

    template <class Ex1, class... Ex>
    inline error_id catch_exceptions_helper( std::exception const & ex, leaf_detail_mp11::mp_list<Ex1,Ex...> )
    {
        if( Ex1 const * p = dynamic_cast<Ex1 const *>(&ex) )
            return catch_exceptions_helper(ex, leaf_detail_mp11::mp_list<Ex...>{ }).load(*p);
        else
            return catch_exceptions_helper(ex, leaf_detail_mp11::mp_list<Ex...>{ });
    }

    template <class T>
    struct deduce_exception_to_result_return_type_impl
    {
        using type = result<T>;
    };

    template <class T>
    struct deduce_exception_to_result_return_type_impl<result<T>>
    {
        using type = result<T>;
    };

    template <class T>
    using deduce_exception_to_result_return_type = typename deduce_exception_to_result_return_type_impl<T>::type;
}

template <class... Ex, class F>
inline
detail::deduce_exception_to_result_return_type<detail::fn_return_type<F>>
exception_to_result( F && f ) noexcept
{
    try
    {
        return std::forward<F>(f)();
    }
    catch( std::exception const & ex )
    {
        return detail::catch_exceptions_helper(ex, leaf_detail_mp11::mp_list<Ex...>());
    }
    catch(...)
    {
        return leaf::new_error(std::current_exception());
    }
}

#endif

} }

#endif // BOOST_LEAF_EXCEPTION_HPP_INCLUDED
// #include <boost/leaf/handle_errors.hpp> // Expanded at line 3098
// >>> #include <boost/leaf/on_error.hpp>
#ifndef BOOST_LEAF_ON_ERROR_HPP_INCLUDED
#define BOOST_LEAF_ON_ERROR_HPP_INCLUDED

// #line 8 "boost/leaf/on_error.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 14
// #include <boost/leaf/error.hpp> // Expanded at line 971

namespace boost { namespace leaf {

class error_monitor
{
#if !defined(BOOST_LEAF_NO_EXCEPTIONS) && BOOST_LEAF_STD_UNCAUGHT_EXCEPTIONS
    int const uncaught_exceptions_;
#endif
    int const err_id_;

public:

    error_monitor() noexcept:
#if !defined(BOOST_LEAF_NO_EXCEPTIONS) && BOOST_LEAF_STD_UNCAUGHT_EXCEPTIONS
        uncaught_exceptions_(std::uncaught_exceptions()),
#endif
        err_id_(detail::current_id())
    {
    }

    int check_id() const noexcept
    {
        int err_id = detail::current_id();
        if( err_id != err_id_ )
            return err_id;
        else
        {
#ifndef BOOST_LEAF_NO_EXCEPTIONS
#   if BOOST_LEAF_STD_UNCAUGHT_EXCEPTIONS
            if( std::uncaught_exceptions() > uncaught_exceptions_ )
#   else
            if( std::uncaught_exception() )
#   endif
                return detail::new_id();
#endif
            return 0;
        }
    }

    int get_id() const noexcept
    {
        int err_id = detail::current_id();
        if( err_id != err_id_ )
            return err_id;
        else
            return detail::new_id();
    }

    error_id check() const noexcept
    {
        return detail::make_error_id(check_id());
    }

    error_id assigned_error_id() const noexcept
    {
        return detail::make_error_id(get_id());
    }
};

////////////////////////////////////////

namespace detail
{
    template <int I, class Tup>
    struct tuple_for_each_preload
    {
        BOOST_LEAF_CONSTEXPR static void trigger( Tup & tup, int err_id ) noexcept
        {
            BOOST_LEAF_ASSERT((err_id&3) == 1);
            tuple_for_each_preload<I-1,Tup>::trigger(tup,err_id);
            std::get<I-1>(tup).trigger(err_id);
        }
    };

    template <class Tup>
    struct tuple_for_each_preload<0, Tup>
    {
        BOOST_LEAF_CONSTEXPR static void trigger( Tup const &, int ) noexcept { }
    };

    template <class E>
    class preloaded_item
    {
        using decay_E = typename std::decay<E>::type;
        decay_E e_;

    public:

        BOOST_LEAF_CONSTEXPR preloaded_item( E && e ):
            e_(std::forward<E>(e))
        {
        }

        BOOST_LEAF_CONSTEXPR void trigger( int err_id ) noexcept
        {
            (void) load_slot<true>(err_id, std::move(e_));
        }
    };

    template <class F, class ReturnType = typename function_traits<F>::return_type>
    class deferred_item
    {
        F f_;

    public:

        BOOST_LEAF_CONSTEXPR deferred_item( F && f ) noexcept:
            f_(std::forward<F>(f))
        {
        }

        BOOST_LEAF_CONSTEXPR void trigger( int err_id ) noexcept
        {
            (void) load_slot_deferred<true>(err_id, f_);
        }
    };

    template <class F>
    class deferred_item<F, void>
    {
        F f_;

    public:

        BOOST_LEAF_CONSTEXPR deferred_item( F && f ) noexcept:
            f_(std::forward<F>(f))
        {
        }

        BOOST_LEAF_CONSTEXPR void trigger( int ) noexcept
        {
            f_();
        }
    };

    template <class F, class A0 = fn_arg_type<F,0>, int arity = function_traits<F>::arity>
    class accumulating_item;

    template <class F, class A0>
    class accumulating_item<F, A0 &, 1>
    {
        F f_;

    public:

        BOOST_LEAF_CONSTEXPR accumulating_item( F && f ) noexcept:
            f_(std::forward<F>(f))
        {
        }

        BOOST_LEAF_CONSTEXPR void trigger( int err_id ) noexcept
        {
            load_slot_accumulate<true>(err_id, std::move(f_));
        }
    };

    template <class... Item>
    class preloaded
    {
        preloaded & operator=( preloaded const & ) = delete;

        std::tuple<Item...> p_;
        bool moved_;
        error_monitor id_;

    public:

        BOOST_LEAF_CONSTEXPR explicit preloaded( Item && ... i ):
            p_(std::forward<Item>(i)...),
            moved_(false)
        {
        }

        BOOST_LEAF_CONSTEXPR preloaded( preloaded && x ) noexcept:
            p_(std::move(x.p_)),
            moved_(false),
            id_(std::move(x.id_))
        {
            x.moved_ = true;
        }

        ~preloaded() noexcept
        {
            if( moved_ )
                return;
            if( auto id = id_.check_id() )
                tuple_for_each_preload<sizeof...(Item),decltype(p_)>::trigger(p_,id);
        }
    };

    template <class T, int arity = function_traits<T>::arity>
    struct deduce_item_type;

    template <class T>
    struct deduce_item_type<T, -1>
    {
        using type = preloaded_item<T>;
    };

    template <class F>
    struct deduce_item_type<F, 0>
    {
        using type = deferred_item<F>;
    };

    template <class F>
    struct deduce_item_type<F, 1>
    {
        using type = accumulating_item<F>;
    };
}

template <class... Item>
BOOST_LEAF_ATTRIBUTE_NODISCARD BOOST_LEAF_CONSTEXPR inline
detail::preloaded<typename detail::deduce_item_type<Item>::type...>
on_error( Item && ... i )
{
    return detail::preloaded<typename detail::deduce_item_type<Item>::type...>(std::forward<Item>(i)...);
}

} }

#endif // BOOST_LEAF_ON_ERROR_HPP_INCLUDED
// >>> #include <boost/leaf/pred.hpp>
#ifndef BOOST_LEAF_PRED_HPP_INCLUDED
#define BOOST_LEAF_PRED_HPP_INCLUDED

// #line 8 "boost/leaf/pred.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 14
// #include <boost/leaf/handle_errors.hpp> // Expanded at line 3098

#if __cplusplus >= 201703L
#   define BOOST_LEAF_MATCH_ARGS(et,v1,v) auto v1, auto... v
#else
#   define BOOST_LEAF_MATCH_ARGS(et,v1,v) typename detail::et::type v1, typename detail::et::type... v
#endif
#define BOOST_LEAF_ESC(...) __VA_ARGS__

namespace boost { namespace leaf {

namespace detail
{
#if __cplusplus >= 201703L
    template <class MatchType, class T>
    BOOST_LEAF_CONSTEXPR BOOST_LEAF_ALWAYS_INLINE bool cmp_value_pack( MatchType const & e, bool (*P)(T) noexcept ) noexcept
    {
        BOOST_LEAF_ASSERT(P != nullptr);
        return P(e);
    }

    template <class MatchType, class T>
    BOOST_LEAF_CONSTEXPR BOOST_LEAF_ALWAYS_INLINE bool cmp_value_pack( MatchType const & e, bool (*P)(T) )
    {
        BOOST_LEAF_ASSERT(P != nullptr);
        return P(e);
    }
#endif

    template <class MatchType, class V>
    BOOST_LEAF_CONSTEXPR BOOST_LEAF_ALWAYS_INLINE bool cmp_value_pack( MatchType const & e, V v )
    {
        return e == v;
    }

    template <class MatchType, class VCar, class... VCdr>
    BOOST_LEAF_CONSTEXPR BOOST_LEAF_ALWAYS_INLINE bool cmp_value_pack( MatchType const & e, VCar car, VCdr ... cdr )
    {
        return cmp_value_pack(e, car) || cmp_value_pack(e, cdr...);
    }
}

////////////////////////////////////////

#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
template <class E, class Enum = E>
struct condition
{
    static_assert(std::is_error_condition_enum<Enum>::value || std::is_error_code_enum<Enum>::value, "leaf::condition<E, Enum> requires Enum to be registered either with std::is_error_condition_enum or std::is_error_code_enum.");
};

template <class Enum>
struct condition<Enum, Enum>
{
    static_assert(std::is_error_condition_enum<Enum>::value || std::is_error_code_enum<Enum>::value, "leaf::condition<Enum> requires Enum to be registered either with std::is_error_condition_enum or std::is_error_code_enum.");
};

#if __cplusplus >= 201703L
template <class ErrorCodeEnum>
BOOST_LEAF_CONSTEXPR inline bool category( std::error_code const & ec )
{
    static_assert(std::is_error_code_enum<ErrorCodeEnum>::value, "leaf::category requires an error code enum");
    return &ec.category() == &std::error_code(ErrorCodeEnum{}).category();
}
#endif
#endif

////////////////////////////////////////

namespace detail
{
    template <class T>
    struct match_enum_type
    {
        using type = T;
    };

#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
    template <class Enum>
    struct match_enum_type<condition<Enum, Enum>>
    {
        using type = Enum;
    };

    template <class E, class Enum>
    struct match_enum_type<condition<E, Enum>>
    {
        static_assert(sizeof(Enum) == 0, "leaf::condition<E, Enum> should be used with leaf::match_value<>, not with leaf::match<>");
    };
#endif
}

template <class E, BOOST_LEAF_MATCH_ARGS(match_enum_type<E>, V1, V)>
struct match
{
    using error_type = E;
    E matched;

    template <class T>
    BOOST_LEAF_CONSTEXPR static bool evaluate(T && x)
    {
        return detail::cmp_value_pack(std::forward<T>(x), V1, V...);
    }
};

#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
template <class Enum, BOOST_LEAF_MATCH_ARGS(BOOST_LEAF_ESC(match_enum_type<condition<Enum, Enum>>), V1, V)>
struct match<condition<Enum, Enum>, V1, V...>
{
    using error_type = std::error_code;
    std::error_code const & matched;

    BOOST_LEAF_CONSTEXPR static bool evaluate(std::error_code const & e) noexcept
    {
        return detail::cmp_value_pack(e, V1, V...);
    }
};
#endif

template <class E, BOOST_LEAF_MATCH_ARGS(match_enum_type<E>, V1, V)>
struct is_predicate<match<E, V1, V...>>: std::true_type
{
};

////////////////////////////////////////

namespace detail
{
    template <class E>
    struct match_value_enum_type
    {
        using type = typename std::remove_reference<decltype(std::declval<E>().value)>::type;
    };

#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
    template <class E, class Enum>
    struct match_value_enum_type<condition<E, Enum>>
    {
        using type = Enum;
    };

    template <class Enum>
    struct match_value_enum_type<condition<Enum, Enum>>
    {
        static_assert(sizeof(Enum) == 0, "leaf::condition<Enum> should be used with leaf::match<>, not with leaf::match_value<>");
    };
#endif
}

template <class E, BOOST_LEAF_MATCH_ARGS(match_value_enum_type<E>, V1, V)>
struct match_value
{
    using error_type = E;
    E const & matched;

    BOOST_LEAF_CONSTEXPR static bool evaluate(E const & e) noexcept
    {
        return detail::cmp_value_pack(e.value, V1, V...);
    }
};

#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
template <class E, class Enum, BOOST_LEAF_MATCH_ARGS(BOOST_LEAF_ESC(match_value_enum_type<condition<E, Enum>>), V1, V)>
struct match_value<condition<E, Enum>, V1, V...>
{
    using error_type = E;
    E const & matched;

    BOOST_LEAF_CONSTEXPR static bool evaluate(E const & e)
    {
        return detail::cmp_value_pack(e.value, V1, V...);
    }
};
#endif

template <class E, BOOST_LEAF_MATCH_ARGS(match_value_enum_type<E>, V1, V)>
struct is_predicate<match_value<E, V1, V...>>: std::true_type
{
};

////////////////////////////////////////

#if __cplusplus >= 201703L
template <auto, auto, auto...>
struct match_member;

template <class T, class E, T E::* P, auto V1, auto... V>
struct match_member<P, V1, V...>
{
    using error_type = E;
    E const & matched;

    BOOST_LEAF_CONSTEXPR static bool evaluate(E const & e) noexcept
    {
        return detail::cmp_value_pack(e.*P, V1, V...);
    }
};

template <auto P, auto V1, auto... V>
struct is_predicate<match_member<P, V1, V...>>: std::true_type
{
};
#endif

////////////////////////////////////////

template <class P>
struct if_not
{
    using error_type = typename P::error_type;
    decltype(std::declval<P>().matched) matched;

    template <class E>
    BOOST_LEAF_CONSTEXPR static bool evaluate(E && e) noexcept
    {
        return !P::evaluate(std::forward<E>(e));
    }
};

template <class P>
struct is_predicate<if_not<P>>: std::true_type
{
};

////////////////////////////////////////


#ifndef BOOST_LEAF_NO_EXCEPTIONS

namespace detail
{
    template <class Ex>
    BOOST_LEAF_CONSTEXPR inline bool check_exception_pack( std::exception const & ex, Ex const * ) noexcept
    {
        return dynamic_cast<Ex const *>(&ex) != nullptr;
    }

    template <class Ex, class... ExRest>
    BOOST_LEAF_CONSTEXPR inline bool check_exception_pack( std::exception const & ex, Ex const *, ExRest const * ... ex_rest ) noexcept
    {
        return dynamic_cast<Ex const *>(&ex) != nullptr || check_exception_pack(ex, ex_rest...);
    }

    BOOST_LEAF_CONSTEXPR inline bool check_exception_pack( std::exception const & ) noexcept
    {
        return true;
    }
}

template <class... Ex>
struct catch_
{
    using error_type = void;
    std::exception const & matched;

    BOOST_LEAF_CONSTEXPR static bool evaluate(std::exception const & ex) noexcept
    {
        return detail::check_exception_pack(ex, static_cast<Ex const *>(nullptr)...);
    }
};

template <class Ex>
struct catch_<Ex>
{
    using error_type = void;
    Ex const & matched;

    BOOST_LEAF_CONSTEXPR static Ex const * evaluate(std::exception const & ex) noexcept
    {
        return dynamic_cast<Ex const *>(&ex);
    }

    explicit catch_( std::exception const & ex ):
        matched(*dynamic_cast<Ex const *>(&ex))
    {
    }
};

template <class... Ex>
struct is_predicate<catch_<Ex...>>: std::true_type
{
};

#endif

} }

#endif // BOOST_LEAF_PRED_HPP_INCLUDED
// >>> #include <boost/leaf/result.hpp>
#ifndef BOOST_LEAF_RESULT_HPP_INCLUDED
#define BOOST_LEAF_RESULT_HPP_INCLUDED

// #line 8 "boost/leaf/result.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 14
// #include <boost/leaf/detail/print.hpp> // Expanded at line 1582
// #include <boost/leaf/detail/capture_list.hpp> // Expanded at line 1576
// #include <boost/leaf/exception.hpp> // Expanded at line 4264

#include <climits>
#include <functional>

namespace boost { namespace leaf {

namespace detail { class dynamic_allocator; }

////////////////////////////////////////

class bad_result:
    public std::exception
{
    char const * what() const noexcept final override
    {
        return "boost::leaf::bad_result";
    }
};

////////////////////////////////////////

namespace detail
{
    template <class T, bool Printable = is_printable<T>::value>
    struct result_value_printer;

    template <class T>
    struct result_value_printer<T, true>
    {
        template <class CharT, class Traits>
        static void print( std::basic_ostream<CharT, Traits> & s, T const & x )
        {
            (void) (s << x);
        }
    };

    template <class T>
    struct result_value_printer<T, false>
    {
        template <class CharT, class Traits>
        static void print( std::basic_ostream<CharT, Traits> & s, T const & )
        {
            (void) (s << "{not printable}");
        }
    };

    template <class CharT, class Traits, class T>
    void print_result_value( std::basic_ostream<CharT, Traits> & s, T const & x )
    {
        result_value_printer<T>::print(s, x);
    }
}

////////////////////////////////////////

namespace detail
{
    template <class T>
    struct stored
    {
        using type = T;
        using value_no_ref = T;
        using value_no_ref_const = T const;
        using value_cref = T const &;
        using value_ref = T &;
        using value_rv_cref = T const &&;
        using value_rv_ref = T &&;

        static value_no_ref_const * cptr( type const & v ) noexcept
        {
            return &v;
        }

        static value_no_ref * ptr( type & v ) noexcept
        {
            return &v;
        }
    };

    template <class T>
    struct stored<T &>
    {
        using type = std::reference_wrapper<T>;
        using value_no_ref = T;
        using value_no_ref_const = T;
        using value_ref = T &;
        using value_cref = T &;
        using value_rv_ref = T &;
        using value_rv_cref = T &;

        static value_no_ref_const * cptr( type const & v ) noexcept
        {
            return &v.get();
        }

        static value_no_ref * ptr( type const & v ) noexcept
        {
            return &v.get();
        }
    };

    class result_discriminant
    {
        int state_;

    public:

        enum kind_t
        {
            err_id_zero = 0,
            err_id = 1,
            err_id_capture_list = 2,
            val = 3
        };

        explicit result_discriminant( error_id id ) noexcept:
            state_(id.value())
        {
            BOOST_LEAF_ASSERT(state_ == 0 || (state_&3) == 1);
            BOOST_LEAF_ASSERT(kind() == err_id_zero || kind() == err_id);
        }

#if BOOST_LEAF_CFG_CAPTURE
        explicit result_discriminant( int err_id, detail::capture_list const & ) noexcept:
            state_((err_id&~3) | 2)
        {
            BOOST_LEAF_ASSERT((err_id&3) == 1);
            BOOST_LEAF_ASSERT(kind() == err_id_capture_list);
        }
#endif

        struct kind_val { };
        explicit result_discriminant( kind_val ) noexcept:
            state_(val)
        {
            BOOST_LEAF_ASSERT((state_&3) == 3);
            BOOST_LEAF_ASSERT(kind() == val);
        }

        kind_t kind() const noexcept
        {
            return kind_t(state_&3);
        }

        error_id get_error_id() const noexcept
        {
            BOOST_LEAF_ASSERT(kind() == err_id_zero || kind() == err_id || kind() == err_id_capture_list);
            return make_error_id(int((state_&~3)|1));
        }
    };
}

////////////////////////////////////////

template <class T>
class BOOST_LEAF_SYMBOL_VISIBLE BOOST_LEAF_ATTRIBUTE_NODISCARD result
{
    template <class U>
    friend class result;

    friend class detail::dynamic_allocator;

#if BOOST_LEAF_CFG_CAPTURE
    using capture_list = detail::capture_list;
#endif

    using result_discriminant = detail::result_discriminant;
    using stored_type = typename detail::stored<T>::type;
    using value_no_ref = typename detail::stored<T>::value_no_ref;
    using value_no_ref_const = typename detail::stored<T>::value_no_ref_const;
    using value_ref = typename detail::stored<T>::value_ref;
    using value_cref = typename detail::stored<T>::value_cref;
    using value_rv_ref = typename detail::stored<T>::value_rv_ref;
    using value_rv_cref = typename detail::stored<T>::value_rv_cref;

    union
    {
        stored_type stored_;
#if BOOST_LEAF_CFG_CAPTURE
        mutable capture_list cap_;
#endif
    };

    result_discriminant what_;

    struct error_result
    {
        error_result( error_result && ) = default;
        error_result( error_result const & ) = delete;
        error_result & operator=( error_result const & ) = delete;

        result & r_;

        error_result( result & r ) noexcept:
            r_(r)
        {
        }

        template <class U>
        operator result<U>() noexcept
        {
            result_discriminant const what = r_.what_;
            switch(auto k = what.kind())
            {
                case result_discriminant::val:
                    return result<U>(error_id());
                case result_discriminant::err_id_capture_list:
#if BOOST_LEAF_CFG_CAPTURE
                    return result<U>(what.get_error_id().value(), std::move(r_.cap_));
#else
                    BOOST_LEAF_ASSERT(0); // Possible ODR violation.
#endif
                default:
                    BOOST_LEAF_ASSERT(k == result_discriminant::err_id);
                case result_discriminant::err_id_zero:
                    return result<U>(what.get_error_id());
            }
        }

        operator error_id() noexcept
        {
            result_discriminant const what = r_.what_;
            return what.kind() == result_discriminant::val? error_id() : what.get_error_id();
        }
    };

    void destroy() const noexcept
    {
        switch(auto k = this->what_.kind())
        {
        default:
            BOOST_LEAF_ASSERT(k == result_discriminant::err_id);
        case result_discriminant::err_id_zero:
            break;
        case result_discriminant::err_id_capture_list:
#if BOOST_LEAF_CFG_CAPTURE
            cap_.~capture_list();
#else
            BOOST_LEAF_ASSERT(0); // Possible ODR violation.
#endif
            break;
        case result_discriminant::val:
            stored_.~stored_type();
        }
    }

    template <class U>
    result_discriminant move_from( result<U> && x ) noexcept
    {
        auto x_what = x.what_;
        switch(auto k = x_what.kind())
        {
        default:
            BOOST_LEAF_ASSERT(k == result_discriminant::err_id);
        case result_discriminant::err_id_zero:
            break;
        case result_discriminant::err_id_capture_list:
#if BOOST_LEAF_CFG_CAPTURE
            (void) new(&cap_) capture_list(std::move(x.cap_));
#else
            BOOST_LEAF_ASSERT(0); // Possible ODR violation.
#endif
            break;
        case result_discriminant::val:
            (void) new(&stored_) stored_type(std::move(x.stored_));
        }
        return x_what;
    }

    error_id get_error_id() const noexcept
    {
        BOOST_LEAF_ASSERT(what_.kind() != result_discriminant::val);
        return what_.get_error_id();
    }

    stored_type const * get() const noexcept
    {
        return has_value() ? &stored_ : nullptr;
    }

    stored_type * get() noexcept
    {
        return has_value() ? &stored_ : nullptr;
    }

protected:

#if BOOST_LEAF_CFG_CAPTURE
    result( int err_id, detail::capture_list && cap ) noexcept:
        cap_(std::move(cap)),
        what_(err_id, cap)
    {
    }
#endif

    void enforce_value_state() const
    {
        switch( what_.kind() )
        {
        case result_discriminant::err_id_capture_list:
#if BOOST_LEAF_CFG_CAPTURE
            cap_.unload(what_.get_error_id().value());
#else
            BOOST_LEAF_ASSERT(0); // Possible ODR violation.
#endif
        case result_discriminant::err_id_zero:
        case result_discriminant::err_id:
            throw_exception(get_error_id(), bad_result{});
        case result_discriminant::val:
            break;
        }
    }

    template <class U>
    void move_assign( result<U> && x ) noexcept
    {
        destroy();
        what_ = move_from(std::move(x));
    }

    template <class CharT, class Traits>
    void print_error_result(std::basic_ostream<CharT, Traits> & os) const
    {
        result_discriminant const what = what_;
        BOOST_LEAF_ASSERT(what.kind() != result_discriminant::val);
        error_id const err_id = what.get_error_id();
        os << "Error serial #" << err_id;
        if( what.kind() == result_discriminant::err_id_capture_list )
        {
#if BOOST_LEAF_CFG_CAPTURE
            char const * prefix = "\nCaptured:";
            cap_.print(os, err_id, prefix);
            os << "\n";
#else
            BOOST_LEAF_ASSERT(0); // Possible ODR violation.
#endif
        }
    }

public:

    using value_type = T;

    // NOTE: Copy constructor implicitly deleted.

    result( result && x ) noexcept:
        what_(move_from(std::move(x)))
    {
    }

    template <class U, class = typename std::enable_if<std::is_convertible<U, T>::value>::type>
    result( result<U> && x ) noexcept:
        what_(move_from(std::move(x)))
    {
    }

    result():
        stored_(stored_type()),
        what_(result_discriminant::kind_val{})
    {
    }

    result( value_no_ref && v ) noexcept:
        stored_(std::forward<value_no_ref>(v)),
        what_(result_discriminant::kind_val{})
    {
    }

    result( value_no_ref const & v ):
        stored_(v),
        what_(result_discriminant::kind_val{})
    {
    }

    template<class... A, class = typename std::enable_if<std::is_constructible<T, A...>::value && sizeof...(A) >= 2>::type>
    result( A && ... a ) noexcept:
        stored_(std::forward<A>(a)...),
        what_(result_discriminant::kind_val{})
    {
    }

    result( error_id err ) noexcept:
        what_(err)
    {
    }

#if defined(BOOST_STRICT_CONFIG) || !defined(__clang__)

    // This should be the default implementation, but std::is_convertible
    // breaks under COMPILER=/usr/bin/clang++ CXXSTD=11 clang 3.3.
    // On the other hand, the workaround exposes a rather severe bug in
    //__GNUC__ under 11: https://github.com/boostorg/leaf/issues/25.

    // SFINAE: T can be initialized with an A, e.g. result<std::string>("literal").
    template<class A, class = typename std::enable_if<std::is_constructible<T, A>::value && std::is_convertible<A, T>::value>::type>
    result( A && a ) noexcept:
        stored_(std::forward<A>(a)),
        what_(result_discriminant::kind_val{})
    {
    }

#else

private:
    static int init_T_with_A( T && );
public:

    // SFINAE: T can be initialized with an A, e.g. result<std::string>("literal").
    template <class A>
    result( A && a, decltype(init_T_with_A(std::forward<A>(a))) * = nullptr ):
        stored_(std::forward<A>(a)),
        what_(result_discriminant::kind_val{})
    {
    }

#endif

#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
    result( std::error_code const & ec ) noexcept:
        what_(error_id(ec))
    {
    }

    template <class Enum, class = typename std::enable_if<std::is_error_code_enum<Enum>::value, int>::type>
    result( Enum e ) noexcept:
        what_(error_id(e))
    {
    }
#endif

    ~result() noexcept
    {
        destroy();
    }

    // NOTE: Assignment operator implicitly deleted.

    result & operator=( result && x ) noexcept
    {
        move_assign(std::move(x));
        return *this;
    }

    template <class U, class = typename std::enable_if<std::is_convertible<U, T>::value>::type>
    result & operator=( result<U> && x ) noexcept
    {
        move_assign(std::move(x));
        return *this;
    }

    bool has_value() const noexcept
    {
        return what_.kind() == result_discriminant::val;
    }

    bool has_error() const noexcept
    {
        return !has_value();
    }

    explicit operator bool() const noexcept
    {
        return has_value();
    }

#ifdef BOOST_LEAF_NO_CXX11_REF_QUALIFIERS

    value_cref value() const
    {
        enforce_value_state();
        return stored_;
    }

    value_ref value()
    {
        enforce_value_state();
        return stored_;
    }

#else

    value_cref value() const &
    {
        enforce_value_state();
        return stored_;
    }

    value_ref value() &
    {
        enforce_value_state();
        return stored_;
    }

    value_rv_cref value() const &&
    {
        enforce_value_state();
        return std::move(stored_);
    }

    value_rv_ref value() &&
    {
        enforce_value_state();
        return std::move(stored_);
    }

#endif

    value_no_ref_const * operator->() const noexcept
    {
        return has_value() ? detail::stored<T>::cptr(stored_) : nullptr;
    }

    value_no_ref * operator->() noexcept
    {
        return has_value() ? detail::stored<T>::ptr(stored_) : nullptr;
    }

#ifdef BOOST_LEAF_NO_CXX11_REF_QUALIFIERS

    value_cref operator*() const noexcept
    {
        auto p = get();
        BOOST_LEAF_ASSERT(p != nullptr);
        return *p;
    }

    value_ref operator*() noexcept
    {
        auto p = get();
        BOOST_LEAF_ASSERT(p != nullptr);
        return *p;
    }

#else

    value_cref operator*() const & noexcept
    {
        auto p = get();
        BOOST_LEAF_ASSERT(p != nullptr);
        return *p;
    }

    value_ref operator*() & noexcept
    {
        auto p = get();
        BOOST_LEAF_ASSERT(p != nullptr);
        return *p;
    }

    value_rv_cref operator*() const && noexcept
    {
        auto p = get();
        BOOST_LEAF_ASSERT(p != nullptr);
        return std::move(*p);
    }

    value_rv_ref operator*() && noexcept
    {
        auto p = get();
        BOOST_LEAF_ASSERT(p != nullptr);
        return std::move(*p);
    }

#endif

    error_result error() noexcept
    {
        return error_result{*this};
    }

    template <class... Item>
    error_id load( Item && ... item ) noexcept
    {
        return error_id(error()).load(std::forward<Item>(item)...);
    }

    void unload()
    {
#if BOOST_LEAF_CFG_CAPTURE
        if( what_.kind() == result_discriminant::err_id_capture_list )
            cap_.unload(what_.get_error_id().value());
#endif
    }

    template <class CharT, class Traits>
    friend std::ostream & operator<<( std::basic_ostream<CharT, Traits> & os, result const & r )
    {
        if( r.what_.kind() == result_discriminant::val )
            detail::print_result_value(os, r.value());
        else
            r.print_error_result(os);
        return os;
    }
};

////////////////////////////////////////

namespace detail
{
    struct void_ { };
}

template <>
class BOOST_LEAF_SYMBOL_VISIBLE BOOST_LEAF_ATTRIBUTE_NODISCARD result<void>:
    result<detail::void_>
{
    template <class U>
    friend class result;

    friend class detail::dynamic_allocator;

    using result_discriminant = detail::result_discriminant;
    using void_ = detail::void_;
    using base = result<void_>;

#if BOOST_LEAF_CFG_CAPTURE
    result( int err_id, detail::capture_list && cap ) noexcept:
        base(err_id, std::move(cap))
    {
    }
#endif

public:

    using value_type = void;

    // NOTE: Copy constructor implicitly deleted.
    result( result && x ) noexcept:
        base(std::move(x))
    {
    }

    result() noexcept
    {
    }

    result( error_id err ) noexcept:
        base(err)
    {
    }

#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
    result( std::error_code const & ec ) noexcept:
        base(ec)
    {
    }

    template <class Enum, class = typename std::enable_if<std::is_error_code_enum<Enum>::value, int>::type>
    result( Enum e ) noexcept:
        base(e)
    {
    }
#endif

    ~result() noexcept
    {
    }

    // NOTE: Assignment operator implicitly deleted.
    result & operator=( result && x ) noexcept
    {
        base::move_assign(std::move(x));
        return *this;
    }

    void value() const
    {
        base::enforce_value_state();
    }

    void const * operator->() const noexcept
    {
        return base::operator->();
    }

    void * operator->() noexcept
    {
        return base::operator->();
    }

    void operator*() const noexcept
    {
        BOOST_LEAF_ASSERT(has_value());
    }

    template <class CharT, class Traits>
    friend std::ostream & operator<<( std::basic_ostream<CharT, Traits> & os, result const & r )
    {
        if( r )
            os << "No error";
        else
            r.print_error_result(os);
        return os;
    }

    using base::operator=;
    using base::operator bool;
    using base::get_error_id;
    using base::error;
    using base::load;
    using base::unload;
};

////////////////////////////////////////

template <class R>
struct is_result_type;

template <class T>
struct is_result_type<result<T>>: std::true_type
{
};

} }

#endif // BOOST_LEAF_RESULT_HPP_INCLUDED
// >>> #include <boost/leaf/to_variant.hpp>
#ifndef BOOST_LEAF_TO_VARIANT_HPP_INCLUDED
#define BOOST_LEAF_TO_VARIANT_HPP_INCLUDED

// #line 8 "boost/leaf/to_variant.hpp"
#if __cplusplus >= 201703L

// #include <boost/leaf/config.hpp> // Expanded at line 14
// #include <boost/leaf/handle_errors.hpp> // Expanded at line 3098
// #include <boost/leaf/result.hpp> // Expanded at line 5073
#include <variant>
#include <optional>
#include <tuple>

namespace boost { namespace leaf {

template <class... E, class TryBlock>
std::variant<typename std::decay<decltype(std::declval<TryBlock>()().value())>::type,std::tuple<std::optional<E>...>>
to_variant( TryBlock && try_block )
{
    static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The return type of the try_block passed to a to_variant function must be registered with leaf::is_result_type");
    using T = typename std::decay<decltype(std::declval<TryBlock>()().value())>::type;
    using error_tuple_type = std::tuple<std::optional<E>...>;
    using variant_type = std::variant<T, error_tuple_type>;
    return try_handle_all(
        [&]() -> result<variant_type>
        {
            if( auto r = std::forward<TryBlock>(try_block)() )
                return *std::move(r);
            else
                return r.error();
        },
        []( E const * ... e ) -> variant_type
        {
            return error_tuple_type { e ? std::optional<E>(*e) : std::optional<E>{}... };
        },
        []() -> variant_type
        {
            return error_tuple_type { };
        } );
}

} }

#endif

#endif // BOOST_LEAF_TO_VARIANT_HPP_INCLUDED

#endif // BOOST_LEAF_HPP_INCLUDED
