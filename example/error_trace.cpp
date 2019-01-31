// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This is a simple program that demonstrates the use of LEAF to transport e-objects between threads,
// without using exception handling. See capture_eh.cpp for the exception-handling variant.

#include <boost/leaf/preload.hpp>
#include <boost/leaf/handle_all.hpp>
#include <boost/leaf/result.hpp>
#include <iostream>
#include <deque>
#include <cstdlib>

#define ENABLE_ERROR_TRACE 1

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

#define ERROR_TRACE auto _trace = leaf::accumulate( [ ]( e_error_trace & tr ) { tr.value.push_back(e_error_trace::rec{__FILE__, __LINE__}); } )

int const failure_percen = 25;

leaf::result<void> f1()
{
	ERROR_TRACE;
	if( (std::rand()%100) > failure_percen )
		return { };
	else
		return leaf::new_error();
}

leaf::result<void> f2()
{
	ERROR_TRACE;
	if( (std::rand()%100) > failure_percen )
		return f1();
	else
		return leaf::new_error();
}

leaf::result<void> f3()
{
	ERROR_TRACE;
	if( (std::rand()%100) > failure_percen )
		return f2();
	else
		return leaf::new_error();
}

leaf::result<void> f4()
{
	ERROR_TRACE;
	if( (std::rand()%100) > failure_percen )
		return f3();
	else
		return leaf::new_error();
}

leaf::result<void> f5()
{
	ERROR_TRACE;
	if( (std::rand()%100) > failure_percen )
		return f4();
	else
		return leaf::new_error();
}

int main()
{
	for( int i=0; i!=10; ++i )
		leaf::handle_all(
			[&]() -> leaf::result<void>
			{
				std::cout << "Run # " << i << ": ";
				LEAF_CHECK(f5());
				std::cout << "Success!" << std::endl;
				return { };
			},
#if ENABLE_ERROR_TRACE
			[ ]( e_error_trace const & tr )
			{
				std::cerr << "Error! Trace:" << std::endl << tr;
			},
#endif
			[ ]
			{
				std::cerr << "Error!" << std::endl;
			} );
	return 0;
}
