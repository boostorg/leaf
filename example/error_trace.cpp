// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This program demonstrates the use of leaf::accumulate to capture the path an error takes
// as is bubbles up the call stack. The path is only captured if:
// - An error occurrs, and
// - A handler that takes e_error_trace argument is present.
// Otherwse none of the error trace machinery will be invoked by LEAF.

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

#define ERROR_TRACE auto _trace = leaf::accumulate( [ ]( e_error_trace & tr ) { tr.value.emplace_front(e_error_trace::rec{__FILE__, __LINE__}); } )

int const failure_percent = 25;

leaf::result<void> f1()
{
	ERROR_TRACE;
	if( (std::rand()%100) > failure_percent )
		return { };
	else
		return leaf::new_error();
}

leaf::result<void> f2()
{
	ERROR_TRACE;
	if( (std::rand()%100) > failure_percent )
		return f1();
	else
		return leaf::new_error();
}

leaf::result<void> f3()
{
	ERROR_TRACE;
	if( (std::rand()%100) > failure_percent )
		return f2();
	else
		return leaf::new_error();
}

leaf::result<void> f4()
{
	ERROR_TRACE;
	if( (std::rand()%100) > failure_percent )
		return f3();
	else
		return leaf::new_error();
}

leaf::result<void> f5()
{
	ERROR_TRACE;
	if( (std::rand()%100) > failure_percent )
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
