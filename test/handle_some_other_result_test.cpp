// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/handle_some.hpp>
#include "_test_res.hpp"
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int> struct info { int value; };

res<int,std::error_code> f( bool succeed )
{
	if( succeed )
		return 42;
	else
		return make_error_code(errc_a::a0);
}

res<int,std::error_code> g( bool succeed )
{
	if( auto r = f(succeed) )
		return r;
	else
		return leaf::error_id(r.error()).load(info<42>{42});
}

int main()
{
	{
		res<int,std::error_code> r = leaf::handle_some(
			[ ]
			{
				return g(true);
			} );
		BOOST_TEST(r);
		BOOST_TEST_EQ(r.value(), 42);
	}
	{
		int called = 0;
		res<int,std::error_code> r = leaf::handle_some(
			[&]
			{
				auto r = g(false);
				BOOST_TEST(!r);
				auto ec = r.error();
				BOOST_TEST_EQ(ec.message(), "LEAF error, use with leaf::handle_some or leaf::handle_all.");
				BOOST_TEST(!std::strcmp(ec.category().name(),"LEAF error, use with leaf::handle_some or leaf::handle_all."));
				return r;
			},
			[&]( info<42> const & x, leaf::match<leaf::condition<cond_x>, cond_x::x00> ec )
			{
				called = 1;
				BOOST_TEST_EQ(x.value, 42);
				return ec.value();
			} );
		BOOST_TEST(!r);
		BOOST_TEST_EQ(r.error(), make_error_code(errc_a::a0));
		BOOST_TEST(called);
	}
	return boost::report_errors();
}
