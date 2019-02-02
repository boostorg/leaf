// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/preload.hpp>
#include <boost/leaf/handle_all.hpp>
#include <boost/leaf/result.hpp>
#include <deque>
#include <functional>
#include "boost/core/lightweight_test.hpp"

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
			}
		}

		std::deque<std::function<void()>> execution_queue;
	};

	template <typename F>
	void post( io_context &io_context, F&& f)
	{
		io_context.execution_queue.emplace_back(std::forward<F>(f));
	}
}

namespace asio = asio_simulator;
namespace leaf = boost::leaf;

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

template <fail_where>
struct loc
{
	int value;
};

template <int>
struct info
{
	int value;
};

leaf::result<int> f( fail_where fw )
{
	if( fw == fail_where::f )
		return leaf::new_error( loc<fail_where::f>{} );

	return 21;
}

leaf::result<int> g( fail_where fw )
{
	auto propagate = leaf::preload( info<1>{} );
	if( fw == fail_where::g )
		return leaf::new_error( loc<fail_where::g>{} );

	return f(fw);
}

struct op_a
{
	template <class H>
	static void start( asio::io_context & ioc, fail_where fw, H h )
	{
		auto propagate = leaf::preload( info<2>{} );
		if( fw == fail_where::op_a_start )
			return h( leaf::new_error(loc<fail_where::op_a_start>{}) );

		asio::post( ioc,
			[=]() mutable
			{
				auto propagate = leaf::preload( info<3>{} );
				h(g(fw));
			} );
	}
};

struct op_b
{
	template <class H>
	static void start( asio::io_context & ioc, fail_where fw, H h )
	{
		auto propagate = leaf::preload( info<4>{} );
		if( fw == fail_where::op_b_start_before_op_a_start )
			return h( leaf::new_error(loc<fail_where::op_b_start_before_op_a_start>{}) );

		op_a::start( ioc, fw,
			[=, &ioc]( leaf::result<int> && a1 ) mutable
			{
				if( !a1 )
					return h(std::forward<leaf::result<int>>(a1));

				auto propagate = leaf::preload( info<5>{} );
				if( fw == fail_where::op_b_start_after_successful_op_a )
					return h( leaf::new_error(loc<fail_where::op_b_start_after_successful_op_a>{}) );

				cont(ioc, fw, *a1, h);
			} );
	}

	template <class H>
	static void cont( asio::io_context & ioc, fail_where fw, int a1, H h )
	{
		auto propagate = leaf::preload( info<6>{} );
		if( fw == fail_where::op_b_cont_before_op_a_start )
			return h( leaf::new_error(loc<fail_where::op_b_cont_before_op_a_start>{}) );

		op_a::start( ioc, fw,
			[=]( leaf::result<int> && a2 ) mutable
			{
				if( !a2 )
					return h(std::forward<leaf::result<int>>(a2));

				auto propagate = leaf::preload( info<7>{} );
				if( fw == fail_where::op_b_cont_after_successful_op_a )
					return h( leaf::new_error(loc<fail_where::op_b_cont_after_successful_op_a>{}) );

				h(a1 + *a2);
			} );
	}
};

///////////////////////////////

int main()
{
	auto try_block =
		[ ]( fail_where fw )
		{
			asio::io_context ioc;
			leaf::result<int> res;
			op_b::start( ioc, fw,
				[&]( leaf::result<int> && answer ) mutable
				{
					res = continuation_result(std::move(answer));
				} );
			if( res )
				ioc.run();
			return res;
		};

	{
		int r = leaf::handle_all(
			[&]
			{
				return try_block(fail_where::f);
			},
			[ ]( loc<fail_where::f>, info<1> const * i1, info<2> const * i2, info<3> const * i3, info<4> const * i4, info<5> const * i5, info<6> const * i6, info<7> const * i7 )
			{
				BOOST_TEST(i1 != 0);
				BOOST_TEST(i2 == 0);
				BOOST_TEST(i3 != 0);
				BOOST_TEST(i4 == 0);
				BOOST_TEST(i5 == 0);
				BOOST_TEST(i6 == 0);
				BOOST_TEST(i7 == 0);
				return 1;
			},
			[ ]
			{
				return 2;
			} );
		BOOST_TEST_EQ(r, 1);
	}

	{
		int r = leaf::handle_all(
			[&]
			{
				return try_block(fail_where::op_a_start);
			},
			[ ]( loc<fail_where::op_a_start>, info<1> const * i1, info<2> const * i2, info<3> const * i3, info<4> const * i4, info<5> const * i5, info<6> const * i6, info<7> const * i7 )
			{
				BOOST_TEST(i1 == 0);
				BOOST_TEST(i2 != 0);
				BOOST_TEST(i3 == 0);
				BOOST_TEST(i4 != 0);
				BOOST_TEST(i5 == 0);
				BOOST_TEST(i6 == 0);
				BOOST_TEST(i7 == 0);
				return 1;
			},
			[ ]
			{
				return 2;
			} );
		BOOST_TEST_EQ(r, 1);
	}

	{
		int r = leaf::handle_all(
			[&]
			{
				return try_block(fail_where::op_b_start_before_op_a_start);
			},
			[ ]( loc<fail_where::op_b_start_before_op_a_start>, info<1> const * i1, info<2> const * i2, info<3> const * i3, info<4> const * i4, info<5> const * i5, info<6> const * i6, info<7> const * i7 )
			{
				BOOST_TEST(i1 == 0);
				BOOST_TEST(i2 == 0);
				BOOST_TEST(i3 == 0);
				BOOST_TEST(i4 != 0);
				BOOST_TEST(i5 == 0);
				BOOST_TEST(i6 == 0);
				BOOST_TEST(i7 == 0);
				return 1;
			},
			[ ]
			{
				return 2;
			} );
		BOOST_TEST_EQ(r, 1);
	}

	{
		int r = leaf::handle_all(
			[&]
			{
				return try_block(fail_where::op_b_start_after_successful_op_a);
			},
			[ ]( loc<fail_where::op_b_start_after_successful_op_a>, info<1> const * i1, info<2> const * i2, info<3> const * i3, info<4> const * i4, info<5> const * i5, info<6> const * i6, info<7> const * i7 )
			{
				BOOST_TEST(i1 == 0);
				BOOST_TEST(i2 == 0);
				BOOST_TEST(i3 != 0);
				BOOST_TEST(i4 == 0);
				BOOST_TEST(i5 != 0);
				BOOST_TEST(i6 == 0);
				BOOST_TEST(i7 == 0);
				return 1;
			},
			[ ]
			{
				return 2;
			} );
		BOOST_TEST_EQ(r, 1);
	}

	{
		int r = leaf::handle_all(
			[&]
			{
				return try_block(fail_where::op_b_cont_before_op_a_start);
			},
			[ ]( loc<fail_where::op_b_cont_before_op_a_start>, info<1> const * i1, info<2> const * i2, info<3> const * i3, info<4> const * i4, info<5> const * i5, info<6> const * i6, info<7> const * i7 )
			{
				BOOST_TEST(i1 == 0);
				BOOST_TEST(i2 == 0);
				BOOST_TEST(i3 != 0);
				BOOST_TEST(i4 == 0);
				BOOST_TEST(i5 != 0);
				BOOST_TEST(i6 != 0);
				BOOST_TEST(i7 == 0);
				return 1;
			},
			[ ]
			{
				return 2;
			} );
		BOOST_TEST_EQ(r, 1);
	}

	{
		int r = leaf::handle_all(
			[&]
			{
				return try_block(fail_where::op_b_cont_after_successful_op_a);
			},
			[ ]( loc<fail_where::op_b_cont_after_successful_op_a>, info<1> const * i1, info<2> const * i2, info<3> const * i3, info<4> const * i4, info<5> const * i5, info<6> const * i6, info<7> const * i7 )
			{
				BOOST_TEST(i1 == 0);
				BOOST_TEST(i2 == 0);
				BOOST_TEST(i3 != 0);
				BOOST_TEST(i4 == 0);
				BOOST_TEST(i5 == 0);
				BOOST_TEST(i6 == 0);
				BOOST_TEST(i7 != 0);
				return 1;
			},
			[ ]
			{
				return 2;
			} );
		BOOST_TEST_EQ(r, 1);
	}

	return boost::report_errors();
}
