//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//This example demonstrates the basic use of LEAF to augment error conditions with
//additional information when using result<T> to report failures. See print_file_eh.cpp
//for the variant that uses exception handling.

#include <boost/leaf/all.hpp>
#include <iostream>
#include <memory>
#include <stdio.h>

namespace leaf = boost::leaf;

//We could define our own error info types, but for this example the ones
//defined in <boost/leaf/common.hpp> are a perfect match.
using leaf::ei_file_name;
using leaf::ei_errno;

//Errors
enum print_file_error_enum
{
	file_open_error,
	file_size_error,
	file_read_error,
	file_eof_error,
	cout_error
};

struct ei_error_code { print_file_error_enum value; };

leaf::result<std::shared_ptr<FILE>> file_open( char const * file_name )
{
	if( FILE * f = fopen(file_name,"rb") )
		return std::shared_ptr<FILE>(f,&fclose);
	else
		return leaf::error( ei_error_code{file_open_error}, ei_file_name{file_name}, ei_errno{errno} );
}

leaf::result<int> file_size( FILE & f )
{
	auto put = leaf::preload(&leaf::get_errno);

	if( fseek(&f,0,SEEK_END) )
		return leaf::error( ei_error_code{file_size_error} );

	int s = ftell(&f);
	if( s==-1L )
		return leaf::error( ei_error_code{file_size_error} );

	if( fseek(&f,0,SEEK_SET) )
		return leaf::error( ei_error_code{file_size_error} );

	return s;
}

leaf::result<void> file_read( FILE & f, void * buf, int size )
{
	int n = fread(buf,1,size,&f);
	if( ferror(&f) )
		return leaf::error( ei_error_code{file_read_error}, ei_errno{errno} );

	if( n!=size )
		return leaf::error( ei_error_code{file_eof_error} );

	return { };
}

leaf::result<void> print_file( char const * file_name )
{
	LEAF_CHECK(f,file_open(file_name));

	auto put = leaf::preload( ei_file_name{file_name} );

	LEAF_CHECK(s,file_size(*f));
	std::string buffer( 1+s, '\0' );

	LEAF_CHECK_(file_read(*f,&buffer[0],buffer.size()-1));
	std::cout << buffer;

	std::cout.flush();
	if( std::cout.fail() )
		return leaf::error( ei_error_code{cout_error} );
	else
		return { };
}

char const * parse_command_line( int argc, char const * argv[ ] )
{
	if( argc!=2 )
		return 0;
	else
		return argv[1];
}

int main( int argc, char const * argv[ ] )
{
	char const * fn = parse_command_line(argc,argv);
	if( !fn )
	{
		std::cout << "Bad command line argument" << std::endl;
		return 1;
	}

	//We expect ei_error_code, ei_file_name and ei_errno error info to arrive with exceptions handled in this function.
	leaf::expect<ei_error_code,ei_file_name,ei_errno> exp;
	if( auto r = print_file(fn) )
		return 0;
	else
		switch( auto ec=*leaf::peek<ei_error_code>(exp,r) )
		{
		case file_open_error:
			//handle_error is given a list of match objects (in this case only one), which it attempts to match (in order) to
			//available error info (if none can be matched, it throws leaf::mismatch_error, which in this program would
			//be a logic error, since it's not supposed to throw exceptions).
			handle_error( exp, r, leaf::match<ei_file_name,ei_errno>( [ ] ( std::string const & fn, int errn )
				{
					if( errn==ENOENT )
						std::cerr << "File not found: " << fn << std::endl;
					else
						std::cerr << "Failed to open " << fn << ", errno=" << errn << std::endl;
				} ) );
			return 2;
		case file_size_error:
		case file_read_error:
		case file_eof_error:
			//handle_error is given a list of match objects, which it attempts to match (in order) to available error info.
			//In this case it will first check if both ei_file_name and ei_errno are avialable; if not, it will next check
			//if just ei_errno is available; and if not, the last match will match even if no error info is available,
			//to print a generic error message.
			handle_error( exp, r,
				leaf::match<ei_file_name,ei_errno>( [ ] ( std::string const & fn, int errn )
				{
					std::cerr << "Failed to access " << fn << ", errno=" << errn << std::endl;
				} ),
				leaf::match<ei_errno>( [ ] ( int errn )
				{
					std::cerr << "I/O error, errno=" << errn << std::endl;
				} ),
				leaf::match<>( [ ]
				{
					std::cerr << "I/O error" << std::endl;
				} ) );
			return 3;
		default:
			//This catch-all is designed to help diagnose logic errors (missing case labels in the switch statement).
			std::cerr << "Unknown error code " << ec << ", cryptic information follows." << std::endl;
			diagnostic_print(std::cerr,exp,r);
			return 4;
		}
}
