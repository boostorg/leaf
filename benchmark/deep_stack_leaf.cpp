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
#include <chrono>
#include <iostream>
#include <iomanip>

namespace leaf = boost::leaf;

// Disable inlining for correct benchmarking.
#ifdef _MSC_VER
#	define NOINLINE __declspec(noinline)
#else
#	define NOINLINE __attribute__((noinline))
#endif

#ifdef LEAF_NO_EXCEPTIONS
// leaf::result<T>::value() calls throw_exception to throw if it has no value.
// When exceptions are disabled, throw_exception is left undefined, and the user
// is required to define it (it may not return).
namespace boost
{
	[[noreturn]] void throw_exception( std::exception const & e )
	{
		std::cerr << "Terminating due to a C++ exception under LEAF_NO_EXCEPTIONS: " << e.what();
		std::terminate();
	}
}
#endif

//////////////////////////////////////

// A simple error code type.
enum class e_error_code
{
	ec1=1,
	ec2
};

namespace boost { namespace leaf {

	template <> struct is_e_type<e_error_code>: std::true_type { };

} }

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

//////////////////////////////////////

template <int N, class T, class... E>
struct benchmark_check_error_noinline
{
	NOINLINE static leaf::result<T> f() noexcept
	{
		LEAF_AUTO(x, (benchmark_check_error_noinline<N-1, T, E...>::f()));
		return x+1;
	}
};

template <class T, class... E>
struct benchmark_check_error_noinline<0, T, E...>
{
	NOINLINE static leaf::result<T> f() noexcept
	{
		return leaf::new_error(E{}...);
	}
};

//////////////////////////////////////

template <int N, class T, class... E>
struct benchmark_check_error_inline
{
	NOINLINE static leaf::result<T> f() noexcept
	{
		LEAF_AUTO(x, (benchmark_check_error_inline<N-1, T, E...>::f()));
		return x+1;
	}
};

template <class T, class... E>
struct benchmark_check_error_inline<0, T, E...>
{
	NOINLINE static leaf::result<T> f() noexcept
	{
		return leaf::new_error(E{}...);
	}
};

//////////////////////////////////////

template <int N, class T, class... E>
struct benchmark_handle_some_noinline
{
	NOINLINE static leaf::result<T> f() noexcept
	{
		return leaf::try_handle_some(
			[]() -> leaf::result<T>
			{
				LEAF_AUTO(x, (benchmark_handle_some_noinline<N-1, T, E...>::f()));
				return x+1;
			},
			[]( leaf::match<e_error_code, e_error_code::ec2> )
			{
				return 2;
			} );
	}
};

template <class T, class... E>
struct benchmark_handle_some_noinline<0, T, E...>
{
	NOINLINE static leaf::result<T> f() noexcept
	{
		return leaf::new_error(E{}...);
	}
};

//////////////////////////////////////

template <int N, class T, class... E>
struct benchmark_handle_some_inline
{
	static leaf::result<T> f() noexcept
	{
		return leaf::try_handle_some(
			[]() -> leaf::result<T>
			{
				LEAF_AUTO(x, (benchmark_handle_some_inline<N-1, T, E...>::f()));
				return x+1;
			},
			[]( leaf::match<e_error_code, e_error_code::ec2> )
			{
				return 2;
			} );
	}
};

template <class T, class... E>
struct benchmark_handle_some_inline<0, T, E...>
{
	static leaf::result<T> f() noexcept
	{
		return leaf::new_error(E{}...);
	}
};

//////////////////////////////////////

template <class Benchmark>
int runner() noexcept
{
	return leaf::try_handle_all(
		[]
		{
			return Benchmark::f();
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
	std::cout << "\ne_error_code    | LEAF_AUTO          | Disabled | ";
	x += print_elapsed_time<iteration_count>( [ ] { return runner<benchmark_check_error_noinline<depth, int, e_error_code>>(); } );
	std::cout << "\ne_error_code    | LEAF_AUTO          | Enabled  | ";
	x += print_elapsed_time<iteration_count>( [ ] { return runner<benchmark_check_error_inline<depth, int, e_error_code>>(); } );
	std::cout << "\ne_error_code    | try_handle_some    | Disabled | ";
	x += print_elapsed_time<iteration_count>( [ ] { return runner<benchmark_handle_some_noinline<depth, int, e_error_code>>(); } );
	std::cout << "\ne_error_code    | try_handle_some    | Enabled  | ";
	x += print_elapsed_time<iteration_count>( [ ] { return runner<benchmark_handle_some_inline<depth, int, e_error_code>>(); } );
	std::cout << "\ne_heavy_payload | LEAF_AUTO          | Disabled | ";
	x += print_elapsed_time<iteration_count>( [ ] { return runner<benchmark_check_error_noinline<depth, int, e_heavy_payload>>(); } );
	std::cout << "\ne_heavy_payload | LEAF_AUTO          | Enabled  | ";
	x += print_elapsed_time<iteration_count>( [ ] { return runner<benchmark_check_error_inline<depth, int, e_heavy_payload>>(); } );
	std::cout << "\ne_heavy_payload | try_handle_some    | Disabled | ";
	x += print_elapsed_time<iteration_count>( [ ] { return runner<benchmark_handle_some_noinline<depth, int, e_heavy_payload>>(); } );
	std::cout << "\ne_heavy_payload | try_handle_some    | Enabled  | ";
	x += print_elapsed_time<iteration_count>( [ ] { return runner<benchmark_handle_some_inline<depth, int, e_heavy_payload>>(); } );

	std::cout << std::endl;
	return x;
}
