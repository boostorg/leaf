#ifndef BOOST_LEAF_SERIALIZATION_JSON_ENCODER_NLOHMANN_HPP_INCLUDED
#define BOOST_LEAF_SERIALIZATION_JSON_ENCODER_NLOHMANN_HPP_INCLUDED

// Copyright 2018-2026 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <utility>

namespace boost { namespace leaf {

namespace serialization
{
    template <class Json>
    struct json_encoder_nlohmann
    {
        Json & j_;

        template <class T>
        friend auto output(json_encoder_nlohmann & e, T const & x) -> decltype(to_json(std::declval<Json &>(), x), void())
        {
            to_json(e.j_, x);
        }

        template <class T>
        friend void output_at(json_encoder_nlohmann & e, T const & x, char const * name)
        {
            json_encoder_nlohmann nested{e.j_[name]};
            output(nested, x);
        }
    };
}

} }

#endif
