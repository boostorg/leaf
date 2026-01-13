#ifndef BOOST_LEAF_SERIALIZATION_JSON_WRITER_BOOST_HPP_INCLUDED
#define BOOST_LEAF_SERIALIZATION_JSON_WRITER_BOOST_HPP_INCLUDED

// Copyright 2018-2026 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <utility>

namespace boost { namespace json {

class value;
struct value_from_tag;

} }

namespace boost { namespace leaf {

namespace serialization
{
    template <class Value = boost::json::value, class ValueFromTag = boost::json::value_from_tag>
    struct json_writer_boost_
    {
        Value & v_;

        template <class T>
        friend auto write(json_writer_boost_ & w, T const & x) -> decltype(std::declval<Value &>() = x, void())
        {
            w.v_ = x;
        }

        template <class T>
        friend auto write(json_writer_boost_ & w, T const & x) -> decltype(tag_invoke(std::declval<ValueFromTag>(), std::declval<Value &>(), x), void())
        {
            tag_invoke(ValueFromTag{}, w.v_, x);
        }

        template <class T>
        friend void write_nested(json_writer_boost_ & w, T const & x, char const * name)
        {
            if( w.v_.is_null() )
                w.v_.emplace_object();
            json_writer_boost_ nested{w.v_.as_object()[name]};
            write(nested, x);
        }
    };

    using json_writer_boost = json_writer_boost_<>;
}

} }

#endif
