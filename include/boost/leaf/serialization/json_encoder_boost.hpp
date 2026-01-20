#ifndef BOOST_LEAF_SERIALIZATION_JSON_ENCODER_BOOST_HPP_INCLUDED
#define BOOST_LEAF_SERIALIZATION_JSON_ENCODER_BOOST_HPP_INCLUDED

// Copyright 2018-2026 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <type_traits>

namespace boost { namespace json {

class value;

template <class T>
void value_from(T &&, value &);

} }

namespace boost { namespace leaf {

namespace serialization
{
    template <class Value = boost::json::value>
    struct json_encoder_boost_
    {
        Value & v_;

        template <class Encoder, class T, class... Deprioritize, class = typename std::enable_if<std::is_same<Encoder, json_encoder_boost_>::value>::type>
        friend void output(Encoder & e, T const & x, Deprioritize...)
        {
            boost::json::value_from(x, e.v_);
        }

        template <class T>
        friend void output_at(json_encoder_boost_ & e, T const & x, char const * name)
        {
            if( e.v_.is_null() )
                e.v_.emplace_object();
            json_encoder_boost_ nested{e.v_.as_object()[name]};
            output(nested, x);
        }
    };

    using json_encoder_boost = json_encoder_boost_<>;
}

} }

#endif
