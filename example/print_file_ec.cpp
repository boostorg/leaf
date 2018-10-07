//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//This example demonstrates the basic use of LEAF to augment error conditions with
//additional information when using error codes to report failures. See print_file_eh.cpp
//for the variant that uses exception handling.

#include <boost/leaf/all.hpp>
#include <iostream>
#include <stdio.h>

namespace leaf = boost::leaf;

//We could define our own exception info types, but for this example the ones
//defined in <boost/leaf/common.hpp> are a perfect match.
using leaf::ei_file_name;
using leaf::ei_errno;

//Errors
enum
error
	{
	ok,
	bad_command_line,
	file_open_error,
	file_size_error,
	file_read_error,
	file_eof_error,
	cout_error
	};

error
file_open( char const * file_name, std::shared_ptr<FILE> & result )
	{
	if( FILE * f = fopen(file_name,"rb") )
		{
		result.reset(f,&fclose);
		return ok;
		}
	else
		{
		leaf::put(ei_file_name{file_name},ei_errno{errno});
		return file_open_error;
		}
	}

error
file_size( FILE & f, int & result )
	{
	auto put = leaf::preload(&leaf::get_errno);
	if( fseek(&f,0,SEEK_END) )
		return file_size_error;
	int s = ftell(&f);
	if( s==-1L )
		return file_size_error;
	if( fseek(&f,0,SEEK_SET) )
		return file_size_error;
	result = s;
	return ok;
	}

error
file_read( FILE & f, void * buf, int size )
	{
	int n = fread(buf,1,size,&f);
	if( ferror(&f) )
		{
		leaf::put(ei_errno{errno});
		return file_read_error;
		}
	if( n!=size )
		return file_eof_error;
	return ok;
	}

error
print_file( char const * file_name )
	{
	std::shared_ptr<FILE> f;
	if( error err = file_open(file_name,f) )
		return err;

	auto put = leaf::preload( ei_file_name{file_name} );

	int s;
	if( error err = file_size(*f,s) )
		return err;
	std::string buffer( 1+s, '\0' );
	if( error err = file_read(*f,&buffer[0],buffer.size()-1) )
		return err;
	std::cout << buffer;
	std::cout.flush();
	if( std::cout.fail() )
		return cout_error;

	put.cancel();
	return ok;
	}

error
parse_command_line( int argc, char const * argv[ ], char const * & result )
	{
	if( argc!=2 )
		return bad_command_line;
	result = argv[1];
	return ok;
	}

bool
return_true() noexcept
	{
	return true;
	}

int
main( int argc, char const * argv[ ] )
	{
	//Instruct leaf::preload to always put() its payload during stack unwinding, not only if there
	//is an uncaught_exception(). If possible, we should supply a function that evaluates to true
	//if there is a pending error in this thread, false otherwise. Our function always returns true.
	leaf::set_has_current_error(&return_true);

	char const * fn;
	if( error err=parse_command_line(argc,argv,fn) )
		{
		std::cout << "Bad command line argument" << std::endl;
		return 1;
		}

	//We expect ei_file_name and ei_errno info to arrive with exceptions handled in this function.
	leaf::expect<ei_file_name,ei_errno> info;
	switch( error err=print_file(fn) )
		{
		case ok:
			return 0;
		case file_open_error:
			//unwrap is given a list of match objects (in this case only one), which it attempts to match (in order) to
			//available exception info (if none can be matched, it throws leaf::mismatch_error).
			unwrap( info.match<ei_file_name,ei_errno>( [ ] ( std::string const & fn, int errn )
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
			//unwrap is given a list of match objects, which it attempts to match (in order) to available exception info.
			//In this case it will first check if both ei_file_name and ei_errno are avialable; if not, it will next check
			//if just ei_errno is available; and if not, the last match will match even if no exception info is available,
			//to print a generic error message.
			unwrap(
				info.match<ei_file_name,ei_errno>( [ ] ( std::string const & fn, int errn )
					{
					std::cerr << "Failed to access " << fn << ", errno=" << errn << std::endl;
					} ),
				info.match<ei_errno>( [ ] ( int errn )
					{
					std::cerr << "I/O error, errno=" << errn << std::endl;
					} ),
				info.match<>( [ ]
					{
					std::cerr << "I/O error" << std::endl;
					} ) );
			return 3;
		default:
			std::cerr <<
				"Unknown error code " << err << ", cryptic information follows." << std::endl <<
				leaf::diagnostic_information;
			return 4;
		}
	}
