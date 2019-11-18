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

#include <boost/outcome/std_outcome.hpp>
#include <boost/outcome/try.hpp>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <iostream>
#include <iomanip>

#ifndef BOOST_NO_EXCEPTIONS
#	error Please disable exception handling.
#endif

namespace boost
{
	void throw_exception( std::exception const & e )
	{
		std::cerr << "Terminating due to a C++ exception under BOOST_NO_EXCEPTIONS: " << e.what();
		std::terminate();
	}
}

//////////////////////////////////////

namespace outcome = boost::outcome_v2;

template <class T, class EC>
using result = outcome::std_outcome<T, EC>;

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

bool check_handle_some_value( e_error_code e ) noexcept
{
	return e==e_error_code::ec2;
}

bool check_handle_some_value( e_heavy_payload const & ) noexcept
{
	return false;
}

//////////////////////////////////////

template <int N, class T, class E>
struct benchmark_check_error_noinline
{
	NOINLINE static result<T, E> f( int failure_rate ) noexcept
	{
		BOOST_OUTCOME_TRY(x, (benchmark_check_error_noinline<N-1, T, E>::f(failure_rate)));
		return x+1;
	}
};

template <class T, class E>
struct benchmark_check_error_noinline<0, T, E>
{
	static result<T, E> f( int failure_rate ) noexcept
	{
		if( should_fail(failure_rate) )
			return make_error<E>();
		else
			return T{ };
	}
};

//////////////////////////////////////

template <int N, class T, class E>
struct benchmark_check_error_inline
{
	static result<T, E> f( int failure_rate ) noexcept
	{
		BOOST_OUTCOME_TRY(x, (benchmark_check_error_inline<N-1, T, E>::f(failure_rate)));
		return x+1;
	}
};

template <class T, class E>
struct benchmark_check_error_inline<0, T, E>
{
	static result<T, E> f( int failure_rate ) noexcept
	{
		if( should_fail(failure_rate) )
			return make_error<E>();
		else
			return T{ };
	}
};

//////////////////////////////////////

template <int N, class T, class E>
struct benchmark_handle_some_noinline
{
	NOINLINE static result<T, E> f( int failure_rate ) noexcept
	{
		if( auto r = benchmark_handle_some_noinline<N-1, T, E>::f(failure_rate) )
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
	static result<T, E> f( int failure_rate ) noexcept
	{
		if( should_fail(failure_rate) )
			return make_error<E>();
		else
			return T{ };
	}
};

//////////////////////////////////////

template <int N, class T, class E>
struct benchmark_handle_some_inline
{
	static result<T, E> f( int failure_rate ) noexcept
	{
		if( auto r = benchmark_handle_some_inline<N-1, T, E>::f(failure_rate) )
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
	static result<T, E> f( int failure_rate ) noexcept
	{
		if( should_fail(failure_rate) )
			return make_error<E>();
		else
			return T{ };
	}
};

//////////////////////////////////////

template <class Benchmark>
int runner( int failure_rate ) noexcept
{
	if( auto r = Benchmark::f(failure_rate) )
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
		std::cout << "\ne_error_code    | OUTCOME_TRY        | Disabled | ";
		x += print_elapsed_time<iteration_count>( [=] { return runner<benchmark_check_error_noinline<depth, int, e_error_code>>(fr); } );
		std::cout << "\ne_error_code    | OUTCOME_TRY        | Enabled  | ";
		x += print_elapsed_time<iteration_count>( [=] { return runner<benchmark_check_error_inline<depth, int, e_error_code>>(fr); } );
		std::cout << "\ne_error_code    | Handle some errors | Disabled | ";
		x += print_elapsed_time<iteration_count>( [=] { return runner<benchmark_handle_some_noinline<depth, int, e_error_code>>(fr); } );
		std::cout << "\ne_error_code    | Handle some errors | Enabled  | ";
		x += print_elapsed_time<iteration_count>( [=] { return runner<benchmark_handle_some_inline<depth, int, e_error_code>>(fr); } );
		std::cout << "\ne_heavy_payload | OUTCOME_TRY        | Disabled | ";
		x += print_elapsed_time<iteration_count>( [=] { return runner<benchmark_check_error_noinline<depth, int, e_heavy_payload>>(fr); } );
		std::cout << "\ne_heavy_payload | OUTCOME_TRY        | Enabled  | ";
		x += print_elapsed_time<iteration_count>( [=] { return runner<benchmark_check_error_inline<depth, int, e_heavy_payload>>(fr); } );
		std::cout << "\ne_heavy_payload | Handle some errors | Disabled | ";
		x += print_elapsed_time<iteration_count>( [=] { return runner<benchmark_handle_some_noinline<depth, int, e_heavy_payload>>(fr); } );
		std::cout << "\ne_heavy_payload | Handle some errors | Enabled  | ";
		x += print_elapsed_time<iteration_count>( [=] { return runner<benchmark_handle_some_inline<depth, int, e_heavy_payload>>(fr); } );
	};

	std::cout << std::endl;
	return x;
}
