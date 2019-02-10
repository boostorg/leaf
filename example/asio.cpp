// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This program demonstrates the use of leaf::accumulate to capture the path an error takes
// as is travels through ASIO continuation chain. The path is only captured if:
// - An error occurrs, and
// - A handler that takes e_error_trace argument is present.
// Otherwse none of the error trace machinery will be invoked by LEAF.

#include <boost/leaf/preload.hpp>
#include <boost/leaf/handle_error.hpp>
#include <boost/leaf/result.hpp>
#include <iostream>
#include <functional>
#include <deque>
#include <cstdlib>

#define ENABLE_ERROR_TRACE 1

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

struct e_error_trace
{
	struct rec
	{
		char const * file;
		int line;
		friend std::ostream & operator<<( std::ostream & os, rec const & x )
		{
			return os << x.file << '(' << x.line << ')' << std::endl;
		}
	};

	std::deque<rec> value;

	friend std::ostream & operator<<( std::ostream & os, e_error_trace const & tr )
	{
		for( auto & i : tr.value )
			os << i;
		return os;
	}
};

#define ERROR_TRACE auto _trace = leaf::accumulate( [ ]( e_error_trace & tr ) { tr.value.emplace_front(e_error_trace::rec{__FILE__, __LINE__}); } )

int const failure_percent = 10;

leaf::result<int> simulate_failure_point( int value ) noexcept
{
	ERROR_TRACE;
	if( (std::rand()%100) > failure_percent )
		return value;
	else
		return LEAF_NEW_ERROR();
}

////////////////////////////////////

struct op_a
{
	template <class CompletionHandler>
	static void start( asio::io_context & ioc, CompletionHandler && ch ) noexcept
	{
		asio::post( ioc,
			[=]() mutable
			{
				ERROR_TRACE;
				ch(simulate_failure_point(21));
			} );
	}
};

////////////////////////////////////

// ob_b: op_a => cont_1 => op_a => call handler

struct op_b
{
	template <class CompletionHandler>
	static void start( asio::io_context & ioc, CompletionHandler && ch ) noexcept
	{
		op_a::start( ioc,
			[=, &ioc]( leaf::result<int> && a1 ) mutable
			{
				ERROR_TRACE;
				if( a1 )
					cont1(ioc, *a1, std::move(ch));
				else
					ch(std::forward<leaf::result<int>>(a1));
			} );
	}

	template <class CompletionHandler>
	static void cont1( asio::io_context & ioc, int a1, CompletionHandler && ch ) noexcept
	{
		op_a::start( ioc,
			[=]( leaf::result<int> && a2 ) mutable
			{
				ERROR_TRACE;
				if( a2 )
					ch(a1 + *a2);
				else
					ch(std::forward<leaf::result<int>>(a2));
			} );
	}
};

////////////////////////////////////

// op_c: op_b => cont_1 => op_b => cont_2 => op_b => call handler

struct op_c
{
	template <class CompletionHandler>
	static void start( asio::io_context & ioc, CompletionHandler && ch ) noexcept
	{
		op_b::start( ioc,
			[=, &ioc]( leaf::result<int> && b1 ) mutable
			{
				ERROR_TRACE;
				if( b1 )
					cont1(ioc, *b1, std::move(ch));
				else
					ch(std::forward<leaf::result<int>>(b1));
			} );
	}

	template <class CompletionHandler>
	static void cont1( asio::io_context & ioc, int b1, CompletionHandler && ch ) noexcept
	{
		op_b::start( ioc,
			[=, &ioc]( leaf::result<int> && b2 ) mutable
			{
				ERROR_TRACE;
				if( b2 )
					cont2(ioc, b1, *b2, std::move(ch));
				else
					ch(std::forward<leaf::result<int>>(b2));
			} );
	}

	template <class CompletionHandler>
	static void cont2( asio::io_context & ioc, int b1, int b2, CompletionHandler && ch ) noexcept
	{
		op_b::start( ioc,
			[=]( leaf::result<int> && b3 ) mutable
			{
				ERROR_TRACE;
				if( b3 )
					ch(b1 + b2 - *b3);
				else
					ch(std::forward<leaf::result<int>>(b3));
			} );
	}
};

////////////////////////////////////

int main()
{
	for( int i=0; i!=10; ++i )
		leaf::try_handle_all(
			[]() -> leaf::result<int>
			{
				asio::io_context ioc;
				leaf::result<int> res;
				op_c::start( ioc,
					[&]( leaf::result<int> && answer ) mutable
					{
						res = make_continuation_result(std::move(answer));
					} );
				ioc.run();
				LEAF_AUTO(r, res);
				std::cout << "Success! Answer=" << r << std::endl;
				return 0;
			},
#if ENABLE_ERROR_TRACE
			[ ]( e_error_trace const & tr )
			{
				std::cerr << "Error! Trace:" << std::endl << tr;
				return 1;
			},
#endif
			[ ]
			{
				std::cerr << "Error!" << std::endl;
				return 2;
			} );
	return 0;
}
