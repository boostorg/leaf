// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This program demonstrates the use of leaf::accumulate to print the path an error takes
// as is bubbles up the call stack. The path is only printed if:
// - An error occurrs, and
// - A handler that takes e_error_log argument is present.
// Otherwse none of the error log machinery will be invoked by LEAF.

#include <boost/leaf/preload.hpp>
#include <boost/leaf/handle_error.hpp>
#include <boost/leaf/result.hpp>
#include <iostream>
#include <cstdlib>

#define ENABLE_ERROR_LOG 1

namespace leaf = boost::leaf;

struct e_error_log
{
	struct rec
	{
		char const * file;
		int line;
		friend std::ostream & operator<<( std::ostream & os, rec const & x )
		{
			return os << x.file << '(' << x.line << ')';
		}
	};

	e_error_log()
	{
		std::cerr << "Error! Log:" << std::endl;
	}

	template <class T>
	friend std::ostream & operator<<( e_error_log const &, T const & x )
	{
		return std::cerr << x << std::endl;
	}
};

namespace boost { namespace leaf {
	template <> struct is_e_type<e_error_log>: std::true_type { };
} }

#define ERROR_LOG auto _log = leaf::accumulate( [ ]( e_error_log & log ) { log << e_error_log::rec{__FILE__, __LINE__}; } )

int const failure_percent = 25;

leaf::result<void> f1()
{
	ERROR_LOG;
	if( (std::rand()%100) > failure_percent )
		return { };
	else
		return leaf::new_error();
}

leaf::result<void> f2()
{
	ERROR_LOG;
	if( (std::rand()%100) > failure_percent )
		return f1();
	else
		return leaf::new_error();
}

leaf::result<void> f3()
{
	ERROR_LOG;
	if( (std::rand()%100) > failure_percent )
		return f2();
	else
		return leaf::new_error();
}

leaf::result<void> f4()
{
	ERROR_LOG;
	if( (std::rand()%100) > failure_percent )
		return f3();
	else
		return leaf::new_error();
}

leaf::result<void> f5()
{
	ERROR_LOG;
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
#if ENABLE_ERROR_LOG
			[ ]( e_error_log const & )
			{
			},
#endif
			[ ]
			{
				std::cerr << "Error!" << std::endl;
			} );
	return 0;
}
