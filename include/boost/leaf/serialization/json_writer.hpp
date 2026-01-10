#ifndef BOOST_LEAF_SERIALIZATION_JSON_WRITER_HPP_INCLUDED
#define BOOST_LEAF_SERIALIZATION_JSON_WRITER_HPP_INCLUDED

// Copyright 2018-2026 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>
#include <boost/leaf/serialization/writer.hpp>

#include <type_traits>
#include <utility>

namespace boost { namespace leaf {

namespace serialization
{
    template <class Json, class E, class = void>
    struct has_to_json : std::false_type {};

    template <class Json, class E>
    struct has_to_json<Json, E, decltype(to_json(std::declval<Json &>(), std::declval<E const &>()), void())> : std::true_type {};

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
        friend void write_nested(json_writer & w, E const & e, char const * name)
        {
            json_writer nested(w.j_[name]);
            write(nested, e);
        }

        template <class E>
        friend typename std::enable_if<has_to_json<Json, E>::value>::type write(json_writer & w, E const & e)
        {
            to_json(w.j_, e);
        }
    };

} // namespace serialization

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_SERIALIZATION_JSON_WRITER_HPP_INCLUDED
