// Copyright 2018-2025 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/serialization/json_writer.hpp>
#include "nlohmann/json.hpp"

struct no_to_json {};

struct e_no_to_json
{
    no_to_json value;
};

nlohmann::json j;
e_no_to_json e;
auto x = (boost::leaf::serialization::to_json(j, e), 0);
