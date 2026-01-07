#ifndef BOOST_LEAF_SERIALIZATION_TYPE_NAME_HPP_INCLUDED
#define BOOST_LEAF_SERIALIZATION_TYPE_NAME_HPP_INCLUDED

// Copyright 2018-2026 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/demangle.hpp>

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
