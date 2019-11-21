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
#include <fstream>
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

std::fstream append_csv()
{
	if( FILE * f = fopen("benchmark.csv","rb") )
	{
		fclose(f);
		return std::fstream("benchmark.csv", std::fstream::out | std::fstream::app);
	}
	else
	{
		std::fstream fs("benchmark.csv", std::fstream::out | std::fstream::app);
		fs << ",\"Check, noinline\",\"Check, inline\",\"Handle some, noinline\", \"Handle some, inline\",\"Error rate\"\n";
		return fs;
	}
}

template <class F>
int print_elapsed_time( int iteration_count, F && f )
{
	auto start = std::chrono::system_clock::now();
	int val = 0;
	for( int i = 0; i!=iteration_count; ++i )
		val += std::forward<F>(f)();
	auto stop = std::chrono::system_clock::now();
	int elapsed = std::chrono::duration_cast<std::chrono::microseconds>(stop-start).count();
	std::cout << std::right << std::setw(8) << elapsed;
	append_csv() << ',' << elapsed;
	return val;
}

//////////////////////////////////////

template <int Depth, class E>
int benchmark_type( char const * type_name, int iteration_count )
{
	int const test_rates[ ] = { 10, 50, 90 };
	int x=0;
	std::cout << "----------------|--------------------|----------|-------|--------";
	for( auto fr : test_rates )
	{
		append_csv() << "Outcome";
		std::cout << '\n' << std::left << std::setw(16) << type_name << "| OUTCOME_TRY        | Disabled | " << std::right << std::setw(4) << fr << "% |";
		std::srand(0);
		x += print_elapsed_time( iteration_count, [=] { return runner<benchmark_check_error_noinline<Depth, int, E>>(fr); } );
		std::cout << '\n' << std::left << std::setw(16) << type_name << "| OUTCOME_TRY        | Enabled  | " << std::right << std::setw(4) << fr << "% |";
		std::srand(0);
		x += print_elapsed_time( iteration_count, [=] { return runner<benchmark_check_error_inline<Depth, int, E>>(fr); } );
		std::cout << '\n' << std::left << std::setw(16) << type_name << "| Handle some errors | Disabled | " << std::right << std::setw(4) << fr << "% |";
		std::srand(0);
		x += print_elapsed_time( iteration_count, [=] { return runner<benchmark_handle_some_noinline<Depth, int, E>>(fr); } );
		std::cout << '\n' << std::left << std::setw(16) << type_name << "| Handle some errors | Enabled  | " << std::right << std::setw(4) << fr << "% |";
		std::srand(0);
		x += print_elapsed_time( iteration_count, [=] { return runner<benchmark_handle_some_inline<Depth, int, E>>(fr); } );
		append_csv() << ',' << fr << '\n';
	};
	std::cout << '\n';
	return x;
}

//////////////////////////////////////

int main()
{
	int const depth = 20;
	int const iteration_count = 1000;
	std::cout <<
		iteration_count << " iterations, call depth " << depth << ", sizeof(e_heavy_payload) = " << sizeof(e_heavy_payload) << "\n"
		"Outcome\n"
		"                |                    | Function | Error | Elapsed\n"
		"Error type      | At each level      | inlining | rate  |    (μs)\n";
	return
		benchmark_type<depth, e_error_code>("e_error_code", iteration_count) +
		benchmark_type<depth, e_heavy_payload>("e_heavy_payload", iteration_count);
}
