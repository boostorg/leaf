// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/preload.hpp>
#include <boost/leaf/handle_error.hpp>
#include <boost/leaf/result.hpp>
#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int>
struct info
{
	int value;
};

leaf::error_id g()
{
	auto load = leaf::preload( info<42>{42}, info<-42>{-42} );
	return leaf::new_error();
}

leaf::error_id f()
{
	return g();
}

int main()
{
	int r = leaf::try_handle_all(
		[]() -> leaf::result<int>
		{
			return f();
		},
		[]( info<42> const & i42, leaf::diagnostic_info const & di )
		{
			BOOST_TEST_EQ(i42.value, 42);
			std::stringstream ss; ss << di;
			std::string s = ss.str();
			std::cout << s;
#ifdef BOOST_LEAF_DISCARD_UNEXPECTED
			BOOST_TEST(s.find("BOOST_LEAF_DISCARD_UNEXPECTED")!=s.npos);
#else
			BOOST_TEST(s.find("info<-42>")!=s.npos);
#endif
			return 1;
		},
		[]
		{
			return 2;
		} );
	BOOST_TEST_EQ(r, 1);
	return boost::report_errors();
}
