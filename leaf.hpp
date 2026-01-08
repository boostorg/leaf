#ifndef BOOST_LEAF_HPP_INCLUDED
#define BOOST_LEAF_HPP_INCLUDED

// Boost LEAF single header distribution. Do not edit.
// Generated on Jan 08, 2026 from https://github.com/boostorg/leaf/tree/5fde62d.

// Latest published version of this file: https://raw.githubusercontent.com/boostorg/leaf/gh-pages/leaf.hpp.

// Copyright 2018-2025 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// >>> #include <boost/leaf/common.hpp>
#ifndef BOOST_LEAF_COMMON_HPP_INCLUDED
#define BOOST_LEAF_COMMON_HPP_INCLUDED

// #line 8 "boost/leaf/common.hpp"
// >>> #include <boost/leaf/config.hpp>
#ifndef BOOST_LEAF_CONFIG_HPP_INCLUDED
#define BOOST_LEAF_CONFIG_HPP_INCLUDED

// #line 8 "boost/leaf/config.hpp"
#include <utility>
#include <exception>

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
#endif // #ifdef BOOST_LEAF_EMBEDDED

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

#if BOOST_LEAF_CFG_WIN32 != 0 && BOOST_LEAF_CFG_WIN32 != 1 && BOOST_LEAF_CFG_WIN32 != 2
#   error BOOST_LEAF_CFG_WIN32 must be 0 or 1 or 2.
#endif

#if BOOST_LEAF_CFG_WIN32 && !defined(_WIN32)
#   warning "Ignoring BOOST_LEAF_CFG_WIN32 because _WIN32 is not defined"
#   define BOOST_LEAF_CFG_WIN32 0
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
#endif // #ifndef BOOST_LEAF_NO_EXCEPTIONS

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

// >>> #include <boost/leaf/config/visibility.hpp>
#ifndef BOOST_LEAF_CONFIG_VISIBILITY_HPP_INCLUDED
#define BOOST_LEAF_CONFIG_VISIBILITY_HPP_INCLUDED

// #line 8 "boost/leaf/config/visibility.hpp"
namespace boost { namespace leaf {

class BOOST_LEAF_SYMBOL_VISIBLE error_id;
class BOOST_LEAF_SYMBOL_VISIBLE error_info;
class BOOST_LEAF_SYMBOL_VISIBLE diagnostic_info;
class BOOST_LEAF_SYMBOL_VISIBLE diagnostic_details;

struct BOOST_LEAF_SYMBOL_VISIBLE e_api_function;
struct BOOST_LEAF_SYMBOL_VISIBLE e_file_name;
struct BOOST_LEAF_SYMBOL_VISIBLE e_errno;
struct BOOST_LEAF_SYMBOL_VISIBLE e_type_info_name;
struct BOOST_LEAF_SYMBOL_VISIBLE e_at_line;
struct BOOST_LEAF_SYMBOL_VISIBLE e_source_location;

class BOOST_LEAF_SYMBOL_VISIBLE bad_result;
template <class> class BOOST_LEAF_SYMBOL_VISIBLE result;

namespace detail
{
    template <class> class BOOST_LEAF_SYMBOL_VISIBLE slot;

    class BOOST_LEAF_SYMBOL_VISIBLE exception_base;

    template <class> class BOOST_LEAF_SYMBOL_VISIBLE exception;

#if BOOST_LEAF_CFG_CAPTURE
    class BOOST_LEAF_SYMBOL_VISIBLE dynamic_allocator;
#endif

#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
    class BOOST_LEAF_SYMBOL_VISIBLE leaf_error_category;
    template <class> struct BOOST_LEAF_SYMBOL_VISIBLE get_leaf_error_category;
#endif
}

} } // namespace boost::leaf

#endif // BOOST_LEAF_CONFIG_VISIBILITY_HPP_INCLUDED
// <<< #include <boost/leaf/config/visibility.hpp>
// #line 229 "boost/leaf/config.hpp"

////////////////////////////////////////

#if defined(__GNUC__) && !(defined(__clang__) || defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC) || defined(__ECC)) && (__GNUC__ * 100 + __GNUC_MINOR__) < 409
#   ifndef BOOST_LEAF_NO_CXX11_REF_QUALIFIERS
#       define BOOST_LEAF_NO_CXX11_REF_QUALIFIERS
#   endif
#endif

////////////////////////////////////////

#ifdef _MSC_VER
#   define BOOST_LEAF_UNREACHABLE __assume(0)
#else
#   define BOOST_LEAF_UNREACHABLE __builtin_unreachable()
#endif

////////////////////////////////////////

namespace boost
{

#ifdef BOOST_LEAF_NO_EXCEPTIONS

[[noreturn]] void throw_exception( std::exception const & ); // user defined

namespace leaf
{
    template <class T>
    [[noreturn]] void throw_exception_( T && e )
    {
        ::boost::throw_exception(std::move(e));
    }
}

#else

namespace leaf
{
    template <class T>
    [[noreturn]] void throw_exception_( T && e )
    {
        throw std::move(e);
    }
}

#endif

} // namespace boost

////////////////////////////////////////

// Configure TLS access
// >>> #include <boost/leaf/config/tls.hpp>
#ifndef BOOST_LEAF_CONFIG_TLS_HPP_INCLUDED
#define BOOST_LEAF_CONFIG_TLS_HPP_INCLUDED

// #line 8 "boost/leaf/config/tls.hpp"
namespace boost { namespace leaf {

// The following declarations specify the thread local storage API used
// internally by LEAF. To port LEAF to a new TLS API, provide definitions for
// each of these functions.
namespace tls
{
    // Generate the next unique error_id. Values start at 1 and increment by 4.
    // Error ids must be unique for the lifetime of the process, and this
    // function must be thread-safe. Postcondition: (id & 3) == 1 && id != 0.
    //
    // This function may not fail.
    unsigned generate_next_error_id() noexcept;

    // Write x to the TLS for the current error_id. The initial value for each
    // thread must be 0. Precondition: x == 0 or (x & 3) == 1.
    //
    // This function may not fail.
    void write_current_error_id( unsigned x ) noexcept;

    // Read the current error_id for this thread. The initial value for each
    // thread must be 0.
    //
    // This function may not fail.
    unsigned read_current_error_id() noexcept;

    // Reserve TLS storage for T. The TLS may be allocated dynamically on the
    // first call to reserve_ptr<T>, but subsequent calls must reuse the same
    // TLS. On platforms where allocation is not needed, this function is
    // still defined but does nothing.
    //
    // This function may throw on allocation failure.
    template <class T>
    void reserve_ptr();

    // Write p to the TLS previously reserved for T by a call to reserve_ptr<T>.
    // It is illegal to call write_ptr<T> without a prior successful call to
    // reserve_ptr<T>.
    //
    // This function may not fail.
    template <class T>
    void write_ptr( T * p ) noexcept;

    // Read the T * value previously written in the TLS for T. Returns nullptr
    // if TLS for T has not yet been reserved.
    //
    // This function may not fail.
    template <class T>
    T * read_ptr() noexcept;
} // namespace tls

} } // namespace boost::leaf

#if defined(BOOST_LEAF_TLS_FREERTOS)
// >>> #   include <boost/leaf/config/tls_freertos.hpp>
#ifndef BOOST_LEAF_CONFIG_TLS_FREERTOS_HPP_INCLUDED
#define BOOST_LEAF_CONFIG_TLS_FREERTOS_HPP_INCLUDED

// #line 8 "boost/leaf/config/tls_freertos.hpp"
// Copyright (c) 2022 Khalil Estell

// This header implements the TLS API specified in tls.hpp via the FreeTOS
// pvTaskGetThreadLocalStoragePointer / pvTaskSetThreadLocalStoragePointer
// functions, using the more general implementation defined in tls_array.hpp.

#include <task.h>

#ifndef BOOST_LEAF_CFG_TLS_ARRAY_SIZE
#   define BOOST_LEAF_CFG_TLS_ARRAY_SIZE configNUM_THREAD_LOCAL_STORAGE_POINTERS
#endif

static_assert((BOOST_LEAF_CFG_TLS_ARRAY_SIZE) <= configNUM_THREAD_LOCAL_STORAGE_POINTERS,
    "Bad BOOST_LEAF_CFG_TLS_ARRAY_SIZE");

namespace boost { namespace leaf {

namespace tls
{
    // See https://www.freertos.org/thread-local-storage-pointers.html.

    BOOST_LEAF_ALWAYS_INLINE void * read_void_ptr( int tls_index ) noexcept
    {
        return pvTaskGetThreadLocalStoragePointer(0, tls_index);
    }

    BOOST_LEAF_ALWAYS_INLINE void write_void_ptr( int tls_index, void * p ) noexcept
    {
        vTaskSetThreadLocalStoragePointer(0, tls_index, p);
    }
}

} }

#endif // #ifndef BOOST_LEAF_CONFIG_TLS_FREERTOS_HPP_INCLUDED
// <<< #   include <boost/leaf/config/tls_freertos.hpp>
// #line 63 "boost/leaf/config/tls.hpp"
#   ifndef BOOST_LEAF_USE_TLS_ARRAY
#       define BOOST_LEAF_USE_TLS_ARRAY
#   endif
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

// This header implements the TLS API specified in tls.hpp for platforms that
// provide TLS by indexing an array (this is typical for embedded platforms).
// The array is accessed via user-defined functions.

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

#include <atomic>
#include <limits>
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

    template <class=void>
    struct BOOST_LEAF_SYMBOL_VISIBLE id_factory
    {
        static atomic_unsigned_int counter;
    };

    template <class T>
    atomic_unsigned_int id_factory<T>::counter(1);

    template <class=void>
    class BOOST_LEAF_SYMBOL_VISIBLE index_counter
    {
        static int c_;

        BOOST_LEAF_ALWAYS_INLINE static BOOST_LEAF_CFG_TLS_INDEX_TYPE next_() noexcept
        {
            int idx = ++c_;
            BOOST_LEAF_ASSERT(idx > (BOOST_LEAF_CFG_TLS_ARRAY_START_INDEX + 1));
            BOOST_LEAF_ASSERT(idx < (BOOST_LEAF_CFG_TLS_ARRAY_SIZE));
            return idx;
        }

    public:

        template <class T>
        BOOST_LEAF_ALWAYS_INLINE static BOOST_LEAF_CFG_TLS_INDEX_TYPE next() noexcept
        {
            return next_();
        }
    };

    template <class T>
    int index_counter<T>::c_ = BOOST_LEAF_CFG_TLS_ARRAY_START_INDEX + 1;

    template <class T>
    struct BOOST_LEAF_SYMBOL_VISIBLE tls_index
    {
        static BOOST_LEAF_CFG_TLS_INDEX_TYPE idx;
    };

    template <class T>
    BOOST_LEAF_CFG_TLS_INDEX_TYPE tls_index<T>::idx = BOOST_LEAF_CFG_TLS_ARRAY_START_INDEX + 1;

    template <class T>
    struct BOOST_LEAF_SYMBOL_VISIBLE reserve_tls_index
    {
        static BOOST_LEAF_CFG_TLS_INDEX_TYPE const idx;
    };

    template <class T>
    BOOST_LEAF_CFG_TLS_INDEX_TYPE const reserve_tls_index<T>::idx = tls_index<T>::idx = index_counter<>::next<T>();
} // namespace detail

} } // namespace boost::leaf

////////////////////////////////////////

namespace boost { namespace leaf {

namespace tls
{
    BOOST_LEAF_ALWAYS_INLINE unsigned generate_next_error_id() noexcept
    {
        unsigned id = (detail::id_factory<>::counter += 4);
        BOOST_LEAF_ASSERT((id&3) == 1);
        return id;
    }

    BOOST_LEAF_ALWAYS_INLINE void write_current_error_id( unsigned x ) noexcept
    {
        static_assert(sizeof(std::intptr_t) >= sizeof(unsigned), "Incompatible tls_array implementation");
        write_void_ptr(BOOST_LEAF_CFG_TLS_ARRAY_START_INDEX, (void *) (std::intptr_t) x);
    }

    BOOST_LEAF_ALWAYS_INLINE unsigned read_current_error_id() noexcept
    {
        static_assert(sizeof(std::intptr_t) >= sizeof(unsigned), "Incompatible tls_array implementation");
        return (unsigned) (std::intptr_t) read_void_ptr(BOOST_LEAF_CFG_TLS_ARRAY_START_INDEX);
    }

    template <class T>
    BOOST_LEAF_ALWAYS_INLINE void reserve_ptr()
    {
        (void) detail::reserve_tls_index<T>::idx;
    }

    template <class T>
    BOOST_LEAF_ALWAYS_INLINE void write_ptr( T * p ) noexcept
    {
        int tls_idx = detail::tls_index<T>::idx;
        BOOST_LEAF_ASSERT(tls_idx != (BOOST_LEAF_CFG_TLS_ARRAY_START_INDEX + 1));
        --tls_idx;
        write_void_ptr(tls_idx, p);
        BOOST_LEAF_ASSERT(read_void_ptr(tls_idx) == p);
    }

    template <class T>
    BOOST_LEAF_ALWAYS_INLINE T * read_ptr() noexcept
    {
        int tls_idx = detail::tls_index<T>::idx;
        if( tls_idx == (BOOST_LEAF_CFG_TLS_ARRAY_START_INDEX + 1) )
            return nullptr;
        --tls_idx;
        return reinterpret_cast<T *>(read_void_ptr(tls_idx));
    }
} // namespace tls

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_CONFIG_TLS_ARRAY_HPP_INCLUDED
// <<< #   include <boost/leaf/config/tls_array.hpp>
// #line 82 "boost/leaf/config/tls.hpp"
#elif BOOST_LEAF_CFG_WIN32 == 2
// >>> #   include <boost/leaf/config/tls_win32.hpp>
#ifndef BOOST_LEAF_CONFIG_TLS_WIN32_HPP_INCLUDED
#define BOOST_LEAF_CONFIG_TLS_WIN32_HPP_INCLUDED

// #line 8 "boost/leaf/config/tls_win32.hpp"
// This header implements the TLS API specified in tls.hpp using Win32 TLS
// functions, allowing error objects to cross DLL boundaries on Windows. This
// implementation is enabled by defining BOOST_LEAF_CFG_WIN32=2 before including
// any LEAF headers.

#ifndef _WIN32
#   error "This header is only for Windows"
#endif

#include <atomic>
#include <unordered_map>
#include <cstdint>
#include <new>
#include <stdexcept>
#include <windows.h>
#ifdef min
#   undef min
#endif
#ifdef max
#   undef max
#endif

namespace boost { namespace leaf {

// Thrown on TLS allocation failure.
class win32_tls_error:
    public std::runtime_error
{
public:
    explicit win32_tls_error(char const * what) noexcept:
        std::runtime_error(what)
    {
    }
};

namespace detail
{
    __declspec(noreturn) inline void raise_fail_fast(NTSTATUS status) noexcept
    {
        EXCEPTION_RECORD rec = {};
        rec.ExceptionCode = status;
        rec.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
        RaiseFailFastException(&rec, nullptr, 0);
        BOOST_LEAF_UNREACHABLE;
    }

    template <class T, class... Args>
    T * heap_new(Args && ... args) noexcept
    {
        void * mem = HeapAlloc(GetProcessHeap(), 0, sizeof(T));
        if (!mem)
        {
            raise_fail_fast(STATUS_NO_MEMORY);
            BOOST_LEAF_UNREACHABLE;
        }
        return new (mem) T(static_cast<Args &&>(args)...);
    }

    template <class T>
    void heap_delete(T * p) noexcept
    {
        if (p)
        {
            p->~T();
            BOOL r = HeapFree(GetProcessHeap(), 0, p);
            BOOST_LEAF_ASSERT(r), (void) r;
        }
    }

    template <class T>
    class heap_allocator
    {
    public:

        using value_type = T;

        heap_allocator() noexcept = default;

        template <class U>
        heap_allocator(heap_allocator<U> const &) noexcept
        {
        }

        T * allocate(std::size_t n) noexcept
        {
            if (void * p = HeapAlloc(GetProcessHeap(), 0, n * sizeof(T)))
                return static_cast<T *>(p);
            raise_fail_fast(STATUS_NO_MEMORY);
            BOOST_LEAF_UNREACHABLE;
        }

        void deallocate(T * p, std::size_t) noexcept
        {
            BOOL r = HeapFree(GetProcessHeap(), 0, p);
            BOOST_LEAF_ASSERT(r), (void) r;
        }

        friend bool operator==(heap_allocator const &, heap_allocator const &) noexcept { return true; }
        friend bool operator!=(heap_allocator const &, heap_allocator const &) noexcept { return false; }
    };

    class critical_section_lock
    {
        critical_section_lock(critical_section_lock const &) = delete;
        critical_section_lock & operator=(critical_section_lock const &) = delete;

        CRITICAL_SECTION & cs_;

    public:

        explicit critical_section_lock(CRITICAL_SECTION & cs) noexcept:
            cs_(cs)
        {
            EnterCriticalSection(&cs_);
        }

        ~critical_section_lock() noexcept
        {
            LeaveCriticalSection(&cs_);
        }
    };

    using atomic_unsigned_int = std::atomic<unsigned int>;

    template <int N, int I>
    struct cpp11_hash_step
    {
        BOOST_LEAF_ALWAYS_INLINE constexpr static std::uint32_t compute(char const (&str)[N], std::uint32_t hash) noexcept
        {
            return cpp11_hash_step<N, I - 1>::compute(str, (hash ^ static_cast<std::uint32_t>(str[I])) * 16777619u);
        }
    };

    template <int N>
    struct cpp11_hash_step<N, -1>
    {
        BOOST_LEAF_ALWAYS_INLINE constexpr static std::uint32_t compute(char const (&)[N], std::uint32_t hash) noexcept
        {
            return hash;
        }
    };

    template <int N>
    BOOST_LEAF_ALWAYS_INLINE constexpr std::uint32_t cpp11_hash_string(char const (&str)[N]) noexcept
    {
        return cpp11_hash_step<N, N - 2>::compute(str, 2166136261u); // str[N-2] is the last character before the \0.
    }
} // namespace detail

namespace n
{
    template <class T>
    BOOST_LEAF_ALWAYS_INLINE constexpr std::uint32_t __cdecl h() noexcept
    {
        return detail::cpp11_hash_string(BOOST_LEAF_PRETTY_FUNCTION);
    }
}

namespace detail
{
    template<class T>
    BOOST_LEAF_ALWAYS_INLINE constexpr std::uint32_t type_hash() noexcept
    {
        return n::h<T>();
    }
}

} } // namespace boost::leaf

////////////////////////////////////////

namespace boost { namespace leaf {

namespace detail
{
    class slot_map
    {
        slot_map(slot_map const &) = delete;
        slot_map & operator=(slot_map const &) = delete;

        class tls_slot_index
        {
            tls_slot_index(tls_slot_index const &) = delete;
            tls_slot_index & operator=(tls_slot_index const &) = delete;
            tls_slot_index & operator=(tls_slot_index &&) = delete;

            DWORD idx_;

        public:

            BOOST_LEAF_ALWAYS_INLINE tls_slot_index():
                idx_(TlsAlloc())
            {
                if (idx_ == TLS_OUT_OF_INDEXES)
                    throw_exception_(win32_tls_error("TLS_OUT_OF_INDEXES"));
            }

            BOOST_LEAF_ALWAYS_INLINE ~tls_slot_index() noexcept
            {
                if (idx_ == TLS_OUT_OF_INDEXES)
                    return;
                BOOL r = TlsFree(idx_);
                BOOST_LEAF_ASSERT(r), (void) r;
            }

            BOOST_LEAF_ALWAYS_INLINE tls_slot_index(tls_slot_index && other) noexcept:
                idx_(other.idx_)
            {
                other.idx_ = TLS_OUT_OF_INDEXES;
            }

            BOOST_LEAF_ALWAYS_INLINE DWORD get() const noexcept
            {
                BOOST_LEAF_ASSERT(idx_ != TLS_OUT_OF_INDEXES);
                return idx_;
            }
        };

        int refcount_;
        HANDLE const mapping_;
        tls_slot_index const error_id_slot_;
        mutable CRITICAL_SECTION cs_;
        std::unordered_map<
            std::uint32_t,
            tls_slot_index,
            std::hash<std::uint32_t>,
            std::equal_to<std::uint32_t>,
            heap_allocator<std::pair<std::uint32_t const, tls_slot_index>>> map_;
        atomic_unsigned_int error_id_storage_;

    public:

        explicit slot_map(HANDLE mapping) noexcept:
            refcount_(1),
            mapping_(mapping),
            error_id_storage_(1)
        {
            BOOST_LEAF_ASSERT(mapping != INVALID_HANDLE_VALUE);
            InitializeCriticalSection(&cs_);
        }

        ~slot_map() noexcept
        {
            DeleteCriticalSection(&cs_);
            BOOL r = CloseHandle(mapping_);
            BOOST_LEAF_ASSERT(r), (void) r;
        }

        BOOST_LEAF_ALWAYS_INLINE void add_ref() noexcept
        {
            BOOST_LEAF_ASSERT(refcount_ >= 1);
            ++refcount_;
        }

        BOOST_LEAF_ALWAYS_INLINE void release() noexcept
        {
            --refcount_;
            BOOST_LEAF_ASSERT(refcount_ >= 0);
            if (refcount_ == 0)
                heap_delete(this);
        }

        DWORD check(std::uint32_t type_hash) const noexcept
        {
            critical_section_lock lock(cs_);
            auto it = map_.find(type_hash);
            return (it != map_.end()) ? it->second.get() : TLS_OUT_OF_INDEXES;
        }

        DWORD get(std::uint32_t type_hash)
        {
            critical_section_lock lock(cs_);
            DWORD idx = map_[type_hash].get();
            BOOST_LEAF_ASSERT(idx != TLS_OUT_OF_INDEXES);
            return idx;
        }

        BOOST_LEAF_ALWAYS_INLINE DWORD error_id_slot() const noexcept
        {
            return error_id_slot_.get();
        }

        BOOST_LEAF_ALWAYS_INLINE atomic_unsigned_int & error_id_storage() noexcept
        {
            return error_id_storage_;
        }
    }; // class slot_map

    class module_state
    {
        module_state(module_state const &) = delete;
        module_state & operator=(module_state const &) = delete;

        static constexpr unsigned tls_failure_create_mapping = 0x01;
        static constexpr unsigned tls_failure_map_view = 0x02;

        void * hinstance_;
        unsigned tls_failures_;
        slot_map * sm_;

    public:

        constexpr module_state() noexcept:
            hinstance_(nullptr),
            tls_failures_(0),
            sm_(nullptr)
        {
        }

        BOOST_LEAF_ALWAYS_INLINE slot_map & sm() const noexcept
        {
            BOOST_LEAF_ASSERT(hinstance_);
            BOOST_LEAF_ASSERT(!(tls_failures_ & tls_failure_create_mapping));
            BOOST_LEAF_ASSERT(!(tls_failures_ & tls_failure_map_view));
            BOOST_LEAF_ASSERT(sm_);
            return *sm_;
        }

        BOOST_LEAF_ALWAYS_INLINE void update(PVOID hinstDLL, DWORD dwReason) noexcept
        {
            if (dwReason == DLL_PROCESS_ATTACH)
            {
                hinstance_ = hinstDLL;
                char name[32] = "Local\\boost_leaf_";
                {
                    constexpr static char const hex[] = "0123456789ABCDEF";
                    DWORD pid = GetCurrentProcessId();
                    for (int i = 7; i >= 0; --i)
                    {
                        name[17 + i] = hex[pid & 0xf];
                        pid >>= 4;
                    }
                    name[25] = '\0';
                }
                HANDLE mapping = CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, sizeof(slot_map *), name);
                DWORD mapping_status = GetLastError();
                if (!mapping)
                {
                    tls_failures_ |= tls_failure_create_mapping;
                    return;
                }
                BOOST_LEAF_ASSERT(mapping_status == ERROR_ALREADY_EXISTS || mapping_status == ERROR_SUCCESS);
                bool is_first_module = (mapping_status == ERROR_SUCCESS);
                slot_map * * mapped_ptr = static_cast<slot_map * *>(MapViewOfFile(mapping, FILE_MAP_WRITE, 0, 0, sizeof(slot_map *)));
                if (!mapped_ptr)
                {
                    tls_failures_ |= tls_failure_map_view;
                    BOOL r = CloseHandle(mapping);
                    BOOST_LEAF_ASSERT(r), (void) r;
                    return;
                }
                if (is_first_module)
                    sm_ = *mapped_ptr = heap_new<slot_map>(mapping);
                else
                {
                    sm_ = *mapped_ptr;
                    sm_->add_ref();
                    BOOL r = CloseHandle(mapping);
                    BOOST_LEAF_ASSERT(r), (void) r;
                }
                UnmapViewOfFile(mapped_ptr);
            }
            else if (dwReason == DLL_PROCESS_DETACH)
            {
                BOOST_LEAF_ASSERT(sm_ || tls_failures_);
                if (sm_)
                {
                    sm_->release();
                    sm_ = nullptr;
                }
            }
        }
    }; // class module_state

    template<int = 0>
    struct module
    {
        static module_state state;
    };

    template<int N>
    module_state module<N>::state;

    BOOST_LEAF_ALWAYS_INLINE unsigned generate_next_error_id() noexcept
    {
        static atomic_unsigned_int & counter = module<>::state.sm().error_id_storage();
        unsigned id = (counter += 4);
        BOOST_LEAF_ASSERT((id&3) == 1);
        return id;
    }

    inline void NTAPI tls_callback(PVOID hinstDLL, DWORD dwReason, PVOID) noexcept
    {
        module<>::state.update(hinstDLL, dwReason);
    }

#ifdef _MSC_VER
#   pragma section(".CRT$XLB", long, read)
#   pragma data_seg(push, ".CRT$XLB")

extern "C" __declspec(selectany) PIMAGE_TLS_CALLBACK boost_leaf_tls_callback = tls_callback;

#   pragma data_seg(pop)
#   ifdef _WIN64
#       pragma comment(linker, "/INCLUDE:boost_leaf_tls_callback")
#   else
#       pragma comment(linker, "/INCLUDE:_boost_leaf_tls_callback")
#   endif
#elif defined(__GNUC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wattributes"

extern "C" __attribute__((used, selectany)) PIMAGE_TLS_CALLBACK boost_leaf_tls_callback __attribute__((section(".CRT$XLB"))) = tls_callback;

#   pragma GCC diagnostic pop
#else
#   error Unknown compiler, unable to define .CRT$XLB section
#endif
} // namespace detail

} } // namespace boost::leaf

////////////////////////////////////////

namespace boost { namespace leaf {

namespace tls
{
    BOOST_LEAF_ALWAYS_INLINE unsigned generate_next_error_id() noexcept
    {
        return detail::generate_next_error_id();
    }

    BOOST_LEAF_ALWAYS_INLINE void write_current_error_id(unsigned x) noexcept
    {
        using namespace detail;
        DWORD slot = module<>::state.sm().error_id_slot();
        BOOL r = TlsSetValue(slot, reinterpret_cast<void *>(static_cast<std::uintptr_t>(x)));
        BOOST_LEAF_ASSERT(r), (void) r;
    }

    BOOST_LEAF_ALWAYS_INLINE unsigned read_current_error_id() noexcept
    {
        using namespace detail;
        DWORD slot = module<>::state.sm().error_id_slot();
        LPVOID value = TlsGetValue(slot);
        BOOST_LEAF_ASSERT(GetLastError() == ERROR_SUCCESS);
        return static_cast<unsigned>(reinterpret_cast<std::uintptr_t>(value));
    }

    template <class T>
    BOOST_LEAF_ALWAYS_INLINE void reserve_ptr()
    {
        using namespace detail;
        thread_local DWORD const cached_slot = module<>::state.sm().get(type_hash<T>());
        BOOST_LEAF_ASSERT(cached_slot != TLS_OUT_OF_INDEXES), (void) cached_slot;
    }

    template <class T>
    BOOST_LEAF_ALWAYS_INLINE void write_ptr(T * p) noexcept
    {
        using namespace detail;
        thread_local DWORD const cached_slot = module<>::state.sm().check(type_hash<T>());
        DWORD slot = cached_slot;
        BOOST_LEAF_ASSERT(slot != TLS_OUT_OF_INDEXES);
        BOOL r = TlsSetValue(slot, p);
        BOOST_LEAF_ASSERT(r), (void) r;
    }

    template <class T>
    BOOST_LEAF_ALWAYS_INLINE T * read_ptr() noexcept
    {
        using namespace detail;
        thread_local DWORD cached_slot = TLS_OUT_OF_INDEXES;
        if (cached_slot == TLS_OUT_OF_INDEXES)
            cached_slot = module<>::state.sm().check(type_hash<T>());
        DWORD slot = cached_slot;
        if (slot == TLS_OUT_OF_INDEXES)
            return nullptr;
        LPVOID value = TlsGetValue(slot);
        BOOST_LEAF_ASSERT(GetLastError() == ERROR_SUCCESS);
        return static_cast<T *>(value);
    }
} // namespace tls

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_CONFIG_TLS_WIN32_HPP_INCLUDED
// <<< #   include <boost/leaf/config/tls_win32.hpp>
// #line 84 "boost/leaf/config/tls.hpp"
#elif defined(BOOST_LEAF_NO_THREADS)
// >>> #   include <boost/leaf/config/tls_globals.hpp>
#ifndef BOOST_LEAF_CONFIG_TLS_GLOBALS_HPP_INCLUDED
#define BOOST_LEAF_CONFIG_TLS_GLOBALS_HPP_INCLUDED

// #line 8 "boost/leaf/config/tls_globals.hpp"
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

    template <class=void>
    struct BOOST_LEAF_SYMBOL_VISIBLE current_error_id_storage
    {
        static unsigned x;
    };

    template <class T>
    unsigned current_error_id_storage<T>::x = 0;
} // namespace detail

} } // namespace boost::leaf

////////////////////////////////////////

namespace boost { namespace leaf {

namespace tls
{
    BOOST_LEAF_ALWAYS_INLINE unsigned generate_next_error_id() noexcept
    {
        unsigned id = (detail::id_factory<>::counter += 4);
        BOOST_LEAF_ASSERT((id&3) == 1);
        return id;
    }

    BOOST_LEAF_ALWAYS_INLINE void write_current_error_id( unsigned v ) noexcept
    {
        detail::current_error_id_storage<>::x = v;
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
// <<< #   include <boost/leaf/config/tls_globals.hpp>
// #line 86 "boost/leaf/config/tls.hpp"
#else
// >>> #   include <boost/leaf/config/tls_cpp11.hpp>
#ifndef BOOST_LEAF_CONFIG_TLS_CPP11_HPP_INCLUDED
#define BOOST_LEAF_CONFIG_TLS_CPP11_HPP_INCLUDED

// #line 8 "boost/leaf/config/tls_cpp11.hpp"
// This header implements the TLS API specified in tls.hpp using the C++11
// built-in thread_local storage class specifier. On Windows, this
// implementation does not allow error objects to cross DLL boundaries. If this
// is required, define BOOST_LEAF_CFG_WIN32=2 before including any LEAF headers
// to enable the alternative implementation defined in tls_win32.hpp.

#include <atomic>
#include <cstdint>

namespace boost { namespace leaf {

namespace detail
{
    using atomic_unsigned_int = std::atomic<unsigned int>;

    template <class=void>
    struct BOOST_LEAF_SYMBOL_VISIBLE id_factory
    {
        static atomic_unsigned_int counter;
    };

    template <class T>
    atomic_unsigned_int id_factory<T>::counter(1);

    template <class T>
    struct BOOST_LEAF_SYMBOL_VISIBLE ptr
    {
        static thread_local T * p;
    };

    template <class T>
    thread_local T * ptr<T>::p;

    template <class=void>
    struct BOOST_LEAF_SYMBOL_VISIBLE current_error_id_storage
    {
        static thread_local unsigned x;
    };

    template <class T>
    thread_local unsigned current_error_id_storage<T>::x;
} // namespace detail

} } // namespace boost::leaf

////////////////////////////////////////

namespace boost { namespace leaf {

namespace tls
{
    BOOST_LEAF_ALWAYS_INLINE unsigned generate_next_error_id() noexcept
    {
        unsigned id = (detail::id_factory<>::counter += 4);
        BOOST_LEAF_ASSERT((id&3) == 1);
        return id;
    }

    BOOST_LEAF_ALWAYS_INLINE void write_current_error_id( unsigned x ) noexcept
    {
        detail::current_error_id_storage<>::x = x;
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

#endif // #ifndef BOOST_LEAF_CONFIG_TLS_CPP11_HPP_INCLUDED
// <<< #   include <boost/leaf/config/tls_cpp11.hpp>
// #line 88 "boost/leaf/config/tls.hpp"
#endif

#endif // #ifndef BOOST_LEAF_CONFIG_TLS_HPP_INCLUDED
// <<< #include <boost/leaf/config/tls.hpp>
// #line 283 "boost/leaf/config.hpp"

#endif // #ifndef BOOST_LEAF_CONFIG_HPP_INCLUDED
// <<< #include <boost/leaf/config.hpp>
// #line 9 "boost/leaf/common.hpp"

#include <iosfwd>
#include <cerrno>
#include <cstring>

#if BOOST_LEAF_CFG_STD_STRING
#   include <string>
#endif

#if BOOST_LEAF_CFG_WIN32
#   include <windows.h>
#   ifdef min
#       undef min
#   endif
#   ifdef max
#       undef max
#   endif
#endif

namespace boost { namespace leaf {

struct e_api_function { char const * value; };

#if BOOST_LEAF_CFG_STD_STRING

struct e_file_name
{
    std::string value;
};

#else

struct e_file_name
{
    char const * value = "<unavailable>";
    BOOST_LEAF_CONSTEXPR explicit e_file_name( char const * ) { }
};

#endif

struct e_errno
{
    int value;

    explicit e_errno(int val=errno): value(val) { }

    template <class CharT, class Traits>
    friend std::ostream & operator<<(std::basic_ostream<CharT, Traits> & os, e_errno const & err)
    {
        return os << err.value << ", \"" << std::strerror(err.value) << '"';
    }
};

struct e_type_info_name { char const * value; };

struct e_at_line { int value; };

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
                if( z != (LPSTR)mb.p && z[-1] == '\n' )
                    *--z = 0;
                if( z != (LPSTR)mb.p && z[-1] == '\r' )
                    *--z = 0;
                return os << err.value << ", \"" << (LPCSTR)mb.p << '"';
            }
            return os;
        }
#endif // #if BOOST_LEAF_CFG_WIN32
    };
} // namespace windows

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_COMMON_HPP_INCLUDED
// >>> #include <boost/leaf/context.hpp>
#ifndef BOOST_LEAF_CONTEXT_HPP_INCLUDED
#define BOOST_LEAF_CONTEXT_HPP_INCLUDED

// #line 8 "boost/leaf/context.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 19
// >>> #include <boost/leaf/serialization/diagnostics_writer.hpp>
#ifndef BOOST_LEAF_SERIALIZATION_DIAGNOSTICS_WRITER_HPP_INCLUDED
#define BOOST_LEAF_SERIALIZATION_DIAGNOSTICS_WRITER_HPP_INCLUDED

// #line 8 "boost/leaf/serialization/diagnostics_writer.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 19
// >>> #include <boost/leaf/serialization/writer.hpp>
#ifndef BOOST_LEAF_SERIALIZATION_WRITER_HPP_INCLUDED
#define BOOST_LEAF_SERIALIZATION_WRITER_HPP_INCLUDED

// #line 8 "boost/leaf/serialization/writer.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 19
// >>> #include <boost/leaf/serialization/type_name.hpp>
#ifndef BOOST_LEAF_SERIALIZATION_TYPE_NAME_HPP_INCLUDED
#define BOOST_LEAF_SERIALIZATION_TYPE_NAME_HPP_INCLUDED

// #line 8 "boost/leaf/serialization/type_name.hpp"
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

// #include <boost/leaf/config.hpp> // Expanded at line 19
#include <iosfwd>
#include <cstdlib>
#include <cstring>

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
//  For some architectures (mips, mips64, x86, x86_64) cxxabi.h in Android NDK is implemented by gabi++ library
//  (https://android.googlesource.com/platform/ndk/+/master/sources/cxx-stl/gabi++/), which does not implement
//  abi::__cxa_demangle(). We detect this implementation by checking the include guard here.
#   if defined(__GABIXX_CXXABI_H__)
#       undef BOOST_LEAF_HAS_CXXABI_H
#   endif
#endif

namespace boost { namespace leaf {

namespace detail
{
    // The functions below are C++11 constexpr, but we use BOOST_LEAF_ALWAYS_INLINE to control object file
    // section count / template bloat.

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

    ////////////////////////////////////////

    template <std::size_t S>
    BOOST_LEAF_ALWAYS_INLINE std::size_t compute_hash(char const (&str)[S], std::size_t begin, std::size_t end) noexcept
    {
        std::size_t h = 2166136261u;
        for( std::size_t i = begin; i != end; ++i )
            h = (h ^ static_cast<std::size_t>(str[i])) * 16777619u;
        return h;
    }
} // namespace detail

namespace n
{
    struct r
    {
        char const * name_not_zero_terminated_at_length;
        std::size_t length;
        std::size_t hash;
    };

#ifdef _MSC_VER
#   define BOOST_LEAF_CDECL __cdecl
#else
#   define BOOST_LEAF_CDECL
#endif

    template <class T>
    BOOST_LEAF_ALWAYS_INLINE r BOOST_LEAF_CDECL p()
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
        std::size_t const p01 = BOOST_LEAF_P("r boost::leaf::n::p() [T = ");
        std::size_t const p02 = BOOST_LEAF_P("r __cdecl boost::leaf::n::p(void) [T = ");
        // old clang style:
        std::size_t const p03 = BOOST_LEAF_P("boost::leaf::n::r boost::leaf::n::p() [T = ");
        std::size_t const p04 = BOOST_LEAF_P("boost::leaf::n::r __cdecl boost::leaf::n::p(void) [T = ");
        // gcc style:
        std::size_t const p05 = BOOST_LEAF_P("boost::leaf::n::r boost::leaf::n::p() [with T = ");
        std::size_t const p06 = BOOST_LEAF_P("boost::leaf::n::r __cdecl boost::leaf::n::p() [with T = ");
        // msvc style, struct:
        std::size_t const p07 = BOOST_LEAF_P("struct boost::leaf::n::r __cdecl boost::leaf::n::p<struct ");
        // msvc style, class:
        std::size_t const p08 = BOOST_LEAF_P("struct boost::leaf::n::r __cdecl boost::leaf::n::p<class ");
        // msvc style, enum:
        std::size_t const p09 = BOOST_LEAF_P("struct boost::leaf::n::r __cdecl boost::leaf::n::p<enum ");
        // msvc style, built-in type:
        std::size_t const p10 = BOOST_LEAF_P("struct boost::leaf::n::r __cdecl boost::leaf::n::p<");
#undef BOOST_LEAF_P

#define BOOST_LEAF_S(S) (sizeof(char[1 + detail::check_suffix(BOOST_LEAF_PRETTY_FUNCTION, S)]) - 1)
        // clang/gcc style:
        std::size_t const s01 = BOOST_LEAF_S("]");
        // msvc style:
        std::size_t const s02 = BOOST_LEAF_S(">(void)");
#undef BOOST_LEAF_S

        char static_assert_unrecognized_pretty_function_format_please_file_github_issue[sizeof(
            char[
                (s01 && (1 == (!!p01 + !!p02 + !!p03 + !!p04 + !!p05 + !!p06)))
                ||
                (s02 && (1 == (!!p07 + !!p08 + !!p09)))
                ||
                (s02 && !!p10)
            ]
        ) * 2 - 1];
        (void) static_assert_unrecognized_pretty_function_format_please_file_github_issue;

        if( std::size_t const p = sizeof(char[1 + !!s01 * (p01 + p02 + p03 + p04 + p05 + p06)]) - 1 )
            return { BOOST_LEAF_PRETTY_FUNCTION + p, s01 - p, detail::compute_hash(BOOST_LEAF_PRETTY_FUNCTION, p, s01) };

        if( std::size_t const p = sizeof(char[1 + !!s02 * (p07 + p08 + p09)]) - 1 )
            return { BOOST_LEAF_PRETTY_FUNCTION + p, s02 - p, detail::compute_hash(BOOST_LEAF_PRETTY_FUNCTION, p, s02) };

        std::size_t const p = sizeof(char[1 + !!s02 * p10]) - 1;
        return { BOOST_LEAF_PRETTY_FUNCTION + p, s02 - p, detail::compute_hash(BOOST_LEAF_PRETTY_FUNCTION, p, s02) };
    }

#undef BOOST_LEAF_CDECL

} // namespace n

} } // namespace boost::leaf

////////////////////////////////////////

namespace boost { namespace leaf {

namespace detail
{
    class demangler
    {
        char const * mangled_name_;
#ifdef BOOST_LEAF_HAS_CXXABI_H
        char * demangled_name_ = nullptr;
#endif

    public:

        explicit demangler(char const * mangled_name) noexcept:
            mangled_name_(mangled_name)
        {
            BOOST_LEAF_ASSERT(mangled_name_);
#ifdef BOOST_LEAF_HAS_CXXABI_H
            int status = 0;
            demangled_name_ = abi::__cxa_demangle(mangled_name_, nullptr, nullptr, &status);
#endif
        }

        ~demangler() noexcept
        {
#ifdef BOOST_LEAF_HAS_CXXABI_H
            std::free(demangled_name_);
#endif
        }

        char const * get() const noexcept
        {
#ifdef BOOST_LEAF_HAS_CXXABI_H
            if( demangled_name_ )
                return demangled_name_;
#endif
            return mangled_name_;
        }
    };
} // namespace detail

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_DETAIL_DEMANGLE_HPP_INCLUDED
// <<< #include <boost/leaf/detail/demangle.hpp>
// #line 9 "boost/leaf/serialization/type_name.hpp"

#include <functional>

#if BOOST_LEAF_CFG_STD_STRING
#   include <string>
#endif

#if __cplusplus >= 201703L
#   include <string_view>
#endif

namespace boost { namespace leaf {

namespace serialization
{

struct type_name
{
    char const * name_not_zero_terminated_at_length;
    std::size_t length;
    std::size_t hash;

    friend bool operator==(type_name const & a, type_name const & b) noexcept
    {
        BOOST_LEAF_ASSERT((a.hash == b.hash) == (a.length == b.length && std::memcmp(a.name_not_zero_terminated_at_length, b.name_not_zero_terminated_at_length, a.length) == 0));
        return a.hash == b.hash;
    }

    friend bool operator!=(type_name const & a, type_name const & b) noexcept
    {
        return !(a == b);
    }

    friend bool operator<(type_name const & a, type_name const & b) noexcept
    {
        if( int cmp = std::memcmp(a.name_not_zero_terminated_at_length, b.name_not_zero_terminated_at_length, a.length < b.length ? a.length : b.length) )
            return cmp < 0;
        return a.length < b.length;
    }

    template <class CharT, class Traits>
    friend std::ostream & operator<<(std::basic_ostream<CharT, Traits> & os, type_name const & x)
    {
        return os.write(x.name_not_zero_terminated_at_length, x.length);
    }

#if __cplusplus >= 201703L
    friend std::string_view to_string_view(type_name const & x) noexcept
    {
        return std::string_view(x.name_not_zero_terminated_at_length, x.length);
    }
#endif

#if BOOST_LEAF_CFG_STD_STRING
    friend std::string to_string(type_name const & x)
    {
        return std::string(x.name_not_zero_terminated_at_length, x.length);
    }
#endif

    template <std::size_t S>
    friend char * to_zstr(char (&zstr)[S], type_name const & x) noexcept
    {
        std::size_t n = x.length < S - 1 ? x.length : S - 1;
        std::memcpy(zstr, x.name_not_zero_terminated_at_length, n);
        zstr[n] = 0;
        return zstr;
    }
};

template <class T>
type_name get_type_name()
{
    n::r parsed = n::p<T>();
    return { parsed.name_not_zero_terminated_at_length, parsed.length, parsed.hash };
}

} // namespace serialization

} } // namespace boost::leaf

namespace std
{
    template <>
    struct hash<boost::leaf::serialization::type_name>
    {
        std::size_t operator()(boost::leaf::serialization::type_name const & x) const noexcept
        {
            return x.hash;
        }
    };
} // namespace std

#endif // #ifndef BOOST_LEAF_SERIALIZATION_TYPE_NAME_HPP_INCLUDED
// <<< #include <boost/leaf/serialization/type_name.hpp>
// #line 10 "boost/leaf/serialization/writer.hpp"

#include <type_traits>

namespace boost { namespace leaf {

namespace serialization
{
class writer
{
    type_name const type_;

protected:

    template <class Derived>
    explicit writer(Derived * d) noexcept:
        type_(get_type_name<Derived>())
    {
        BOOST_LEAF_ASSERT(d == this), (void) d;
    }

    ~writer() noexcept
    {
    }

public:

    template <class Derived>
    Derived * get() noexcept
    {
        return type_ == get_type_name<typename std::decay<Derived>::type>() ? static_cast<Derived *>(this) : nullptr;
    }
};

    template <class W, class E>
    typename std::enable_if<std::is_base_of<writer, W>::value>::type
    serialize(W &, E const &)
    {
    }
}

namespace detail
{
    template <class>
    struct dependent_writer
    {
        using type = serialization::writer;
    };
}

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_SERIALIZATION_WRITER_HPP_INCLUDED
// <<< #include <boost/leaf/serialization/writer.hpp>
// #line 10 "boost/leaf/serialization/diagnostics_writer.hpp"
// >>> #include <boost/leaf/detail/exception_base.hpp>
#ifndef BOOST_LEAF_DETAIL_EXCEPTION_BASE_HPP_INCLUDED
#define BOOST_LEAF_DETAIL_EXCEPTION_BASE_HPP_INCLUDED

// #line 8 "boost/leaf/detail/exception_base.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 19
// #include <boost/leaf/serialization/type_name.hpp> // Expanded at line 1446

namespace boost { namespace leaf {

class error_id;

namespace detail
{
    class exception_base
    {
    public:
        virtual error_id get_error_id() const noexcept = 0;
        virtual serialization::type_name get_type_name() const = 0;
    protected:
        exception_base() noexcept { }
        ~exception_base() noexcept { }
    };
}

} }

#endif // #ifndef BOOST_LEAF_DETAIL_EXCEPTION_BASE_HPP_INCLUDED
// <<< #include <boost/leaf/detail/exception_base.hpp>
// #line 11 "boost/leaf/serialization/diagnostics_writer.hpp"

#include <type_traits>

#if BOOST_LEAF_CFG_DIAGNOSTICS
#   include <iostream>
#else
#   include <iosfwd>
#endif

#ifndef BOOST_LEAF_NO_EXCEPTIONS
#   include <typeinfo>
#endif

namespace boost { namespace leaf {

template <class E>
struct show_in_diagnostics: std::integral_constant<bool, BOOST_LEAF_CFG_DIAGNOSTICS>
{
};

namespace serialization
{
    template <class T, class E = void>
    struct is_printable: std::false_type
    {
    };

    template <class T>
    struct is_printable<T, decltype(std::declval<std::ostream&>()<<std::declval<T const &>(), void())>: std::true_type
    {
    };

    template <class T, class E = void>
    struct has_printable_member_value: std::false_type
    {
    };

    template <class T>
    struct has_printable_member_value<T, decltype(std::declval<std::ostream&>()<<std::declval<T const &>().value, void())>: std::true_type
    {
    };

    template <class T, class E = void>
    struct has_member_value: std::false_type
    {
    };

    template <class T>
    struct has_member_value<T, decltype((void)std::declval<T const &>().value)>: std::true_type
    {
    };

    ////////////////////////////////////////

    class diagnostics_writer: public writer
    {
        diagnostics_writer(diagnostics_writer const &) = delete;
        diagnostics_writer & operator=(diagnostics_writer const &) = delete;

        std::ostream & os_;
        char const * prefix_;
        char const * delimiter_;
        void (* const print_suffix_)(std::ostream &);

        template <class T, class CharT, class Traits>
        static void print_name(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter)
        {
            static_assert(show_in_diagnostics<T>::value, "show_in_diagnostics violation");
            BOOST_LEAF_ASSERT(delimiter);
            char const * p = prefix;
            prefix = nullptr;
            os << (p ? p : delimiter) << get_type_name<T>();
        }

        template <class T, class PrintableInfo, class CharT, class Traits>
        static bool print_impl(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter, char const * mid, PrintableInfo const & x)
        {
            print_name<T>(os, prefix, delimiter);
            if( mid )
                os << mid << x;
            return true;
        }

        template <class T, class PrintableInfo, class CharT, class Traits>
        static bool print_impl(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter, char const * mid, PrintableInfo const * x)
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

        template <
            class Wrapper,
            bool ShowInDiagnostics = show_in_diagnostics<Wrapper>::value,
            bool WrapperPrintable = is_printable<Wrapper>::value,
            bool ValuePrintable = has_printable_member_value<Wrapper>::value,
            bool IsException = std::is_base_of<std::exception,Wrapper>::value,
            bool IsEnum = std::is_enum<Wrapper>::value>
        struct diagnostic;

    public:

        template <class CharT, class Traits>
        explicit diagnostics_writer(std::basic_ostream<CharT, Traits> & os) noexcept:
            writer(this),
            os_(os),
            prefix_(BOOST_LEAF_CFG_DIAGNOSTICS_FIRST_DELIMITER),
            delimiter_(BOOST_LEAF_CFG_DIAGNOSTICS_DELIMITER),
            print_suffix_([](std::basic_ostream<CharT, Traits> &) { })
        {
        }

        template <class CharT, class Traits>
        diagnostics_writer(std::basic_ostream<CharT, Traits> & os, error_id const & id, e_source_location const * loc, std::exception const * ex) noexcept:
            writer(this),
            os_(os),
            prefix_(BOOST_LEAF_CFG_DIAGNOSTICS_FIRST_DELIMITER),
            delimiter_(BOOST_LEAF_CFG_DIAGNOSTICS_DELIMITER),
            print_suffix_([](std::basic_ostream<CharT, Traits> & os) { os << '\n'; })
        {
            os << "Error with serial #" << id;
            if( loc )
                os << " reported at " << *loc;
#ifndef BOOST_LEAF_NO_EXCEPTIONS
            if( ex )
            {
                os << "\nCaught:" BOOST_LEAF_CFG_DIAGNOSTICS_FIRST_DELIMITER;
                if( auto eb = dynamic_cast<detail::exception_base const *>(ex) )
                    os << eb->get_type_name();
                else
                    os << detail::demangler(typeid(*ex).name()).get();
                os << ": \"" << ex->what() << '"';
            }
            else
#endif
            {
                prefix_ = "\nCaught:" BOOST_LEAF_CFG_DIAGNOSTICS_FIRST_DELIMITER;
            }
            (void) ex;
        }

        ~diagnostics_writer() noexcept
        {
            print_suffix_(os_);
        }

        void set_prefix(char const * prefix) noexcept
        {
            prefix_ = prefix;
        }

        void set_delimiter(char const * delimiter) noexcept
        {
            delimiter_ = delimiter;
        }

        template <class E>
        void write(E const & e)
        {
            diagnostic<E>::print(os_, prefix_, delimiter_, e);
        }
    }; // class diagnostics_writer

    ////////////////////////////////////////

    template <class Wrapper, bool WrapperPrintable, bool ValuePrintable, bool IsException, bool IsEnum>
    struct diagnostics_writer::diagnostic<Wrapper, false, WrapperPrintable, ValuePrintable, IsException, IsEnum>
    {
        template <class CharT, class Traits>
        static bool print(std::basic_ostream<CharT, Traits> &, char const * &, char const *, Wrapper const &) noexcept
        {
            return false;
        }
    };

    template <class Wrapper, bool ValuePrintable, bool IsEnum>
    struct diagnostics_writer::diagnostic<Wrapper, true, true, ValuePrintable, false, IsEnum>
    {
        template <class CharT, class Traits>
        static bool print(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter, Wrapper const & x)
        {
            return print_impl<Wrapper>(os, prefix, delimiter, ": ", x);
        }
    };

    template <class Wrapper>
    struct diagnostics_writer::diagnostic<Wrapper, true, false, true, false, false>
    {
        template <class CharT, class Traits>
        static bool print(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter, Wrapper const & x)
        {
            return print_impl<Wrapper>(os, prefix, delimiter, ": ", x.value);
        }
    };

    template <class Exception, bool WrapperPrintable, bool ValuePrintable>
    struct diagnostics_writer::diagnostic<Exception, true, WrapperPrintable, ValuePrintable, true, false>
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
    struct diagnostics_writer::diagnostic<Wrapper, true, false, false, false, false>
    {
        template <class CharT, class Traits>
        static bool print(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter, Wrapper const &)
        {
            return print_impl<Wrapper>(os, prefix, delimiter, nullptr, 0);
        }
    };

    template <class Enum>
    struct diagnostics_writer::diagnostic<Enum, true, false, false, false, true>
    {
        template <class CharT, class Traits>
        static bool print(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter, Enum const & enum_)
        {
            return print_impl<Enum>(os, prefix, delimiter, ": ", static_cast<typename std::underlying_type<Enum>::type>(enum_));
        }
    };

} // namespace serialization

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_SERIALIZATION_DIAGNOSTICS_WRITER_HPP_INCLUDED
// <<< #include <boost/leaf/serialization/diagnostics_writer.hpp>
// #line 10 "boost/leaf/context.hpp"
// >>> #include <boost/leaf/error.hpp>
#ifndef BOOST_LEAF_ERROR_HPP_INCLUDED
#define BOOST_LEAF_ERROR_HPP_INCLUDED

// #line 8 "boost/leaf/error.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 19
// >>> #include <boost/leaf/detail/optional.hpp>
#ifndef BOOST_LEAF_DETAIL_OPTIONAL_HPP_INCLUDED
#define BOOST_LEAF_DETAIL_OPTIONAL_HPP_INCLUDED

// #line 8 "boost/leaf/detail/optional.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 19
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
    }; // template optional

} // namespace detail

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_DETAIL_OPTIONAL_HPP_INCLUDED
// <<< #include <boost/leaf/detail/optional.hpp>
// #line 10 "boost/leaf/error.hpp"
// >>> #include <boost/leaf/detail/function_traits.hpp>
#ifndef BOOST_LEAF_DETAIL_FUNCTION_TRAITS_HPP_INCLUDED
#define BOOST_LEAF_DETAIL_FUNCTION_TRAITS_HPP_INCLUDED

// #line 8 "boost/leaf/detail/function_traits.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 19
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

#endif // #ifndef BOOST_LEAF_DETAIL_MP11_HPP_INCLUDED
// <<< #include <boost/leaf/detail/mp11.hpp>
// #line 10 "boost/leaf/detail/function_traits.hpp"

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
} // namespace detail

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_DETAIL_FUNCTION_TRAITS_HPP_INCLUDED
// <<< #include <boost/leaf/detail/function_traits.hpp>
// #line 11 "boost/leaf/error.hpp"
// >>> #include <boost/leaf/detail/capture_list.hpp>
#ifndef BOOST_LEAF_DETAIL_CAPTURE_LIST_HPP_INCLUDED
#define BOOST_LEAF_DETAIL_CAPTURE_LIST_HPP_INCLUDED

// #line 8 "boost/leaf/detail/capture_list.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 19

#if BOOST_LEAF_CFG_CAPTURE

namespace boost { namespace leaf {

class error_id;
namespace serialization { class writer; }

namespace detail
{

    class capture_list
    {
        capture_list( capture_list const & ) = delete;
        capture_list & operator=( capture_list const & ) = delete;

    protected:

        class node
        {
            friend class capture_list;

            virtual void unload( int err_id ) = 0;
            virtual void write_to(serialization::writer &, error_id const &) const = 0;

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
            tls::write_current_error_id(unsigned(err_id));
            moved.for_each(
                [err_id]( node & n )
                {
                    n.unload(err_id); // last node may throw
                } );
        }

        void write_to(serialization::writer & w, error_id const & id) const
        {
            if( first_ )
            {
                for_each(
                    [&w, &id]( node const & n )
                    {
                        n.write_to(w, id);
                    } );
            }
        }
    }; // class capture_list

} // namespace detail

} } // namespace boost::leaf

#endif // #if BOOST_LEAF_CFG_CAPTURE

#endif // #ifndef BOOST_LEAF_DETAIL_CAPTURE_LIST_HPP_INCLUDED
// <<< #include <boost/leaf/detail/capture_list.hpp>
// #line 12 "boost/leaf/error.hpp"
// #include <boost/leaf/serialization/diagnostics_writer.hpp> // Expanded at line 1434

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

#else // #if BOOST_LEAF_CFG_GNUC_STMTEXPR

#define BOOST_LEAF_CHECK(r)\
    {\
        auto && BOOST_LEAF_TMP = (r);\
        static_assert(::boost::leaf::is_result_type<typename std::decay<decltype(BOOST_LEAF_TMP)>::type>::value,\
            "BOOST_LEAF_CHECK requires a result object (see is_result_type)");\
        if( !BOOST_LEAF_TMP )\
            return BOOST_LEAF_TMP.error();\
    }

#endif // #else (#if BOOST_LEAF_CFG_GNUC_STMTEXPR)

#define BOOST_LEAF_NEW_ERROR ::boost::leaf::detail::inject_loc{__FILE__,__LINE__,__FUNCTION__}+::boost::leaf::new_error

namespace boost { namespace leaf {

struct e_source_location
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

namespace detail
{
    template <class Writer, class E>
    void serialize_(Writer & w, E const & e)
    {
        using namespace serialization;
        typename dependent_writer<Writer>::type & wr = w;
        serialize(wr, e);
        if( diagnostics_writer * dw = w.template get<diagnostics_writer>() )
            dw->write(e);
    }
}

////////////////////////////////////////

namespace detail
{
    template <class E>
    class slot:
        optional<E>
    {
        static_assert(std::is_same<E, typename std::decay<E>::type>::value, "E must be decayed for slot<E>");
        slot( slot const & ) = delete;
        slot & operator=( slot const & ) = delete;

        using impl = optional<E>;
        slot<E> * prev_;

    public:

        BOOST_LEAF_CONSTEXPR slot():
            prev_(nullptr)
        {
            tls::reserve_ptr<slot<E>>();
        }

        template <class T>
        BOOST_LEAF_CONSTEXPR slot( int key, T && e ):
            optional<E>(key, std::forward<T>(e)),
            prev_(nullptr)
        {
            tls::reserve_ptr<slot<E>>();
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

        template <class ErrorID>
        void write_to(serialization::writer & w, ErrorID id) const
        {
            if( int k = this->key() )
            {
                if( id && id.value() != k )
                    return;
                serialize_(w, value(k));
            }
        }

        using impl::load;
        using impl::has_value;
        using impl::value;
    }; // template slot
} // namespace detail

////////////////////////////////////////

#if BOOST_LEAF_CFG_CAPTURE

namespace detail
{
    class preloaded_base;

    template <class E>
    struct capturing_slot_node_allocator;

    class dynamic_allocator:
        capture_list
    {
        dynamic_allocator( dynamic_allocator const & ) = delete;
        dynamic_allocator & operator=( dynamic_allocator const & ) = delete;

        template <class>
        friend struct capturing_slot_node_allocator;

        preloaded_base * preloaded_list_;

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
        class capturing_slot_node final:
            public slot<E>,
            public capturing_node
        {
            using impl = slot<E>;
            capturing_slot_node( capturing_slot_node const & ) = delete;
            capturing_slot_node & operator=( capturing_slot_node const & ) = delete;
            void deactivate() const noexcept override
            {
                impl::deactivate();
            }
            void unload( int err_id ) override
            {
                impl::unload(err_id);
            }
            void write_to(serialization::writer & w, error_id const & id) const override
            {
                impl::write_to(w, id);
            }
        public:
            BOOST_LEAF_CONSTEXPR explicit capturing_slot_node( capture_list::node * * & last ):
                capturing_node(last)
            {
                BOOST_LEAF_ASSERT(last == &next_);
                BOOST_LEAF_ASSERT(next_ == nullptr);
            }
            template <class T>
            BOOST_LEAF_CONSTEXPR capturing_slot_node( capture_list::node * * & last, int err_id, T && e ):
                slot<E>(err_id, std::forward<T>(e)),
                capturing_node(last)
            {
                BOOST_LEAF_ASSERT(last == &next_);
                BOOST_LEAF_ASSERT(next_ == nullptr);
            }
        };

#ifndef BOOST_LEAF_NO_EXCEPTIONS
        class capturing_exception_node final:
            public capturing_node
        {
            capturing_exception_node( capturing_exception_node const & ) = delete;
            capturing_exception_node & operator=( capturing_exception_node const & ) = delete;
            void deactivate() const noexcept override
            {
                BOOST_LEAF_ASSERT(0);
            }
            void unload( int ) override
            {
                std::rethrow_exception(ex_);
            }
            void write_to(serialization::writer &, error_id const &) const override
            {
            }
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
#endif // #ifndef BOOST_LEAF_NO_EXCEPTIONS

        node * * last_;

    public:

        dynamic_allocator() noexcept:
            capture_list(nullptr),
            preloaded_list_(nullptr),
            last_(&first_)
        {
            BOOST_LEAF_ASSERT(first_ == nullptr);
        }

        dynamic_allocator( dynamic_allocator && other ) noexcept:
            capture_list(std::move(other)),
            preloaded_list_(nullptr),
            last_(other.last_ == &other.first_? &first_ : other.last_)
        {
            BOOST_LEAF_ASSERT(last_ != nullptr);
            BOOST_LEAF_ASSERT(*last_ == nullptr);
            BOOST_LEAF_ASSERT(other.first_ == nullptr);
            BOOST_LEAF_ASSERT(other.preloaded_list_ == nullptr);
            other.last_ = &other.first_;
        }

        preloaded_base * preloaded_list() const noexcept
        {
            return preloaded_list_;
        }

        preloaded_base * link_preloaded_item(preloaded_base * pb) noexcept
        {
            BOOST_LEAF_ASSERT(pb != nullptr);
            preloaded_base * next = preloaded_list_;
            preloaded_list_ = pb;
            return next;
        }

        void unlink_preloaded_item(preloaded_base * next) noexcept
        {
            preloaded_list_ = next;
        }

        template <class E>
        slot<E> * alloc()
        {
            BOOST_LEAF_ASSERT(last_ != nullptr);
            BOOST_LEAF_ASSERT(*last_ == nullptr);
            BOOST_LEAF_ASSERT(tls::read_ptr<slot<E>>() == nullptr);
            capturing_slot_node<E> * csn = capturing_slot_node_allocator<E>::new_(last_);
            csn->activate();
            return csn;
        }

        template <class E>
        slot<E> * reserve()
        {
            if( slot<E> * p = tls::read_ptr<slot<E>>() )
                return p;
            else
                return alloc<E>();
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
        using capture_list::write_to;
    }; // class dynamic_allocator

    template <class E>
    struct capturing_slot_node_allocator
    {
        template <class... A>
        static dynamic_allocator::capturing_slot_node<E> * new_( A && ... a )
        {
            return new dynamic_allocator::capturing_slot_node<E>(std::forward<A>(a)...);
        }

        static void delete_( dynamic_allocator::capturing_slot_node<E> * p ) noexcept
        {
            delete p;
        }
    };

    template <>
    class slot<dynamic_allocator>
    {
        slot( slot const & ) = delete;
        slot & operator=( slot const & ) = delete;

        dynamic_allocator da_;
        slot * prev_;

    public:

        slot() noexcept:
            prev_(nullptr)
        {
            tls::reserve_ptr<slot<dynamic_allocator>>();
        }

        slot( slot && x ) noexcept:
            da_(std::move(x.da_)),
            prev_(nullptr)
        {
            BOOST_LEAF_ASSERT(x.prev_ == nullptr);
        }

        ~slot() noexcept
        {
            BOOST_LEAF_ASSERT(tls::read_ptr<slot<dynamic_allocator>>() != this);
        }

        dynamic_allocator const & get() const noexcept
        {
            return da_;
        }

        dynamic_allocator & get() noexcept
        {
            return da_;
        }

        void activate() noexcept
        {
            prev_ = tls::read_ptr<slot<dynamic_allocator>>();
            tls::write_ptr<slot<dynamic_allocator>>(this);
        }

        void deactivate() const noexcept
        {
            da_.deactivate();
            tls::write_ptr<slot<dynamic_allocator>>(prev_);
        }

        void unload( int err_id )
        {
            BOOST_LEAF_ASSERT(err_id);
            da_.unload(err_id);
        }

        template <class ErrorID>
        void write_to(serialization::writer &, ErrorID) const
        {
        }
    }; // slot specialization for dynamic_allocator
} // namespace detail

#endif // #if BOOST_LEAF_CFG_CAPTURE

////////////////////////////////////////

namespace detail
{
#if BOOST_LEAF_CFG_CAPTURE
    inline dynamic_allocator * get_dynamic_allocator() noexcept
    {
        if( slot<dynamic_allocator> * sl = tls::read_ptr<slot<dynamic_allocator>>() )
            return &sl->get();
        return nullptr;
    }
#endif

    template <class E>
    inline slot<E> * get_slot() noexcept(!BOOST_LEAF_CFG_CAPTURE)
    {
        static_assert(!std::is_pointer<E>::value, "Error objects of pointer types are not allowed");
        static_assert(!std::is_same<E, error_id>::value, "Error objects of type error_id are not allowed");
        if( slot<E> * p = tls::read_ptr<slot<E>>() )
            return p;
#if BOOST_LEAF_CFG_CAPTURE
        if( dynamic_allocator * da = get_dynamic_allocator() )
            return da->alloc<E>();
#endif
        return nullptr;
    }

    template <class E>
    inline void slot<E>::unload( int err_id ) noexcept(!BOOST_LEAF_CFG_CAPTURE)
    {
        BOOST_LEAF_ASSERT(err_id);
        if( this->key() != err_id )
            return;
        if( impl * p = get_slot<E>() )
            if( !p->has_value(err_id) )
                *p = std::move(*this);
    }

    template <class E>
    BOOST_LEAF_CONSTEXPR inline int load_slot( int err_id, E && e ) noexcept(!BOOST_LEAF_CFG_CAPTURE)
    {
        using E_decayed = typename std::decay<E>::type;
        BOOST_LEAF_ASSERT((err_id&3) == 1);
        if( slot<E_decayed> * p = get_slot<E_decayed>() )
            (void) p->load(err_id, std::forward<E>(e));
        return 0;
    }

    template <class F>
    BOOST_LEAF_CONSTEXPR inline int load_slot_deferred( int err_id, F && f )
    {
        using E = typename function_traits<F>::return_type;
        using E_decayed = typename std::decay<E>::type;
        BOOST_LEAF_ASSERT((err_id&3) == 1);
        if( slot<E_decayed> * p = get_slot<E_decayed>() )
            (void) p->load(err_id, std::forward<F>(f)());
        return 0;
    }

    template <class F>
    BOOST_LEAF_CONSTEXPR inline int load_slot_accumulate( int err_id, F && f )
    {
        static_assert(function_traits<F>::arity == 1, "Lambdas passed to accumulate must take a single e-type argument by reference");
        using E = fn_arg_type<F,0>;
        using E_decayed = typename std::decay<E>::type;
        BOOST_LEAF_ASSERT((err_id&3) == 1);
        if( slot<E_decayed> * p = get_slot<E_decayed>() )
            if( E_decayed * v = p->has_value(err_id) )
                (void) std::forward<F>(f)(*v);
            else
                (void) std::forward<F>(f)(p->load(err_id, E_decayed()));
        return 0;
    }
} // namespace detail

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
        BOOST_LEAF_CONSTEXPR static int load_( int err_id, E && e )
        {
            return load_slot(err_id, std::forward<E>(e));
        }
    };

    template <class F>
    struct load_item<F, 0>
    {
        BOOST_LEAF_CONSTEXPR static int load_( int err_id, F && f )
        {
            return load_slot_deferred(err_id, std::forward<F>(f));
        }
    };

    template <class F>
    struct load_item<F, 1>
    {
        BOOST_LEAF_CONSTEXPR static int load_( int err_id, F && f )
        {
            return load_slot_accumulate(err_id, std::forward<F>(f));
        }
    };
}

////////////////////////////////////////

namespace detail
{
#if BOOST_LEAF_CFG_CAPTURE
    class preloaded_base
    {
    protected:

        preloaded_base() noexcept:
            next_(
                []( preloaded_base * this_ ) -> preloaded_base *
                {
                    if( dynamic_allocator * da = get_dynamic_allocator() )
                        return da->link_preloaded_item(this_);
                    return nullptr;
                }(this))
        {
        }

        ~preloaded_base() noexcept
        {
            if( dynamic_allocator * da = get_dynamic_allocator() )
                da->unlink_preloaded_item(next_);
            else
                BOOST_LEAF_ASSERT(next_ == nullptr);
        }

    public:

        preloaded_base * const next_;

        virtual void reserve( dynamic_allocator & ) const = 0;
    };
#endif // #if BOOST_LEAF_CFG_CAPTURE

    inline int current_id() noexcept
    {
        unsigned id = tls::read_current_error_id();
        BOOST_LEAF_ASSERT(id == 0 || (id&3) == 1);
        return int(id);
    }

    inline int new_id() noexcept
    {
        unsigned id = tls::generate_next_error_id();
        tls::write_current_error_id(id);
        return int(id);
    }

    inline int start_new_error() noexcept(!BOOST_LEAF_CFG_CAPTURE)
    {
#if BOOST_LEAF_CFG_CAPTURE
        if( dynamic_allocator * da = get_dynamic_allocator() )
            for( preloaded_base const * e = da->preloaded_list(); e; e = e->next_ )
                e->reserve(*da);
#endif
        return new_id();
    }

    struct inject_loc
    {
        char const * file;
        int line;
        char const * fn;

        template <class T>
        friend T operator+( inject_loc loc, T && x ) noexcept(!BOOST_LEAF_CFG_CAPTURE)
        {
            x.load_source_location_(loc.file, loc.line, loc.fn);
            return std::move(x);
        }
    };
} // namespace detail

#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR

namespace detail
{
    class leaf_error_category final:
        public std::error_category
    {
        bool equivalent( int,  std::error_condition const & ) const noexcept override { return false; }
        bool equivalent( std::error_code const &, int ) const noexcept override { return false; }
        char const * name() const noexcept override { return "LEAF error"; }
        std::string message( int ) const override { return name(); }
    public:
        ~leaf_error_category() noexcept override { }
    };

    template <class=void>
    struct get_leaf_error_category
    {
        static leaf_error_category cat;
    };

    template <class T>
    leaf_error_category get_leaf_error_category<T>::cat;

    inline int import_error_code( std::error_code const & ec ) noexcept(!BOOST_LEAF_CFG_CAPTURE)
    {
        if( int err_id = ec.value() )
        {
            std::error_category const & cat = get_leaf_error_category<>::cat;
            if( &ec.category() == &cat )
            {
                BOOST_LEAF_ASSERT((err_id&3) == 1);
                return (err_id&~3)|1;
            }
            else
            {
                err_id = start_new_error();
                (void) load_slot(err_id, ec);
                return (err_id&~3)|1;
            }
        }
        else
            return 0;
    }
} // namespace detail

inline bool is_error_id( std::error_code const & ec ) noexcept
{
    bool res = (&ec.category() == &detail::get_leaf_error_category<>::cat);
    BOOST_LEAF_ASSERT(!res || !ec.value() || ((ec.value()&3) == 1));
    return res;
}

#endif // #if BOOST_LEAF_CFG_STD_SYSTEM_ERROR

////////////////////////////////////////

namespace detail
{
    BOOST_LEAF_CONSTEXPR error_id make_error_id(int) noexcept;
}

class error_id
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
    explicit error_id( std::error_code const & ec ) noexcept(!BOOST_LEAF_CFG_CAPTURE):
        value_(detail::import_error_code(std::error_code(ec)))
    {
        BOOST_LEAF_ASSERT(!value_ || ((value_&3) == 1));
    }

    template <class Enum>
    error_id( Enum e, typename std::enable_if<std::is_error_code_enum<Enum>::value, int>::type = 0 ) noexcept(!BOOST_LEAF_CFG_CAPTURE):
        value_(detail::import_error_code(e))
    {
    }

    template <class T, typename std::enable_if<std::is_constructible<T, std::error_code>::value, int>::type = 0>
    operator T() const noexcept
    {
        return std::error_code(value_, detail::get_leaf_error_category<>::cat);
    }
#endif // #if BOOST_LEAF_CFG_STD_SYSTEM_ERROR

    BOOST_LEAF_CONSTEXPR error_id load() const noexcept
    {
        return *this;
    }

    template <class Item>
    BOOST_LEAF_CONSTEXPR error_id load(Item && item) const
    {
        if (int err_id = value())
        {
            int const unused[] = { 42, detail::load_item<Item>::load_(err_id, std::forward<Item>(item)) };
            (void)unused;
        }
        return *this;
    }

    template <class... Item>
    BOOST_LEAF_CONSTEXPR error_id load( Item && ... item ) const
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

    BOOST_LEAF_CONSTEXPR void load_source_location_( char const * file, int line, char const * function ) const noexcept(!BOOST_LEAF_CFG_CAPTURE)
    {
        BOOST_LEAF_ASSERT(file&&*file);
        BOOST_LEAF_ASSERT(line>0);
        BOOST_LEAF_ASSERT(function&&*function);
        BOOST_LEAF_ASSERT(value_);
        (void) load(e_source_location {file,line,function});
    }
}; // class error_id

namespace detail
{
    BOOST_LEAF_CONSTEXPR inline error_id make_error_id( int err_id ) noexcept
    {
        BOOST_LEAF_ASSERT(err_id == 0 || (err_id&3) == 1);
        return error_id((err_id&~3)|1);
    }
}

inline error_id new_error()
{
    return detail::make_error_id(detail::start_new_error());
}

template <class... Item>
inline error_id new_error( Item && ... item )
{
    return detail::make_error_id(detail::start_new_error()).load(std::forward<Item>(item)...);
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

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_ERROR_HPP_INCLUDED
// <<< #include <boost/leaf/error.hpp>
// #line 11 "boost/leaf/context.hpp"

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
} // namespace detail

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
} // namespace detail

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

        static void write_to(serialization::writer & w, void const * tup, error_id id)
        {
            BOOST_LEAF_ASSERT(tup != nullptr);
            tuple_for_each<I-1,Tup>::write_to(w, tup, id);
            std::get<I-1>(*static_cast<Tup const *>(tup)).write_to(w, id);
        }
    };

    template <class Tup>
    struct tuple_for_each<0, Tup>
    {
        BOOST_LEAF_CONSTEXPR static void activate( Tup & ) noexcept { }
        BOOST_LEAF_CONSTEXPR static void deactivate( Tup & ) noexcept { }
        BOOST_LEAF_CONSTEXPR static void unload( Tup &, int ) noexcept { }
        BOOST_LEAF_CONSTEXPR static void write_to(serialization::writer &, void const *, error_id) { }
    };

    template <class Tup>
    BOOST_LEAF_CONSTEXPR void serialize_tuple_contents(serialization::writer & w, void const * tup, error_id id)
    {
        tuple_for_each<std::tuple_size<Tup>::value, Tup>::write_to(w, tup, id);
    }
} // namespace detail

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
} // namespace detail

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
#if __cplusplus < 201703L
        BOOST_LEAF_CONSTEXPR BOOST_LEAF_ALWAYS_INLINE raii_deactivator( raii_deactivator && x ) noexcept:
            ctx_(x.ctx_)
        {
            x.ctx_ = nullptr;
        }
#endif
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

    BOOST_LEAF_CONSTEXPR context():
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

    void write_to( serialization::writer & w ) const
    {
        detail::serialize_tuple_contents<Tup>(w, &tup_, error_id());
    }

    template <class CharT, class Traits>
    friend std::ostream & operator<<( std::basic_ostream<CharT, Traits> & os, context const & ctx )
    {
        serialization::diagnostics_writer w(os);
        w.set_prefix("Contents:");
        ctx.write_to(w);
        return os;
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
}; // template context

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
} // namespace detail

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

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_CONTEXT_HPP_INCLUDED
// >>> #include <boost/leaf/diagnostics.hpp>
#ifndef BOOST_LEAF_DIAGNOSTICS_HPP_INCLUDED
#define BOOST_LEAF_DIAGNOSTICS_HPP_INCLUDED

// #line 8 "boost/leaf/diagnostics.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 19
// #include <boost/leaf/context.hpp> // Expanded at line 1428
// >>> #include <boost/leaf/handle_errors.hpp>
#ifndef BOOST_LEAF_HANDLE_ERRORS_HPP_INCLUDED
#define BOOST_LEAF_HANDLE_ERRORS_HPP_INCLUDED

// #line 8 "boost/leaf/handle_errors.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 19
// #include <boost/leaf/context.hpp> // Expanded at line 1428
// #include <boost/leaf/serialization/diagnostics_writer.hpp> // Expanded at line 1434

namespace boost { namespace leaf {

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

#endif // #ifndef BOOST_LEAF_NO_EXCEPTIONS

////////////////////////////////////////

class error_info
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

    template <class Writer>
    void write_to(Writer & w) const
    {
        detail::serialize_(w, err_id_);
#ifndef BOOST_LEAF_NO_EXCEPTIONS
        if( ex_ )
            detail::serialize_(w, *ex_);
#endif
    }

    template <class CharT, class Traits>
    friend std::ostream & operator<<(std::basic_ostream<CharT, Traits> & os, error_info const & x)
    {
        serialization::diagnostics_writer w(os, x.error(), x.source_location(), x.exception());
        return os;
    }
}; // class error_info

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
#endif // #if BOOST_LEAF_CFG_STD_SYSTEM_ERROR

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

#endif // #ifndef BOOST_LEAF_NO_EXCEPTIONS

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
} // namespace detail

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
} // namespace detail

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
} // namespace detail

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
        error_id id(r.error());
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
        error_id id(r.error());
        ctx.deactivate();
        using R = typename std::decay<decltype(std::declval<TryBlock>()())>::type;
        auto rr = ctx.template handle_error<R>(std::move(id), std::forward<H>(h)..., [&r]()->R { return std::move(r); });
        if( !rr )
            ctx.unload(error_id(rr.error()));
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

#else // #ifdef BOOST_LEAF_NO_EXCEPTIONS

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
            return r;
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
} // namespace detail

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
        error_id id(r.error());
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
        error_id id(r.error());
        ctx.deactivate();
        using R = typename std::decay<decltype(std::declval<TryBlock>()())>::type;
        auto rr = ctx.template handle_error<R>(std::move(id), std::forward<H>(h)...,
            [&r]()->R
            {
                return std::move(r);
            });
        if( !rr )
            ctx.unload(error_id(rr.error()));
        return rr;
    }
    else
    {
        ctx.unload(error_id(r.error()));
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

#endif // #else (#ifdef BOOST_LEAF_NO_EXCEPTIONS)

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
        try_capture_all_( TryBlock && try_block )
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
                    return leaf_result(sl.get().template extract_capture_list<leaf_result>(err_id));
                }
            }
#ifndef BOOST_LEAF_NO_EXCEPTIONS
            catch( std::exception & ex )
            {
                sl.deactivate();
                int err_id = unpack_error_id(ex).value();
                return sl.get().template extract_capture_list<leaf_result>(err_id);
            }
            catch(...)
            {
                sl.deactivate();
                int err_id = current_error().value();
                return sl.get().template extract_capture_list<leaf_result>(err_id);
            }
#endif // #ifndef BOOST_LEAF_NO_EXCEPTIONS
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
        try_capture_all_( TryBlock && try_block )
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
                return sl.get().template extract_capture_list<leaf_result>(err_id);
            }
            catch(...)
            {
                sl.deactivate();
                int err_id = current_error().value();
                return sl.get().template extract_capture_list<leaf_result>(err_id);
            }
#endif // #ifndef BOOST_LEAF_NO_EXCEPTIONS
        }
    };
} // namespace detail

template <class TryBlock>
inline
typename detail::try_capture_all_dispatch<decltype(std::declval<TryBlock>()())>::leaf_result
try_capture_all( TryBlock && try_block ) noexcept
{
    return detail::try_capture_all_dispatch<decltype(std::declval<TryBlock>()())>::try_capture_all_(std::forward<TryBlock>(try_block));
}

#endif // #if BOOST_LEAF_CFG_CAPTURE

} } // namespace boost::leaf

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
} // namespace detail

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_HANDLE_ERRORS_HPP_INCLUDED
// <<< #include <boost/leaf/handle_errors.hpp>
// #line 11 "boost/leaf/diagnostics.hpp"

namespace boost { namespace leaf {

class diagnostic_info: public error_info
{
    void const * tup_;
    void (*serialize_tuple_contents_)(serialization::writer &, void const *, error_id);

protected:

    diagnostic_info( diagnostic_info const & ) noexcept = default;

    template <class Tup>
    BOOST_LEAF_CONSTEXPR diagnostic_info( error_info const & ei, Tup const & tup ) noexcept:
        error_info(ei),
        tup_(&tup),
        serialize_tuple_contents_(&detail::serialize_tuple_contents<Tup>)
    {
    }

    template <class Writer>
    void write_to_(Writer & w) const
    {
        serialize_tuple_contents_(w, tup_, error());
    }

public:

    template <class Writer>
    void write_to(Writer & w) const
    {
        error_info::write_to(w);
        write_to_(w);
    }

    template <class CharT, class Traits>
    friend std::ostream & operator<<( std::basic_ostream<CharT, Traits> & os, diagnostic_info const & x )
    {
        serialization::diagnostics_writer w(os, x.error(), x.source_location(), x.exception());
#if BOOST_LEAF_CFG_DIAGNOSTICS
        x.write_to_(w);
#else
        os << "\nboost::leaf::diagnostic_info N/A due to BOOST_LEAF_CFG_DIAGNOSTICS=0";
#endif
        return os;
    }
}; // class diagnostic_info

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

////////////////////////////////////////

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

    template <class Writer>
    void write_to_(Writer & w) const
    {
        if( da_ )
            da_->write_to(w, error());
    }

public:

    template <class Writer>
    void write_to(Writer & w) const
    {
        diagnostic_info::write_to(w);
        write_to_(w);
    }

    template <class CharT, class Traits>
    friend std::ostream & operator<<( std::basic_ostream<CharT, Traits> & os, diagnostic_details const & x )
    {
        serialization::diagnostics_writer w(os, x.error(), x.source_location(), x.exception());
#if BOOST_LEAF_CFG_DIAGNOSTICS
        x.diagnostic_info::write_to_(w);
        w.set_prefix("\nDiagnostic details:" BOOST_LEAF_CFG_DIAGNOSTICS_FIRST_DELIMITER);
        x.write_to_(w);
#else
        os << "\nboost::leaf::diagnostic_details N/A due to BOOST_LEAF_CFG_DIAGNOSTICS=0";
#endif
        return os;
    }
}; // class diagnostic_details

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
            return diagnostic_details_(ei, tup, da ? &da->get() : nullptr );
        }
    };
}

#else // #if BOOST_LEAF_CFG_CAPTURE

class diagnostic_details: public diagnostic_info
{
protected:

    diagnostic_details( diagnostic_details const & ) noexcept = default;

    template <class Tup>
    BOOST_LEAF_CONSTEXPR diagnostic_details( error_info const & ei, Tup const & tup ) noexcept:
        diagnostic_info(ei, tup)
    {
    }

public:

    template <class Writer>
    void write_to(Writer & w) const
    {
        diagnostic_info::write_to(w);
    }

    template <class CharT, class Traits>
    friend std::ostream & operator<<( std::basic_ostream<CharT, Traits> & os, diagnostic_details const & x )
    {
        serialization::diagnostics_writer w(os, x.error(), x.source_location(), x.exception());
#if BOOST_LEAF_CFG_DIAGNOSTICS
        x.diagnostic_info::write_to_(w);
        os << "\nboost::leaf::diagnostic_details N/A due to BOOST_LEAF_CFG_CAPTURE=0";
#else
        os << "\nboost::leaf::diagnostic_details N/A due to BOOST_LEAF_CFG_DIAGNOSTICS=0";
#endif
        return os;
    }
}; // class diagnostic_details

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

#endif // #else (#if BOOST_LEAF_CFG_CAPTURE)

using verbose_diagnostic_info = diagnostic_details;

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_DIAGNOSTICS_HPP_INCLUDED
// #include <boost/leaf/error.hpp> // Expanded at line 2126
// >>> #include <boost/leaf/exception.hpp>
#ifndef BOOST_LEAF_EXCEPTION_HPP_INCLUDED
#define BOOST_LEAF_EXCEPTION_HPP_INCLUDED

// #line 8 "boost/leaf/exception.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 19
// #include <boost/leaf/error.hpp> // Expanded at line 2126
// #include <boost/leaf/detail/exception_base.hpp> // Expanded at line 1852

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
            ::boost::leaf::throw_exception_(std::move(ex));
        }
    };
}

////////////////////////////////////////

namespace detail
{
    inline void enforce_std_exception( std::exception const & ) noexcept { }

    template <class Ex>
    class exception final:
        public Ex,
        public exception_base,
        public error_id
    {
        mutable bool clear_current_error_;

        bool is_current_exception() const noexcept
        {
            return tls::read_current_error_id() == unsigned(error_id::value());
        }

        error_id get_error_id() const noexcept override
        {
            clear_current_error_ = false;
            return *this;
        }

        serialization::type_name get_type_name() const override
        {
            return serialization::get_type_name<Ex>();
        }

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
                tls::write_current_error_id(0);
        }
    }; // template exception

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
    make_exception( error_id err, Ex && ex, E && ... e ) noexcept(!BOOST_LEAF_CFG_CAPTURE)
    {
        static_assert(!at_least_one_derives_from_std_exception<E...>::value, "Error objects passed to leaf::exception may not derive from std::exception");
        return exception<typename std::remove_reference<Ex>::type>( err.load(std::forward<E>(e)...), std::forward<Ex>(ex) );
    }

    template <class E1, class... E>
    inline
    typename std::enable_if<!std::is_base_of<std::exception,typename std::remove_reference<E1>::type>::value, exception<std::exception>>::type
    make_exception( error_id err, E1 && car, E && ... cdr ) noexcept(!BOOST_LEAF_CFG_CAPTURE)
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
    make_exception( Ex && ex, E && ... e ) noexcept(!BOOST_LEAF_CFG_CAPTURE)
    {
        static_assert(!at_least_one_derives_from_std_exception<E...>::value, "Error objects passed to leaf::exception may not derive from std::exception");
        return exception<typename std::remove_reference<Ex>::type>( new_error().load(std::forward<E>(e)...), std::forward<Ex>(ex) );
    }

    template <class E1, class... E>
    inline
    typename std::enable_if<!std::is_base_of<std::exception,typename std::remove_reference<E1>::type>::value, exception<std::exception>>::type
    make_exception( E1 && car, E && ... cdr ) noexcept(!BOOST_LEAF_CFG_CAPTURE)
    {
        static_assert(!at_least_one_derives_from_std_exception<E...>::value, "Error objects passed to leaf::exception may not derive from std::exception");
        return exception<std::exception>( new_error().load(std::forward<E1>(car), std::forward<E>(cdr)...) );
    }

    inline exception<std::exception> make_exception() noexcept
    {
        return exception<std::exception>(leaf::new_error());
    }
} // namespace detail

template <class... E>
[[noreturn]] void throw_exception( E && ... e )
{
    // Warning: setting a breakpoint here will not intercept exceptions thrown
    // via BOOST_LEAF_THROW_EXCEPTION or originating in the few other throw
    // points elsewhere in LEAF. To intercept all of those exceptions as well,
    // set a breakpoint inside boost::leaf::throw_exception_.
    throw_exception_(detail::make_exception(std::forward<E>(e)...));
}

////////////////////////////////////////

#ifndef BOOST_LEAF_NO_EXCEPTIONS

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
} // namespace detail

template <class... Ex, class F>
inline
detail::deduce_exception_to_result_return_type<detail::fn_return_type<F>>
exception_to_result( F && f ) noexcept(!BOOST_LEAF_CFG_CAPTURE)
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

#endif // #ifndef BOOST_LEAF_NO_EXCEPTIONS

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_EXCEPTION_HPP_INCLUDED
// #include <boost/leaf/handle_errors.hpp> // Expanded at line 4112
// >>> #include <boost/leaf/on_error.hpp>
#ifndef BOOST_LEAF_ON_ERROR_HPP_INCLUDED
#define BOOST_LEAF_ON_ERROR_HPP_INCLUDED

// #line 8 "boost/leaf/on_error.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 19
// #include <boost/leaf/error.hpp> // Expanded at line 2126

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
#   if BOOST_LEAF_CFG_CAPTURE
                try
                {
                    return detail::start_new_error();
                }
                catch(...)
                {
                    BOOST_LEAF_ASSERT(detail::current_id() == err_id_);
                    return detail::new_id();
                }
#   else
                return detail::start_new_error();
#   endif
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
}; // class error_monitor

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

#if BOOST_LEAF_CFG_CAPTURE
        static void reserve( Tup const & tup, dynamic_allocator & da )
        {
            tuple_for_each_preload<I-1,Tup>::reserve(tup,da);
            std::get<I-1>(tup).reserve(da);
        }
#endif
    };

    template <class Tup>
    struct tuple_for_each_preload<0, Tup>
    {
        BOOST_LEAF_CONSTEXPR static void trigger( Tup const &, int ) noexcept { }

#if BOOST_LEAF_CFG_CAPTURE
        static void reserve( Tup const &, dynamic_allocator & ) { }
#endif
    };

    template <class E>
    class preloaded_item
    {
        using E_decayed = typename std::decay<E>::type;
        E_decayed e_;

    public:

        BOOST_LEAF_CONSTEXPR preloaded_item( E && e ):
            e_(std::forward<E>(e))
        {
        }

        BOOST_LEAF_CONSTEXPR void trigger( int err_id ) noexcept
        {
            if( slot<E_decayed> * p = tls::read_ptr<slot<E_decayed>>() )
                if( !p->has_value(err_id) )
                    (void) p->load(err_id, std::move(e_));
        }

#if BOOST_LEAF_CFG_CAPTURE
        void reserve( dynamic_allocator & da ) const
        {
            da.reserve<E_decayed>();
        }
#endif
    };

    template <class F, class ReturnType = typename function_traits<F>::return_type>
    class deferred_item
    {
        using E_decayed = typename std::decay<ReturnType>::type;
        F f_;

    public:

        BOOST_LEAF_CONSTEXPR deferred_item( F && f ):
            f_(std::forward<F>(f))
        {
        }

        void trigger( int err_id ) noexcept
        {
            if( slot<E_decayed> * p = tls::read_ptr<slot<E_decayed>>() )
                if( !p->has_value(err_id) )
                {
#ifndef BOOST_LEAF_NO_EXCEPTIONS
                    try
                    {
#endif
                        (void) p->load(err_id, f_());
#ifndef BOOST_LEAF_NO_EXCEPTIONS
                    }
                    catch(...)
                    {
                    }
#endif
                }
        }

#if BOOST_LEAF_CFG_CAPTURE
        void reserve( dynamic_allocator & da ) const
        {
            da.reserve<E_decayed>();
        }
#endif
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

        void trigger( int ) noexcept
        {
#ifndef BOOST_LEAF_NO_EXCEPTIONS
            try
            {
#endif
                f_();
#ifndef BOOST_LEAF_NO_EXCEPTIONS
            }
            catch(...)
            {
            }
#endif
        }

#if BOOST_LEAF_CFG_CAPTURE
        void reserve( dynamic_allocator & ) const
        {
        }
#endif
    };

    template <class F, class A0 = fn_arg_type<F,0>, int arity = function_traits<F>::arity>
    class accumulating_item;

    template <class F, class A0>
    class accumulating_item<F, A0 &, 1>
    {
        using E_decayed = typename std::decay<A0>::type;
        F f_;

    public:

        BOOST_LEAF_CONSTEXPR accumulating_item( F && f ):
            f_(std::forward<F>(f))
        {
        }

        void trigger( int err_id ) noexcept
        {
            if( slot<E_decayed> * p = tls::read_ptr<slot<E_decayed>>() )
            {
#ifndef BOOST_LEAF_NO_EXCEPTIONS
                try
                {
#endif
                    if( E_decayed * v = p->has_value(err_id) )
                        (void) std::move(f_)(*v);
                    else
                        (void) std::move(f_)(p->load(err_id, E_decayed()));
#ifndef BOOST_LEAF_NO_EXCEPTIONS
                }
                catch(...)
                {
                }
#endif
            }
        }

#if BOOST_LEAF_CFG_CAPTURE
        void reserve( dynamic_allocator & da ) const
        {
            da.reserve<E_decayed>();
        }
#endif
    };

    template <class... Item>
    class preloaded
#if BOOST_LEAF_CFG_CAPTURE
        : preloaded_base
#endif
    {
        preloaded( preloaded const & ) = delete;
        preloaded & operator=( preloaded const & ) = delete;

        std::tuple<Item...> p_;
        error_monitor id_;
#if __cplusplus < 201703L
        bool moved_ = false;
#endif

#if BOOST_LEAF_CFG_CAPTURE
        void reserve( dynamic_allocator & da ) const override
        {
            tuple_for_each_preload<sizeof...(Item),decltype(p_)>::reserve(p_,da);
        }
#endif

    public:

        BOOST_LEAF_CONSTEXPR explicit preloaded( Item && ... i ):
            p_(std::forward<Item>(i)...)
        {
        }

#if __cplusplus < 201703L
        BOOST_LEAF_CONSTEXPR preloaded( preloaded && x ) noexcept:
            p_(std::move(x.p_)),
            id_(std::move(x.id_))
        {
            x.moved_ = true;
        }
#endif

        ~preloaded() noexcept
        {
#if __cplusplus < 201703L
            if( moved_ )
                return;
#endif
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
} // namespace detail

template <class... Item>
BOOST_LEAF_ATTRIBUTE_NODISCARD BOOST_LEAF_CONSTEXPR inline
detail::preloaded<typename detail::deduce_item_type<Item>::type...>
on_error( Item && ... i )
{
    return detail::preloaded<typename detail::deduce_item_type<Item>::type...>(std::forward<Item>(i)...);
}

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_ON_ERROR_HPP_INCLUDED
// >>> #include <boost/leaf/pred.hpp>
#ifndef BOOST_LEAF_PRED_HPP_INCLUDED
#define BOOST_LEAF_PRED_HPP_INCLUDED

// #line 8 "boost/leaf/pred.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 19
// #include <boost/leaf/handle_errors.hpp> // Expanded at line 4112

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
#endif // #if __cplusplus >= 201703L

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
} // namespace detail

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
#endif // #if BOOST_LEAF_CFG_STD_SYSTEM_ERROR

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
#endif // #if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
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
#endif // #if BOOST_LEAF_CFG_STD_SYSTEM_ERROR

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
#endif // #if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
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
#endif // #if BOOST_LEAF_CFG_STD_SYSTEM_ERROR

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
#endif // #if __cplusplus >= 201703L

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

#endif // #ifndef BOOST_LEAF_NO_EXCEPTIONS

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_PRED_HPP_INCLUDED
// >>> #include <boost/leaf/result.hpp>
#ifndef BOOST_LEAF_RESULT_HPP_INCLUDED
#define BOOST_LEAF_RESULT_HPP_INCLUDED

// #line 8 "boost/leaf/result.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 19
// #include <boost/leaf/exception.hpp> // Expanded at line 5190
// #include <boost/leaf/serialization/diagnostics_writer.hpp> // Expanded at line 1434
// #include <boost/leaf/detail/capture_list.hpp> // Expanded at line 2712

#include <functional>

namespace boost { namespace leaf {

////////////////////////////////////////

class bad_result:
    public std::exception
{
    char const * what() const noexcept override
    {
        return "boost::leaf::bad_result";
    }
};

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
    }; // class result_discriminant
} // namespace detail

////////////////////////////////////////

template <class T>
class BOOST_LEAF_ATTRIBUTE_NODISCARD result
{
    template <class U>
    friend class result;

#if BOOST_LEAF_CFG_CAPTURE
    friend class detail::dynamic_allocator;
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
                    (void) k;
                case result_discriminant::err_id_zero:
                    return result<U>(what.get_error_id());
            }
        }

        operator error_id() const noexcept
        {
            result_discriminant const what = r_.what_;
            return what.kind() == result_discriminant::val?
                error_id() :
                what.get_error_id();
        }
    };

    void destroy() const noexcept
    {
        switch(auto k = this->what_.kind())
        {
        default:
            BOOST_LEAF_ASSERT(k == result_discriminant::err_id);
            (void) k;
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
            (void) k;
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

    template <class Writer>
    error_id write_error_to(Writer & w) const
    {
        result_discriminant const what = what_;
        BOOST_LEAF_ASSERT(what.kind() != result_discriminant::val);
        error_id const err_id = what.get_error_id();
        detail::serialize_(w, err_id);
        return err_id;
    }

    template <class Writer>
    void write_capture_to(Writer & w, error_id err_id) const
    {
        if( what_.kind() == result_discriminant::err_id_capture_list )
        {
#if BOOST_LEAF_CFG_CAPTURE
            cap_.write_to(w, err_id);
#else
            BOOST_LEAF_ASSERT(0); // Possible ODR violation.
#endif
        }
    }

    template <class Writer>
    void print_error( Writer & w ) const
    {
        error_id err_id = write_error_to(w);
        w.set_prefix(", captured -> ");
        w.set_delimiter(", ");
        write_capture_to(w, err_id);
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

#else // #if defined(BOOST_STRICT_CONFIG) || !defined(__clang__)

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

#endif // #else (#if defined(BOOST_STRICT_CONFIG) || !defined(__clang__))

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
#endif // #if BOOST_LEAF_CFG_STD_SYSTEM_ERROR

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

#else // #ifdef BOOST_LEAF_NO_CXX11_REF_QUALIFIERS

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

#endif // #else (#ifdef BOOST_LEAF_NO_CXX11_REF_QUALIFIERS)

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

#else // #ifdef BOOST_LEAF_NO_CXX11_REF_QUALIFIERS

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

#endif // #else (#ifdef BOOST_LEAF_NO_CXX11_REF_QUALIFIERS)

    error_result error() noexcept
    {
        return error_result{*this};
    }

    template <class... Item>
    error_id load( Item && ... item ) noexcept(!BOOST_LEAF_CFG_CAPTURE)
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

    template <class Writer>
    void write_to(Writer & w) const
    {
        if( what_.kind() == result_discriminant::val )
            detail::serialize_(w, value());
        else
            write_capture_to(w, write_error_to(w));
    }

    template <class CharT, class Traits>
    friend std::ostream & operator<<( std::basic_ostream<CharT, Traits> & os, result const & r )
    {
        serialization::diagnostics_writer w(os);
        w.set_prefix(": ");
        if( r )
        {
            os << "Success";
            detail::serialize_(w, r.value());
        }
        else
        {
            os << "Failure";
            r.print_error(w);
        }
        return os;
    }
}; // template result

////////////////////////////////////////

namespace detail
{
    struct void_ { };
}

template <>
class BOOST_LEAF_ATTRIBUTE_NODISCARD result<void>:
    result<detail::void_>
{
    template <class U>
    friend class result;

    using result_discriminant = detail::result_discriminant;
    using void_ = detail::void_;
    using base = result<void_>;

#if BOOST_LEAF_CFG_CAPTURE
    friend class detail::dynamic_allocator;

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
#endif // #if BOOST_LEAF_CFG_STD_SYSTEM_ERROR

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

    template <class Writer>
    void write_to(Writer & w) const
    {
        if( !*this )
            write_error_to(w);
    }

    template <class CharT, class Traits>
    friend std::ostream & operator<<( std::basic_ostream<CharT, Traits> & os, result const & r )
    {
        if( r )
            os << "Success";
        else
        {
            serialization::diagnostics_writer w(os);
            w.set_prefix(": ");
            os << "Failure";
            r.print_error(w);
        }
        return os;
    }

    using base::operator=;
    using base::operator bool;
    using base::get_error_id;
    using base::error;
    using base::load;
    using base::unload;
}; // result specialization for void

////////////////////////////////////////

template <class R>
struct is_result_type;

template <class T>
struct is_result_type<result<T>>: std::true_type
{
};

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_RESULT_HPP_INCLUDED
// #include <boost/leaf/serialization/diagnostics_writer.hpp> // Expanded at line 1434
// >>> #include <boost/leaf/serialization/json_writer.hpp>
#ifndef BOOST_LEAF_SERIALIZATION_JSON_WRITER_HPP_INCLUDED
#define BOOST_LEAF_SERIALIZATION_JSON_WRITER_HPP_INCLUDED

// #line 8 "boost/leaf/serialization/json_writer.hpp"
// #include <boost/leaf/config.hpp> // Expanded at line 19
// #include <boost/leaf/serialization/writer.hpp> // Expanded at line 1440
// #include <boost/leaf/detail/exception_base.hpp> // Expanded at line 1852
// #include <boost/leaf/error.hpp> // Expanded at line 2126
// #include <boost/leaf/common.hpp> // Expanded at line 14

#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
#   include <system_error>
#endif

#ifndef BOOST_LEAF_NO_EXCEPTIONS
#   include <typeinfo>
#endif

namespace boost { namespace leaf {

namespace serialization
{
    template <class Json>
    void to_json(Json & j, error_id x)
    {
        to_json(j, x.value() / 4);
    }

    template <class Json>
    void to_json(Json & j, e_source_location const & x)
    {
        to_json(j["file"], x.file);
        to_json(j["line"], x.line);
        to_json(j["function"], x.function);
    }

    template <class Json>
    void to_json(Json & j, e_errno const & e)
    {
        to_json(j["value"], e.value);
        to_json(j["message"], std::strerror(e.value));
    }

#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
    template <class Json>
    void to_json(Json & j, std::error_code const & ec)
    {
        to_json(j["category"], ec.category().name());
        to_json(j["value"], ec.value());
        to_json(j["message"], ec.message());
    }

    template <class Json>
    void to_json(Json & j, std::error_condition const & ec)
    {
        to_json(j["category"], ec.category().name());
        to_json(j["value"], ec.value());
        to_json(j["message"], ec.message());
    }
#endif

    template <class Json>
    void to_json(Json & j, detail::exception_base const & eb)
    {
        char zstr[1024];
        to_json(j["type"], to_zstr(zstr, eb.get_type_name()));
        char const * what = "N/A";
#ifndef BOOST_LEAF_NO_EXCEPTIONS
        if( std::exception const * ex = dynamic_cast<std::exception const *>(&eb) )
            what = ex->what();
#endif
        to_json(j["what"], what ? what : "<<nullptr>>");
    }

#ifndef BOOST_LEAF_NO_EXCEPTIONS
    template <class Json>
    void to_json(Json & j, std::exception const & ex)
    {
        if( detail::exception_base const * eb = dynamic_cast<detail::exception_base const *>(&ex) )
        {
            char zstr[1024];
            to_json(j["type"], to_zstr(zstr, eb->get_type_name()));
        }
        else
            to_json(j["type"], detail::demangler(typeid(ex).name()).get());
        if( char const * w = ex.what() )
            to_json(j["what"], w);
        else
            to_json(j["what"], "<<nullptr>>");
    }
#endif

    template <class Json>
    void to_json(Json & j, std::exception_ptr const & ep)
    {
        if( ep )
        {
#ifndef BOOST_LEAF_NO_EXCEPTIONS
            try
            {
                std::rethrow_exception(ep);
            }
            catch( detail::exception_base const & eb )
            {
                to_json(j, eb);
                return;
            }
            catch( std::exception const & ex )
            {
                to_json(j, ex);
                return;
            }
            catch( ... )
            {
            }
#endif
            to_json(j["type"], "<<unknown>>");
        }
        else
            to_json(j["type"], "<<empty>>");
        to_json(j["what"], "N/A");
    }

    template <class Json, class E>
    auto to_json(Json & j, E const & e) -> decltype(to_json(j, e.value), void())
    {
        to_json(j["value"], e.value);
    }

    ////////////////////////////////////////

    template <class Json>
    class json_writer: public writer
    {
        Json & j_;

    public:

        explicit json_writer(Json & j) noexcept:
            writer(this),
            j_(j)
        {
        }

        template <class E>
        void write(E const & e)
        {
            char zstr[1024];
            to_json(j_[to_zstr(zstr, get_type_name<E>())], e);
        }
    };

} // namespace serialization

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_SERIALIZATION_JSON_WRITER_HPP_INCLUDED
// #include <boost/leaf/serialization/writer.hpp> // Expanded at line 1440
// >>> #include <boost/leaf/to_variant.hpp>
#ifndef BOOST_LEAF_TO_VARIANT_HPP_INCLUDED
#define BOOST_LEAF_TO_VARIANT_HPP_INCLUDED

// #line 8 "boost/leaf/to_variant.hpp"
#if __cplusplus >= 201703L

// #include <boost/leaf/config.hpp> // Expanded at line 19
// #include <boost/leaf/handle_errors.hpp> // Expanded at line 4112
// #include <boost/leaf/result.hpp> // Expanded at line 6066
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

} } // namespace boost::leaf

#endif // #if __cplusplus >= 201703L

#endif // #ifndef BOOST_LEAF_TO_VARIANT_HPP_INCLUDED

#endif // BOOST_LEAF_HPP_INCLUDED
