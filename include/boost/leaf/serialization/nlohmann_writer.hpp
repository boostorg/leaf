#ifndef BOOST_LEAF_SERIALIZATION_NLOHMANN_WRITER_HPP_INCLUDED
#define BOOST_LEAF_SERIALIZATION_NLOHMANN_WRITER_HPP_INCLUDED

// Copyright 2018-2026 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <utility>

namespace boost { namespace leaf {

namespace serialization
{
    template <class Json>
    struct nlohmann_writer
    {
        Json & j_;

        template <class T>
        friend auto write(nlohmann_writer & w, T const & x) -> decltype(to_json(std::declval<Json &>(), x))
        {
            to_json(w.j_, x);
        }

        template <class T>
        friend void write_nested(nlohmann_writer & w, T const & x, char const * name)
        {
            nlohmann_writer nested{w.j_[name]};
            write(nested, x);
        }
    };

}

} }

#endif // #ifndef BOOST_LEAF_SERIALIZATION_NLOHMANN_WRITER_HPP_INCLUDED
