// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/error.hpp>

struct t0 { int value; };
struct t1 { int value(); };
struct t2 { };
struct t3 { };

namespace boost { namespace leaf {
	template <> struct is_e_type<t3> { static constexpr bool value = true; };
} }

namespace leaf = boost::leaf;

int main()
{
	static_assert(leaf::is_e_type<t0>::value,"t0");
	static_assert(!leaf::is_e_type<t1>::value,"t1");
	static_assert(!leaf::is_e_type<t2>::value,"t2");
	static_assert(leaf::is_e_type<t3>::value,"t3");
	return 0;
}
