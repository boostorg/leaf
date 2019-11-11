// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>
#ifdef LEAF_NO_EXCEPTIONS

#include <iostream>

int main()
{
	std::cout << "Unit test not applicable." << std::endl;
	return 0;
}

#else

#include <boost/leaf/context.hpp>
#include <boost/leaf/handle_exception.hpp>
#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int>
struct info
{
	int value;
};

template <class Ctx>
void f( Ctx & ctx )
{
	leaf::context_activator active_context(ctx, leaf::on_deactivation::do_not_propagate);
	throw leaf::exception(std::exception(), info<1>{1});
}

int main()
{
	{
		auto handle_error = []( leaf::error_info const & error )
		{
			return leaf::remote_handle_exception( error,
				[]( leaf::catch_<std::exception>, info<1> x )
				{
					BOOST_TEST(x.value==1);
					return 1;
				},
				[]
				{
					return 2;
				} );
		};

		{
			auto ctx = leaf::make_context(&handle_error);
			try
			{
				f(ctx);
			}
			catch(...)
			{
				int r = ctx.remote_handle_current_exception<int>(
					[&]( leaf::error_info const & error )
					{
						return handle_error(error);
					} );
				BOOST_TEST_EQ(r, 1);
			}
		}

		{
			auto ctx = leaf::make_context(&handle_error);
			try
			{
				f(ctx);
			}
			catch(...)
			{
				int r = ctx.remote_handle_exception<int>(
					std::current_exception(),
					[&]( leaf::error_info const & error )
					{
						return handle_error(error);
					} );
				BOOST_TEST_EQ(r, 1);
			}
		}
	}

	{
		auto handle_error = []( leaf::error_info const & error, int & r )
		{
			return leaf::remote_handle_exception( error,
				[&]( leaf::catch_<std::exception>, info<1> x )
				{
					BOOST_TEST(x.value==1);
					r = 1;
				},
				[&]
				{
					r = 2;
				} );
		};

		{
			auto ctx = leaf::make_context(&handle_error);
			try
			{
				f(ctx);
			}
			catch(...)
			{
				int r = 0;
				ctx.remote_handle_current_exception<void>(
					[&]( leaf::error_info const & error )
					{
						return handle_error(error, r);
					} );
				BOOST_TEST_EQ(r, 1);
			}
		}

		{
			auto ctx = leaf::make_context(&handle_error);
			try
			{
				f(ctx);
			}
			catch(...)
			{
				int r = 0;
				ctx.remote_handle_exception<void>(
					std::current_exception(),
					[&]( leaf::error_info const & error )
					{
						return handle_error(error, r);
					} );
				BOOST_TEST_EQ(r, 1);
			}
		}
	}

	return boost::report_errors();
}

#endif
