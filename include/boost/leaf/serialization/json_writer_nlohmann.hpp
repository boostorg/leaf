#ifndef BOOST_LEAF_SERIALIZATION_JSON_WRITER_NLOHMANN_HPP_INCLUDED
#define BOOST_LEAF_SERIALIZATION_JSON_WRITER_NLOHMANN_HPP_INCLUDED

// Copyright 2018-2026 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <utility>

namespace boost { namespace leaf {

namespace serialization
{
    template <class Json>
    struct json_writer_nlohmann
    {
        Json & j_;

        template <class T>
        friend auto write(json_writer_nlohmann & w, T const & x) -> decltype(to_json(std::declval<Json &>(), x), void())
        {
            to_json(w.j_, x);
        }

        template <class T>
        friend void write_nested(json_writer_nlohmann & w, T const & x, char const * name)
        {
            json_writer_nlohmann nested{w.j_[name]};
            write(nested, x);
        }
    };
}

} }

#endif
