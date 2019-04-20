// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/context.hpp>
#include <boost/leaf/handle_error.hpp>
#include <boost/leaf/result.hpp>
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int>
struct info
{
	int value;
};

template <class Ctx>
leaf::result<int> f( Ctx & ctx )
{
	leaf::context_activator active_context(ctx, leaf::on_deactivation::do_not_propagate);
	return leaf::new_error( info<1>{1} );
}

int main()
{
	auto handle_error = []( leaf::error_info const & error )
	{
		return leaf::remote_handle_some( error,
			[]( info<1> x ) -> leaf::result<int>
			{
				BOOST_TEST(x.value==1);
				return 1;
			},
			[]
			{
				return 2;
			} );
	};

	auto ctx = leaf::make_context(&handle_error);

	{
		leaf::result<int> r1 = f(ctx);
		BOOST_TEST(!r1);

		leaf::result<int> r2 = ctx.remote_handle_some( r1,
			[&]( leaf::error_info const & error )
			{
				return handle_error(error);
			} );
		BOOST_TEST_EQ(r2.value(), 1);
	}

	{
		auto r1 = leaf::result<int>(leaf::error_id());
		BOOST_TEST(!r1);

		leaf::result<int> r2 = ctx.remote_handle_some( r1,
			[&]( leaf::error_info const & error )
			{
				return handle_error(error);
			} );
		BOOST_TEST_EQ(r2.value(), 2);
	}

	return boost::report_errors();
}
