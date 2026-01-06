#ifndef BOOST_LEAF_SERIALIZATION_WRITER_HPP_INCLUDED
#define BOOST_LEAF_SERIALIZATION_WRITER_HPP_INCLUDED

// Copyright 2018-2025 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/demangle.hpp>

#include <type_traits>

namespace boost { namespace leaf {

namespace serialization
{
class writer
{
    parsed const type_;

protected:

    template <class Derived>
    explicit writer(Derived * d) noexcept:
        type_(parse<Derived>())
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
        return type_ == parse<typename std::decay<Derived>::type>() ? static_cast<Derived *>(this) : nullptr;
    }
}; // class writer

template <class W, class E>
typename std::enable_if<std::is_base_of<writer, W>::value>::type
serialize(W &, E const &)
{
}
} // namespace serialization

namespace detail
{
    template <class>
    struct dependent_writer
    {
        using type = serialization::writer;
    };
} // namespace detail

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_SERIALIZATION_WRITER_HPP_INCLUDED
