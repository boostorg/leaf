// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This benchmark forwards an outcome<T, E> through very many stack frames.
//
// It runs the following outcome<T, E> variants:
//
// - outcome<int, e_error_code>, where e_error_code is a simple error enum.
// - outcome<int, e_heavy_payload>, where e_heavy_payload is a large error type.
//
// Each of the above variants is benchmarked using 2 different scenarios:
//
// - Each level computes a value, forwards all errors to the caller.
// - Each level computes a value, handles some errors, forwards other errors.
//
// Benchmarking is run with inlining enabled as well as disabled.

#include <boost/outcome/outcome.hpp>
#include <boost/outcome/try.hpp>
#include <cstring>
#include <chrono>
#include <iostream>
#include <iomanip>

namespace outcome = boost::outcome_v2;

template <class T, class EC>
using result = outcome::outcome<T, EC>;

// Disable inlining for correct benchmarking.
#ifdef _MSC_VER
#	define NOINLINE __declspec(noinline)
#else
#	define NOINLINE __attribute__((noinline))
#endif

//////////////////////////////////////

// A simple error code type.
enum class e_error_code
{
	ec1=1,
	ec2
};

// This error type has a large size and takes at least a second to init or move.
// Note: in LEAF, handling of error objects is O(1) no matter how many stack frames.
struct e_heavy_payload
{
	char value[4096];

	e_heavy_payload() noexcept
	{
		std::memset(value, 0, sizeof(value));
	}
};

bool check_handle_some_value( e_error_code e )
{
	return e==e_error_code::ec2;
}

bool check_handle_some_value( e_heavy_payload const & )
{
	return false;
}

//////////////////////////////////////

template <int N, class T, class E>
struct benchmark_check_error_noinline
{
	NOINLINE static result<T, E> f() noexcept
	{
		BOOST_OUTCOME_TRY(x, (benchmark_check_error_noinline<N-1, T, E>::f()));
		return x+1;
	}
};

template <class T, class E>
struct benchmark_check_error_noinline<0, T, E>
{
	NOINLINE static result<T, E> f() noexcept
	{
		return E{};
	}
};

//////////////////////////////////////

template <int N, class T, class E>
struct benchmark_check_error_inline
{
	NOINLINE static result<T, E> f() noexcept
	{
		BOOST_OUTCOME_TRY(x, (benchmark_check_error_noinline<N-1, T, E>::f()));
		return x+1;
	}
};

template <class T, class E>
struct benchmark_check_error_inline<0, T, E>
{
	NOINLINE static result<T, E> f() noexcept
	{
		return E{};
	}
};

//////////////////////////////////////

template <int N, class T, class E>
struct benchmark_handle_some_noinline
{
	NOINLINE static result<T, E> f() noexcept
	{
		if( auto r = benchmark_handle_some_noinline<N-1, T, E>::f() )
			return r.value()+1;
		else if( check_handle_some_value(r.error()) )
			return 1;
		else
			return r.as_failure();
	}
};

template <class T, class E>
struct benchmark_handle_some_noinline<0, T, E>
{
	NOINLINE static result<T, E> f() noexcept
	{
		return E{};
	}
};

//////////////////////////////////////

template <int N, class T, class E>
struct benchmark_handle_some_inline
{
	static result<T, E> f() noexcept
	{
		if( auto r = benchmark_handle_some_noinline<N-1, T, E>::f() )
			return r.value()+1;
		else if( check_handle_some_value(r.error()) )
			return 1;
		else
			return r.as_failure();
	}
};

template <class T, class E>
struct benchmark_handle_some_inline<0, T, E>
{
	static result<T, E> f() noexcept
	{
		return E{};
	}
};

//////////////////////////////////////

template <class Benchmark>
int runner() noexcept
{
	if( auto r = Benchmark::f() )
		return r.value();
	else
		return -1;
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
	std::cout << std::setw(10) << std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count();
	return val;
}

//////////////////////////////////////

int main()
{
	int const depth = 200;
	int const iteration_count = 50000;
	int x=0;

	std::cout <<
	iteration_count << " iterations, call depth " << depth << ", sizeof(e_heavy_payload) = " << sizeof(e_heavy_payload) << ":"
	"\n"
	"\nError type      | At each level      | Inlining | Elapsed ms"
	"\n----------------|--------------------|----------|-----------";
	std::cout << "\ne_error_code    | OUTCOME_TRY        | Disabled | ";
	x += print_elapsed_time<iteration_count>( [ ] { return runner<benchmark_check_error_noinline<depth, int, e_error_code>>(); } );
	std::cout << "\ne_error_code    | OUTCOME_TRY        | Enabled  | ";
	x += print_elapsed_time<iteration_count>( [ ] { return runner<benchmark_check_error_inline<depth, int, e_error_code>>(); } );
	std::cout << "\ne_error_code    | Handle some errors | Disabled | ";
	x += print_elapsed_time<iteration_count>( [ ] { return runner<benchmark_handle_some_noinline<depth, int, e_error_code>>(); } );
	std::cout << "\ne_error_code    | Handle some errors | Enabled  | ";
	x += print_elapsed_time<iteration_count>( [ ] { return runner<benchmark_handle_some_inline<depth, int, e_error_code>>(); } );
	std::cout << "\ne_heavy_payload | OUTCOME_TRY        | Disabled | ";
	x += print_elapsed_time<iteration_count>( [ ] { return runner<benchmark_check_error_noinline<depth, int, e_heavy_payload>>(); } );
	std::cout << "\ne_heavy_payload | OUTCOME_TRY        | Enabled  | ";
	x += print_elapsed_time<iteration_count>( [ ] { return runner<benchmark_check_error_inline<depth, int, e_heavy_payload>>(); } );
	std::cout << "\ne_heavy_payload | Handle some errors | Disabled | ";
	x += print_elapsed_time<iteration_count>( [ ] { return runner<benchmark_handle_some_noinline<depth, int, e_heavy_payload>>(); } );
	std::cout << "\ne_heavy_payload | Handle some errors | Enabled  | ";
	x += print_elapsed_time<iteration_count>( [ ] { return runner<benchmark_handle_some_inline<depth, int, e_heavy_payload>>(); } );

	std::cout << std::endl;
	return x;
}
