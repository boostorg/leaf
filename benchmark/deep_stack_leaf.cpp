// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This benchmark forwards a leaf::result<int> through very many stack frames.
//
// It runs the following scenarios:
//
// - In case of error, communicating e_error_code, a simple error enum.
// - In case of error, communicating e_heavy_payload, a large error type.
//
// Each of the above variants is benchmarked using 2 different scenarios:
//
// - Each level computes a value, forwards all errors to the caller.
// - Each level computes a value, handles some errors, forwards other errors.
//
// Benchmarking is run with inlining enabled as well as disabled.

//Disable diagnostic features.
#define LEAF_NO_DIAGNOSTIC_INFO
#define LEAF_DISCARD_UNEXPECTED

#if GODBOLT
#	include "https://raw.githubusercontent.com/zajo/leaf/master/include/boost/leaf/all.hpp"
#else
#	include <boost/leaf/all.hpp>
#endif
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include <iomanip>

#ifndef LEAF_NO_EXCEPTIONS
#	error Please disable exception handling.
#endif

namespace boost
{
	void throw_exception( std::exception const & e )
	{
		std::cerr << "Terminating due to a C++ exception under LEAF_NO_EXCEPTIONS: " << e.what();
		std::terminate();
	}
}

//////////////////////////////////////

namespace leaf = boost::leaf;

#ifdef _MSC_VER
#	define NOINLINE __declspec(noinline)
#else
#	define NOINLINE __attribute__((noinline))
#endif

//////////////////////////////////////

enum class e_error_code
{
	ec1=1,
	ec2
};

namespace boost { namespace leaf {

	template <> struct is_e_type<e_error_code>: std::true_type { };

} }

// Note: in LEAF, handling of error objects is O(1) no matter how many stack frames.
struct e_heavy_payload
{
	char value[4096];

	e_heavy_payload() noexcept
	{
		std::memset(value, std::rand(), sizeof(value));
	}
};

template <class E>
E make_error() noexcept;

template <>
inline e_error_code make_error() noexcept
{
	return (std::rand()%2) ? e_error_code::ec1 : e_error_code::ec2;
}

template <>
inline e_heavy_payload make_error() noexcept
{
	return e_heavy_payload();
}

inline bool should_fail( int failure_rate ) noexcept
{
	assert(failure_rate>=0);
	assert(failure_rate<=100);
	return (std::rand()%100) < failure_rate;
}

//////////////////////////////////////

template <int N, class T, class E>
struct benchmark_check_error_noinline
{
	NOINLINE static leaf::result<T> f( int failure_rate ) noexcept
	{
		LEAF_AUTO(x, (benchmark_check_error_noinline<N-1, T, E>::f(failure_rate)));
		return x+1;
	}
};

template <class T, class E>
struct benchmark_check_error_noinline<0, T, E>
{
	NOINLINE static leaf::result<T> f( int failure_rate ) noexcept
	{
		if( should_fail(failure_rate) )
			return leaf::new_error(make_error<E>());
		else
			return T{ };
	}
};

//////////////////////////////////////

template <int N, class T, class E>
struct benchmark_check_error_inline
{
	static leaf::result<T> f( int failure_rate ) noexcept
	{
		LEAF_AUTO(x, (benchmark_check_error_inline<N-1, T, E>::f(failure_rate)));
		return x+1;
	}
};

template <class T, class E>
struct benchmark_check_error_inline<0, T, E>
{
	static leaf::result<T> f( int failure_rate ) noexcept
	{
		if( should_fail(failure_rate) )
			return leaf::new_error(make_error<E>());
		else
			return T{ };
	}
};

//////////////////////////////////////

template <int N, class T, class E>
struct benchmark_handle_some_noinline
{
	NOINLINE static leaf::result<T> f( int failure_rate ) noexcept
	{
		return leaf::try_handle_some(
			[=]() -> leaf::result<T>
			{
				LEAF_AUTO(x, (benchmark_handle_some_noinline<N-1, T, E>::f(failure_rate)));
				return x+1;
			},
			[]( leaf::match<e_error_code, e_error_code::ec2> )
			{
				return 2;
			} );
	}
};

template <class T, class E>
struct benchmark_handle_some_noinline<0, T, E>
{
	NOINLINE static leaf::result<T> f( int failure_rate ) noexcept
	{
		if( should_fail(failure_rate) )
			return leaf::new_error(make_error<E>());
		else
			return T{ };
	}
};

//////////////////////////////////////

template <int N, class T, class E>
struct benchmark_handle_some_inline
{
	static leaf::result<T> f( int failure_rate ) noexcept
	{
		return leaf::try_handle_some(
			[=]() -> leaf::result<T>
			{
				LEAF_AUTO(x, (benchmark_handle_some_inline<N-1, T, E>::f(failure_rate)));
				return x+1;
			},
			[]( leaf::match<e_error_code, e_error_code::ec2> )
			{
				return 2;
			} );
	}
};

template <class T, class E>
struct benchmark_handle_some_inline<0, T, E>
{
	static leaf::result<T> f( int failure_rate ) noexcept
	{
		if( should_fail(failure_rate) )
			return leaf::new_error(make_error<E>());
		else
			return T{ };
	}
};

//////////////////////////////////////

template <class Benchmark>
int runner( int failure_rate ) noexcept
{
	return leaf::try_handle_all(
		[=]
		{
			return Benchmark::f(failure_rate);
		},
		[]( e_error_code const & )
		{
			return -1;
		},
		[]( e_heavy_payload const & )
		{
			return -2;
		},
		[]
		{
			return -3;
		} );
}

//////////////////////////////////////

template <int Iterations, class F>
int print_elapsed_time( F && f )
{
	auto start = std::chrono::system_clock::now();
	int val = 0;
	for( int i = 0; i!=Iterations; ++i )
		val += std::forward<F>(f)();
	auto stop = std::chrono::system_clock::now();
	std::cout << std::setw(10) << std::chrono::duration_cast<std::chrono::microseconds>(stop-start).count();
	return val;
}

//////////////////////////////////////

int main()
{
	int const depth = 200;
	int const iteration_count = 5000;
	int const test_rates[ ] = { 5, 50, 95 };
	int x=0;

	std::cout << iteration_count << " iterations, call depth " << depth << ", sizeof(e_heavy_payload) = " << sizeof(e_heavy_payload);
	for( auto fr : test_rates )
	{
		std::cout << "\n"
		"\nError type      | At each level      | Inlining | Elapsed μs"
		"\n----------------|--------------------|----------|----------- failure rate = " << fr << '%';
		std::cout << "\ne_error_code    | LEAF_AUTO          | Disabled | ";
		x += print_elapsed_time<iteration_count>( [=] { return runner<benchmark_check_error_noinline<depth, int, e_error_code>>(fr); } );
		std::cout << "\ne_error_code    | LEAF_AUTO          | Enabled  | ";
		x += print_elapsed_time<iteration_count>( [=] { return runner<benchmark_check_error_inline<depth, int, e_error_code>>(fr); } );
		std::cout << "\ne_error_code    | try_handle_some    | Disabled | ";
		x += print_elapsed_time<iteration_count>( [=] { return runner<benchmark_handle_some_noinline<depth, int, e_error_code>>(fr); } );
		std::cout << "\ne_error_code    | try_handle_some    | Enabled  | ";
		x += print_elapsed_time<iteration_count>( [=] { return runner<benchmark_handle_some_inline<depth, int, e_error_code>>(fr); } );
		std::cout << "\ne_heavy_payload | LEAF_AUTO          | Disabled | ";
		x += print_elapsed_time<iteration_count>( [=] { return runner<benchmark_check_error_noinline<depth, int, e_heavy_payload>>(fr); } );
		std::cout << "\ne_heavy_payload | LEAF_AUTO          | Enabled  | ";
		x += print_elapsed_time<iteration_count>( [=] { return runner<benchmark_check_error_inline<depth, int, e_heavy_payload>>(fr); } );
		std::cout << "\ne_heavy_payload | try_handle_some    | Disabled | ";
		x += print_elapsed_time<iteration_count>( [=] { return runner<benchmark_handle_some_noinline<depth, int, e_heavy_payload>>(fr); } );
		std::cout << "\ne_heavy_payload | try_handle_some    | Enabled  | ";
		x += print_elapsed_time<iteration_count>( [=] { return runner<benchmark_handle_some_inline<depth, int, e_heavy_payload>>(fr); } );
	};

	std::cout << std::endl;
	return x;
}
