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
	auto active_context = activate_context(ctx);
	throw leaf::exception(std::exception(), info<1>{1});
}

int main()
{
	{
		using Ctx = leaf::context<leaf::catch_<std::exception>, info<1>>;

		{
			Ctx ctx;
			try
			{
				f(ctx);
			}
			catch(...)
			{
				int r = ctx.handle_current_exception<int>(
					[]( leaf::catch_<std::exception>, info<1> x )
					{
						BOOST_TEST(x.value==1);
						return 1;
					},
					[]
					{
						return 2;
					} );
				BOOST_TEST_EQ(r, 1);
			}
		}

		{
			Ctx ctx;
			try
			{
				f(ctx);
			}
			catch(...)
			{
				int r = ctx.handle_exception<int>(
					std::current_exception(),
					[]( leaf::catch_<std::exception>, info<1> x )
					{
						BOOST_TEST(x.value==1);
						return 1;
					},
					[]
					{
						return 2;
					} );
				BOOST_TEST_EQ(r, 1);
			}
		}
	}

	{
		using Ctx = leaf::context<leaf::catch_<std::exception>, info<1>>;

		{
			Ctx ctx;
			try
			{
				f(ctx);
			}
			catch(...)
			{
				int r = 0;
				ctx.handle_current_exception<void>(
					[&]( leaf::catch_<std::exception>, info<1> x )
					{
						BOOST_TEST(x.value==1);
						r = 1;
					},
					[&]
					{
						r = 2;
					} );
				BOOST_TEST_EQ(r, 1);
			}
		}

		{
			Ctx ctx;
			try
			{
				f(ctx);
			}
			catch(...)
			{
				int r = 0;
				ctx.handle_exception<void>(
					std::current_exception(),
					[&]( leaf::catch_<std::exception>, info<1> x )
					{
						BOOST_TEST(x.value==1);
						r = 1;
					},
					[&]
					{
						r = 2;
					} );
				BOOST_TEST_EQ(r, 1);
			}
		}
	}

	return boost::report_errors();
}

#endif
