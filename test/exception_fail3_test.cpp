// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/exception.hpp>

namespace leaf = boost::leaf;

struct my_exception: std::exception { };

struct derives_from_std_exception: std::exception { };

void f()
{
	throw leaf::exception(my_exception{}, derives_from_std_exception{});
}
