#ifndef BOOST_LEAF_DETAIL_DEMANGLE_HPP_INCLUDED
#define BOOST_LEAF_DETAIL_DEMANGLE_HPP_INCLUDED

// Copyright 2018-2023 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This file is based on boost::core::demangle
//
// Copyright 2014 Peter Dimov
// Copyright 2014 Andrey Semashev
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt

#include <boost/leaf/config.hpp>

#include <iosfwd>
#include <cstring>

namespace boost { namespace leaf {

namespace leaf_detail
{
    template <int N>
    BOOST_LEAF_CONSTEXPR inline int check_prefix(char const * t, char const (&prefix)[N]) noexcept
    {
        return std::strncmp(t, prefix, sizeof(prefix)-1) == 0 ? sizeof(prefix) - 1 : 0;
    }
}

template <class Name>
inline char const * type_str()
{
#if defined(__clang__)
    BOOST_LEAF_ASSERT(leaf_detail::check_prefix(__PRETTY_FUNCTION__, "const char *boost::leaf::type_str() [Name = ") == 44);
    return __PRETTY_FUNCTION__ + 44;
#elif defined(__GNUC__)
    BOOST_LEAF_ASSERT(leaf_detail::check_prefix(__PRETTY_FUNCTION__, "const char* boost::leaf::type_str() [with Name = ") == 49);
    return __PRETTY_FUNCTION__ + 49;
#elif defined _MSC_VER
    if( char const * p = std::strstr(__FUNCSIG__, "boost::leaf::type_str<") )
        return p + 22;
    else
        return __FUNCSIG__;
#else
    if( int clang_style = leaf_detail::check_prefix(__PRETTY_FUNCTION__, "const char *boost::leaf::type_str() [Name = ") )
        return __PRETTY_FUNCTION__ + clang_style;
    else if( int gcc_style = leaf_detail::check_prefix(__PRETTY_FUNCTION__, "const char* boost::leaf::type_str() [with Name = ") )
        return __PRETTY_FUNCTION__ + gcc_style;
    else
        return __PRETTY_FUNCTION__;
#endif
}

template <class Name, class CharT, class Traits>
inline std::ostream & print_type_str(std::basic_ostream<CharT, Traits> & os)
{
    if( char const * t = type_str<Name>() )
    {
        char const * end = std::strchr(t, 0);
#if defined(__clang__) || defined(__GNUC__)
        BOOST_LEAF_ASSERT(end != t);
        BOOST_LEAF_ASSERT(end[-1] == ']');
        return os.write(t, end - t - 1) << ": ";
#elif defined(_MSC_VER)
        BOOST_LEAF_ASSERT(end - t > 7);
        BOOST_LEAF_ASSERT(std::memcmp(end - 7, ">(void)", 7) == 0);
        if( *t == 's' )
        {
            BOOST_LEAF_ASSERT(std::memcmp(t + 1, "truct ", 6) == 0);
            t += 7;
        }
        else if( *t == 'c' )
        {
            BOOST_LEAF_ASSERT(std::memcmp(t + 1, "lass ", 5) == 0);
            t += 6;
        }
        else if( *t == 'e' )
        {
            BOOST_LEAF_ASSERT(std::memcmp(t + 1, "num ", 4) == 0);
            t += 5;
        }
        return os.write(t, end - t - 7) << ": ";
#else
        if( end != t && end[-1] == ']')
            return os.write(t, end - t - 1) << ": ";
        else
            return os << t << ": ";
#endif
    }
    else
        return os << "no name: ";
}

} }

////////////////////////////////////////

// __has_include is currently supported by GCC and Clang. However GCC 4.9 may have issues and
// returns 1 for 'defined( __has_include )', while '__has_include' is actually not supported:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=63662
#if defined(__has_include) && (!defined(__GNUC__) || defined(__clang__) || (__GNUC__ + 0) >= 5)
#   if __has_include(<cxxabi.h>)
#       define BOOST_LEAF_HAS_CXXABI_H
#   endif
#elif defined( __GLIBCXX__ ) || defined( __GLIBCPP__ )
#   define BOOST_LEAF_HAS_CXXABI_H
#endif

#if defined( BOOST_LEAF_HAS_CXXABI_H )
#   include <cxxabi.h>
//  For some archtectures (mips, mips64, x86, x86_64) cxxabi.h in Android NDK is implemented by gabi++ library
//  (https://android.googlesource.com/platform/ndk/+/master/sources/cxx-stl/gabi++/), which does not implement
//  abi::__cxa_demangle(). We detect this implementation by checking the include guard here.
#   if defined( __GABIXX_CXXABI_H__ )
#       undef BOOST_LEAF_HAS_CXXABI_H
#   else
#       include <cstdlib>
#       include <cstddef>
#   endif
#endif

#if BOOST_LEAF_CFG_STD_STRING

#include <string>

namespace boost { namespace leaf {

namespace leaf_detail
{
    inline char const * demangle_alloc( char const * name ) noexcept;
    inline void demangle_free( char const * name ) noexcept;

    class scoped_demangled_name
    {
    private:

        char const * m_p;

    public:

        explicit scoped_demangled_name( char const * name ) noexcept :
            m_p( demangle_alloc( name ) )
        {
        }

        ~scoped_demangled_name() noexcept
        {
            demangle_free( m_p );
        }

        char const * get() const noexcept
        {
            return m_p;
        }

        scoped_demangled_name( scoped_demangled_name const& ) = delete;
        scoped_demangled_name& operator= ( scoped_demangled_name const& ) = delete;
    };

#ifdef BOOST_LEAF_HAS_CXXABI_H

    inline char const * demangle_alloc( char const * name ) noexcept
    {
        int status = 0;
        std::size_t size = 0;
        return abi::__cxa_demangle( name, NULL, &size, &status );
    }

    inline void demangle_free( char const * name ) noexcept
    {
        std::free( const_cast< char* >( name ) );
    }

    inline std::string demangle( char const * name )
    {
        scoped_demangled_name demangled_name( name );
        char const * p = demangled_name.get();
        if( !p )
            p = name;
        return p;
    }

#else

    inline char const * demangle_alloc( char const * name ) noexcept
    {
        return name;
    }

    inline void demangle_free( char const * ) noexcept
    {
    }

    inline char const * demangle( char const * name )
    {
        return name;
    }

#endif
}

} }

#else

namespace boost { namespace leaf {

namespace leaf_detail
{
    inline char const * demangle( char const * name )
    {
        return name;
    }
}

} }

#endif

#ifdef BOOST_LEAF_HAS_CXXABI_H
#   undef BOOST_LEAF_HAS_CXXABI_H
#endif

#endif
