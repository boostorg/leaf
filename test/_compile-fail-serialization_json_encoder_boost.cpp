// Copyright 2018-2025 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/serialization/json_encoder_boost.hpp>
#include <boost/json.hpp>

struct no_tag_invoke {};

struct e_no_tag_invoke
{
    no_tag_invoke value;
};

boost::json::value v;
boost::leaf::serialization::json_encoder_boost e{v};
e_no_tag_invoke x;
auto y = (output(e, x), 0);
