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

namespace leaf_detail
{
    template <int S1, int S2, int I, bool = S1 >= S2>
    struct cpp11_prefix
    {
        constexpr static bool check(char const (&)[S1], char const (&)[S2]) noexcept
        {
            return false;
        }
    };
    template <int S1, int S2, int I>
    struct cpp11_prefix<S1, S2, I, true>
    {
        constexpr static bool check(char const (&str)[S1], char const (&prefix)[S2]) noexcept
        {
            return str[I] == prefix[I] && cpp11_prefix<S1, S2, I - 1>::check(str, prefix);
        }
    };
    template <int S1, int S2>
    struct cpp11_prefix<S1, S2, 0, true>
    {
        constexpr static bool check(char const (&str)[S1], char const (&prefix)[S2]) noexcept
        {
            return str[0] == prefix[0];
        }
    };
    template <int S1, int S2>
    constexpr int check_prefix(char const (&str)[S1], char const (&prefix)[S2]) noexcept
    {
        return cpp11_prefix<S1, S2, S2 - 2>::check(str, prefix) ? S2 - 1 : 0;
    }

    ////////////////////////////////////////

    template <int S1, int S2, int I1, int I2, bool = S1 >= S2>
    struct cpp11_suffix
    {
        constexpr static bool check(char const (&)[S1], char const (&)[S2]) noexcept
        {
            return false;
        }
    };
    template <int S1, int S2, int I1, int I2>
    struct cpp11_suffix<S1, S2, I1, I2, true>
    {
        constexpr static bool check(char const (&str)[S1], char const (&suffix)[S2]) noexcept
        {
            return str[I1] == suffix[I2] && cpp11_suffix<S1, S2, I1 - 1, I2 - 1>::check(str, suffix);
        }
    };
    template <int S1, int S2, int I1>
    struct cpp11_suffix<S1, S2, I1, 0, true>
    {
        constexpr static bool check(char const (&str)[S1], char const (&suffix)[S2]) noexcept
        {
            return str[I1] == suffix[0];
        }
    };
    template <int S1, int S2>
    constexpr int check_suffix(char const (&str)[S1], char const (&suffix)[S2]) noexcept
    {
        return cpp11_suffix<S1, S2, S1 - 2, S2 - 2>::check(str, suffix) ? S1 - S2 : 0;
    }
}

struct parsed_name
{
    char const * name;
    int len;
    parsed_name(char const * name, int len) noexcept:
        name(name),
        len(len)
    {
    }
    template <int S>
    parsed_name(char const(&name)[S]) noexcept:
        name(name),
        len(S-1)
    {
    }
    bool parse_success() const noexcept
    {
        return name[len] != 0;
    }
    template <class CharT, class Traits>
    friend std::ostream & operator<<(std::basic_ostream<CharT, Traits> & os, parsed_name const & pn)
    {
        return os.write(pn.name, pn.len);
    }
};

template <class Name>
parsed_name parse_name()
{
    // Workaround for older gcc compilers where __PRETTY_FUNCTION__ is not constexpr.
    // Instead of evaluating constexpr int x = f(__PRETTY_FUNCTION__), which fails,
    // we evaluate int const x = f(__PRETTY_FUNCTION__). Then we enforce compile-time
    // execution by evaluating sizeof(char[1 + x]) -1.
#define BOOST_LEAF_PARSE_PF(prefix, suffix) \
    { \
        if( int const s = leaf_detail::check_suffix(BOOST_LEAF_PRETTY_FUNCTION, suffix) ) \
            if( int const p = leaf_detail::check_prefix(BOOST_LEAF_PRETTY_FUNCTION, prefix) ) \
                return parsed_name(BOOST_LEAF_PRETTY_FUNCTION + sizeof(char[1 + p]) - 1, sizeof(char[1 + s - p]) - 1); \
    }
    // clang style:
    BOOST_LEAF_PARSE_PF( "parsed_name boost::leaf::parse_name() [Name = ", "]");
    // old clang style:
    BOOST_LEAF_PARSE_PF( "boost::leaf::parsed_name boost::leaf::parse_name() [Name = ", "]");
    // gcc style:
    BOOST_LEAF_PARSE_PF( "boost::leaf::parsed_name boost::leaf::parse_name() [with Name = ", "]");
    // msvc style, __cdecl, struct/class/enum:
    BOOST_LEAF_PARSE_PF( "struct boost::leaf::parsed_name __cdecl boost::leaf::parse_name<struct ", ">(void)");
    BOOST_LEAF_PARSE_PF( "struct boost::leaf::parsed_name __cdecl boost::leaf::parse_name<class ", ">(void)");
    BOOST_LEAF_PARSE_PF( "struct boost::leaf::parsed_name __cdecl boost::leaf::parse_name<enum ", ">(void)");
    // msvc style, __stdcall, struct/class/enum:
    BOOST_LEAF_PARSE_PF( "struct boost::leaf::parsed_name __stdcall boost::leaf::parse_name<struct ", ">(void)");
    BOOST_LEAF_PARSE_PF( "struct boost::leaf::parsed_name __stdcall boost::leaf::parse_name<class ", ">(void)");
    BOOST_LEAF_PARSE_PF( "struct boost::leaf::parsed_name __stdcall boost::leaf::parse_name<enum ", ">(void)");
    // msvc style, __fastcall, struct/class/enum:
    BOOST_LEAF_PARSE_PF( "struct boost::leaf::parsed_name __fastcall boost::leaf::parse_name<struct ", ">(void)");
    BOOST_LEAF_PARSE_PF( "struct boost::leaf::parsed_name __fastcall boost::leaf::parse_name<class ", ">(void)");
    BOOST_LEAF_PARSE_PF( "struct boost::leaf::parsed_name __fastcall boost::leaf::parse_name<enum ", ">(void)");
#undef BOOST_LEAF_PARSE_PF

    // Unrecognized __PRETTY_FUNCTION__/__FUNSIG__ format, return as-is. Note, parsing is done at compile-time.
    return parsed_name(BOOST_LEAF_PRETTY_FUNCTION);
}

} }

////////////////////////////////////////

namespace boost { namespace leaf {

namespace leaf_detail
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

#endif
