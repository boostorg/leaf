// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>
#ifdef LEAF_NO_THREADS

#include <iostream>

int main()
{
	std::cout << "Unit test not applicable." << std::endl;
	return 0;
}

#else

#include <boost/leaf/preload.hpp>
#include <boost/leaf/handle_error.hpp>
#include <boost/leaf/result.hpp>
#include <deque>
#include <functional>
#include <future>
#include "lightweight_test.hpp"

namespace asio_simulator
{
	struct io_context
	{
		io_context( io_context const & ) = delete;
		io_context & operator=( io_context const & ) = delete;

		io_context() noexcept
		{
		}

		void run() noexcept
		{
			while (!execution_queue.empty())
			{
				auto e = std::move(execution_queue.front());
				execution_queue.pop_front();
				e();
				std::async(std::launch::async, [this]{ run(); }).wait(); // Force new thread
			}
		}

		std::deque<std::function<void()>> execution_queue;
	};

	template <class F>
	void post( io_context & io_context, F && f )
	{
		io_context.execution_queue.emplace_back(std::forward<F>(f));
	}
}

namespace asio = asio_simulator;
namespace leaf = boost::leaf;

struct io_task_context
{
	std::shared_ptr<leaf::polymorphic_context> err_ctx;
	asio_simulator::io_context io_ctx;

	template <class F>
	void post( F && f )
	{
		asio::post( io_ctx,
			[=]() mutable
			{
				leaf::context_activator active_context(*err_ctx, leaf::on_deactivation::do_not_propagate);
				f();
			} );
	}
};

enum class fail_where
{
	nowhere,
	f,
	g,
	op_a_start,
	op_b_start_before_op_a_start,
	op_b_start_after_successful_op_a,
	op_b_cont_before_op_a_start,
	op_b_cont_after_successful_op_a
};

struct loc
{
	fail_where value;
};

template <int>
struct info
{
	int value;
};

leaf::result<int> f( fail_where fw )
{
	if( fw == fail_where::f )
		return leaf::new_error( loc{fail_where::f} );

	return 21;
}

leaf::result<int> g( fail_where fw )
{
	auto load = leaf::preload( info<1>{} );
	if( fw == fail_where::g )
		return leaf::new_error( loc{fail_where::g} );

	return f(fw);
}

struct op_a
{
	template <class H>
	static void start( io_task_context  & ioc, fail_where fw, H h )
	{
		auto load = leaf::preload( info<2>{} );
		if( fw == fail_where::op_a_start )
		{
			auto error = leaf::new_error(loc{fail_where::op_a_start});
			ioc.post(
				[=]() mutable
				{
					return h(error);
				} );
			return;
		}

		ioc.post(
			[=]() mutable
			{
				auto load = leaf::preload( info<3>{} );
				h(g(fw));
			} );
	}
};

struct op_b
{
	template <class H>
	static void start( io_task_context  & ioc, fail_where fw, H h )
	{
		auto load = leaf::preload( info<4>{} );
		if( fw == fail_where::op_b_start_before_op_a_start )
		{
			auto error = leaf::new_error(loc{fail_where::op_b_start_before_op_a_start});
			ioc.post(
				[=]() mutable
				{
					return h(error);
				} );
			return;
		}

		op_a::start( ioc, fw,
			[=, &ioc]( leaf::result<int> && a1 ) mutable
			{
				if( !a1 )
					return h(std::forward<leaf::result<int>>(a1));

				auto load = leaf::preload( info<5>{} );
				if( fw == fail_where::op_b_start_after_successful_op_a )
					return h( leaf::new_error(loc{fail_where::op_b_start_after_successful_op_a}) );

				cont(ioc, fw, *a1, h);
			} );
	}

	template <class H>
	static void cont( io_task_context  & ioc, fail_where fw, int a1, H h )
	{
		auto load = leaf::preload( info<6>{} );
		if( fw == fail_where::op_b_cont_before_op_a_start )
			return h( leaf::new_error(loc{fail_where::op_b_cont_before_op_a_start}) );

		op_a::start( ioc, fw,
			[=]( leaf::result<int> && a2 ) mutable
			{
				if( !a2 )
					return h(std::forward<leaf::result<int>>(a2));

				auto load = leaf::preload( info<7>{} );
				if( fw == fail_where::op_b_cont_after_successful_op_a )
					return h( leaf::new_error(loc{fail_where::op_b_cont_after_successful_op_a}) );

				h(a1 + *a2);
			} );
	}
};

///////////////////////////////

int main()
{
	auto handle_error = []( leaf::error_info const & error )
	{
		return leaf::remote_handle_all( error,
			[]( leaf::match<loc,fail_where::f, fail_where::g> wh, info<1> const * i1, info<2> const * i2, info<3> const * i3, info<4> const * i4, info<5> const * i5, info<6> const * i6, info<7> const * i7 )
			{
				BOOST_TEST(i1 != 0);
				BOOST_TEST(i2 == 0);
				BOOST_TEST(i3 != 0);
				BOOST_TEST(i4 == 0);
				BOOST_TEST(i5 == 0);
				BOOST_TEST(i6 == 0);
				BOOST_TEST(i7 == 0);
				return -int(wh.value());
			},
			[]( leaf::match<loc,fail_where::op_a_start> wh, info<1> const * i1, info<2> const * i2, info<3> const * i3, info<4> const * i4, info<5> const * i5, info<6> const * i6, info<7> const * i7 )
			{
				BOOST_TEST(i1 == 0);
				BOOST_TEST(i2 != 0);
				BOOST_TEST(i3 == 0);
				BOOST_TEST(i4 != 0);
				BOOST_TEST(i5 == 0);
				BOOST_TEST(i6 == 0);
				BOOST_TEST(i7 == 0);
				return -int(wh.value());
			},
			[]( leaf::match<loc,fail_where::op_b_start_before_op_a_start> wh, info<1> const * i1, info<2> const * i2, info<3> const * i3, info<4> const * i4, info<5> const * i5, info<6> const * i6, info<7> const * i7 )
			{
				BOOST_TEST(i1 == 0);
				BOOST_TEST(i2 == 0);
				BOOST_TEST(i3 == 0);
				BOOST_TEST(i4 != 0);
				BOOST_TEST(i5 == 0);
				BOOST_TEST(i6 == 0);
				BOOST_TEST(i7 == 0);
				return -int(wh.value());
			},
			[]( leaf::match<loc,fail_where::op_b_start_after_successful_op_a> wh, info<1> const * i1, info<2> const * i2, info<3> const * i3, info<4> const * i4, info<5> const * i5, info<6> const * i6, info<7> const * i7 )
			{
				BOOST_TEST(i1 == 0);
				BOOST_TEST(i2 == 0);
				BOOST_TEST(i3 != 0);
				BOOST_TEST(i4 == 0);
				BOOST_TEST(i5 != 0);
				BOOST_TEST(i6 == 0);
				BOOST_TEST(i7 == 0);
				return -int(wh.value());
			},
			[]( leaf::match<loc,fail_where::op_b_cont_before_op_a_start> wh, info<1> const * i1, info<2> const * i2, info<3> const * i3, info<4> const * i4, info<5> const * i5, info<6> const * i6, info<7> const * i7 )
			{
				BOOST_TEST(i1 == 0);
				BOOST_TEST(i2 == 0);
				BOOST_TEST(i3 != 0);
				BOOST_TEST(i4 == 0);
				BOOST_TEST(i5 != 0);
				BOOST_TEST(i6 != 0);
				BOOST_TEST(i7 == 0);
				return -int(wh.value());
			},
			[]( leaf::match<loc,fail_where::op_b_cont_after_successful_op_a> wh, info<1> const * i1, info<2> const * i2, info<3> const * i3, info<4> const * i4, info<5> const * i5, info<6> const * i6, info<7> const * i7 )
			{
				BOOST_TEST(i1 == 0);
				BOOST_TEST(i2 == 0);
				BOOST_TEST(i3 != 0);
				BOOST_TEST(i4 == 0);
				BOOST_TEST(i5 == 0);
				BOOST_TEST(i6 == 0);
				BOOST_TEST(i7 != 0);
				return -int(wh.value());
			},
			[]
			{
				return 0;
			} );
	};

	auto try_block =
		[&]( fail_where fw )
		{
			io_task_context ioc;
			ioc.err_ctx = leaf::make_shared_context(&handle_error);

			leaf::result<int> res;
			op_b::start( ioc, fw,
				[&]( leaf::result<int> && answer ) mutable
				{
					res = leaf::make_continuation_result(answer, ioc.err_ctx);
				} );
			if( res )
				ioc.io_ctx.run();
			return res;
		};

	{
		int r = leaf::remote_try_handle_all(
			[&]
			{
				return try_block(fail_where::nowhere);
			},
			[&]( leaf::error_info const & error )
			{
				return handle_error(error);
			} );
		BOOST_TEST_EQ(r, 42);
	}

	{
		int r = leaf::remote_try_handle_all(
			[&]
			{
				return try_block(fail_where::f);
			},
			[&]( leaf::error_info const & error )
			{
				return handle_error(error);
			} );
		BOOST_TEST_EQ(r, -int(fail_where::f));
	}

	{
		int r = leaf::remote_try_handle_all(
			[&]
			{
				return try_block(fail_where::g);
			},
			[&]( leaf::error_info const & error )
			{
				return handle_error(error);
			} );
		BOOST_TEST_EQ(r, -int(fail_where::g));
	}

	{
		int r = leaf::remote_try_handle_all(
			[&]
			{
				return try_block(fail_where::op_a_start);
			},
			[&]( leaf::error_info const & error )
			{
				return handle_error(error);
			} );
		BOOST_TEST_EQ(r, -int(fail_where::op_a_start));
	}

	{
		int r = leaf::remote_try_handle_all(
			[&]
			{
				return try_block(fail_where::op_b_start_before_op_a_start);
			},
			[&]( leaf::error_info const & error )
			{
				return handle_error(error);
			} );
		BOOST_TEST_EQ(r, -int(fail_where::op_b_start_before_op_a_start));
	}

	{
		int r = leaf::remote_try_handle_all(
			[&]
			{
				return try_block(fail_where::op_b_start_after_successful_op_a);
			},
			[&]( leaf::error_info const & error )
			{
				return handle_error(error);
			} );
		BOOST_TEST_EQ(r, -int(fail_where::op_b_start_after_successful_op_a));
	}

	{
		int r = leaf::remote_try_handle_all(
			[&]
			{
				return try_block(fail_where::op_b_cont_before_op_a_start);
			},
			[&]( leaf::error_info const & error )
			{
				return handle_error(error);
			} );
		BOOST_TEST_EQ(r, -int(fail_where::op_b_cont_before_op_a_start));
	}

	{
		int r = leaf::remote_try_handle_all(
			[&]
			{
				return try_block(fail_where::op_b_cont_after_successful_op_a);
			},
				[&]( leaf::error_info const & error )
			{
				return handle_error(error);
			} );
		BOOST_TEST_EQ(r, -int(fail_where::op_b_cont_after_successful_op_a));
	}

	return boost::report_errors();
}

#endif
