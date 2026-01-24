#ifndef BOOST_LEAF_SERIALIZATION_NLOHMANN_JSON_ENCODER_HPP_INCLUDED
#define BOOST_LEAF_SERIALIZATION_NLOHMANN_JSON_ENCODER_HPP_INCLUDED

// Copyright 2018-2026 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <utility>

namespace boost { namespace leaf {

namespace serialization
{
    template <class Json>
    struct nlohmann_json_encoder
    {
        Json & j_;

        template <class T>
        friend auto output(nlohmann_json_encoder & e, T const & x) -> decltype(to_json(std::declval<Json &>(), x), void())
        {
            to_json(e.j_, x);
        }

        template <class T>
        friend void output_at(nlohmann_json_encoder & e, T const & x, char const * name)
        {
            nlohmann_json_encoder nested{e.j_[name]};
            output(nested, x);
        }
    };
}

} }

#endif
