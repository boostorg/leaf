#ifndef BOOST_LEAF_SERIALIZATION_WRITER_HPP_INCLUDED
#define BOOST_LEAF_SERIALIZATION_WRITER_HPP_INCLUDED

// Copyright 2018-2026 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>
#include <boost/leaf/serialization/type_name.hpp>

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
