// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

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
	leaf::context_activator active_context(ctx, leaf::context_activator::on_deactivation::do_not_propagate);
	return leaf::new_error( info<1>{1} );
}

int main()
{
	auto handle_error = [ ]( leaf::error_info const & error )
	{
		return leaf::remote_handle_some( error,
			[ ]( info<1> x ) -> leaf::result<int>
			{
				BOOST_TEST(x.value==1);
				return 1;
			} );
	};

	auto ctx = leaf::make_context(&handle_error);
	leaf::result<int> r = f(ctx);

	leaf::result<int> rr = ctx.remote_handle_some( r,
		[&]( leaf::error_info const & error )
		{
			return handle_error(error);
		} );
	BOOST_TEST_EQ(rr.value(), 1);

	return boost::report_errors();
}
