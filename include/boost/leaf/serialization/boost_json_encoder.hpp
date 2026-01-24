#ifndef BOOST_LEAF_SERIALIZATION_BOOST_JSON_ENCODER_HPP_INCLUDED
#define BOOST_LEAF_SERIALIZATION_BOOST_JSON_ENCODER_HPP_INCLUDED

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
    struct boost_json_encoder_
    {
        Value & v_;

        template <class T>
        friend auto output(boost_json_encoder_ & e, T const & x) -> decltype(std::declval<Value &>() = x, void())
        {
            e.v_ = x;
        }

        template <class T>
        friend auto output(boost_json_encoder_ & e, T const & x) -> decltype(tag_invoke(std::declval<ValueFromTag>(), std::declval<Value &>(), x), void())
        {
            tag_invoke(ValueFromTag{}, e.v_, x);
        }

        template <class T>
        friend void output_at(boost_json_encoder_ & e, T const & x, char const * name)
        {
            if( e.v_.is_null() )
                e.v_.emplace_object();
            boost_json_encoder_ nested{e.v_.as_object()[name]};
            output(nested, x);
        }
    };

    using boost_json_encoder = boost_json_encoder_<>;
}

} }

#endif
