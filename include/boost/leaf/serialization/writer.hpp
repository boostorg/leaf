#ifndef BOOST_LEAF_SERIALIZATION_WRITER_HPP_INCLUDED
#define BOOST_LEAF_SERIALIZATION_WRITER_HPP_INCLUDED

// Copyright 2018-2026 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>
#include <boost/leaf/detail/type_name.hpp>

#include <type_traits>

namespace boost { namespace leaf {

namespace serialization
{
    class writer
    {
        detail::type_name const type_;

    protected:

        template <class Derived>
        explicit writer(Derived * d) noexcept:
            type_(detail::get_type_name<Derived>())
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
            return type_ == detail::get_type_name<typename std::decay<Derived>::type>() ? static_cast<Derived *>(this) : nullptr;
        }
    };
}

} }

////////////////////////////////////////

namespace boost { namespace leaf {

namespace serialization
{
    template <class T, class E = void>
    struct has_member_value: std::false_type
    {
    };

    template <class T>
    struct has_member_value<T, decltype((void)std::declval<T const &>().value)>: std::true_type
    {
    };

    template <class W, class E>
    typename std::enable_if<has_member_value<E>::value>::type
    write(W & w, E const & e)
    {
        write(w, e.value);
    }
}

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_SERIALIZATION_WRITER_HPP_INCLUDED
