//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//This example demonstrates the basic use of LEAF to augment error conditions with
//additional information when using exceptions to report failures. See print_file_result.cpp
//for the variant that doesn't use exceptions.

#include <boost/leaf/all.hpp>
#include <iostream>
#include <memory>
#include <stdio.h>

namespace leaf = boost::leaf;

//We could define our own error info types, but for this example the ones
//defined in <boost/leaf/common.hpp> are a perfect match.
using leaf::ei_file_name;
using leaf::ei_errno;

//Exception type hierarchy.
struct print_file_error : virtual std::exception { };
struct command_line_error : virtual print_file_error { };
struct bad_command_line : virtual command_line_error { };
struct io_error : virtual print_file_error { };
struct file_error : virtual io_error { };
struct file_open_error : virtual file_error { };
struct file_size_error : virtual file_error { };
struct file_read_error : virtual file_error { };
struct file_eof_error : virtual file_error { };

std::shared_ptr<FILE> file_open( char const * file_name )
{
	if( FILE * f = fopen(file_name,"rb") )
		return std::shared_ptr<FILE>(f,&fclose);
	else
		leaf::throw_exception( file_open_error(), ei_file_name{file_name}, ei_errno{errno} );
}

int file_size( FILE & f )
{
	auto put = leaf::preload(&leaf::get_errno);

	if( fseek(&f,0,SEEK_END) )
		throw file_size_error();

	int s = ftell(&f);
	if( s==-1L )
		throw file_size_error();

	if( fseek(&f,0,SEEK_SET) )
		throw file_size_error();

	return s;
}

void file_read( FILE & f, void * buf, int size )
{
	int n = fread(buf,1,size,&f);

	if( ferror(&f) )
		leaf::throw_exception( file_read_error(), ei_errno{errno} );

	if( n!=size )
		throw file_eof_error();
}

void print_file( char const * file_name )
{
	auto put = leaf::preload( ei_file_name{file_name} );

	std::shared_ptr<FILE> f = file_open( file_name );
	std::string buffer( 1+file_size(*f), '\0' );
	file_read(*f,&buffer[0],buffer.size()-1);

	std::cout << buffer;
	std::cout.flush();
}

char const * parse_command_line( int argc, char const * argv[ ] )
{
	if( argc!=2 )
		throw bad_command_line();
	return argv[1];
}

int main( int argc, char const * argv[ ] )
{
 	std::cout.exceptions ( std::ostream::failbit | std::ostream::badbit );
 
 	//We expect ei_file_name and ei_errno info to arrive with exceptions handled in this function.
	leaf::expect<ei_file_name,ei_errno> exp;

	try
	{
		print_file(parse_command_line(argc,argv));
	}
	catch( bad_command_line const & )
	{
		std::cout << "Bad command line argument" << std::endl;
		return 1;
	}
	catch( file_open_error const & e )
	{
		//handle_error is given a list of match objects (in this case only one), which it attempts to match (in order) to
		//available exception info (if none can be matched, it throws leaf::mismatch_error).
		handle_error( exp, e, leaf::match<ei_file_name,ei_errno>( [ ] ( std::string const & fn, int errn )
			{
				if( errn==ENOENT )
					std::cerr << "File not found: " << fn << std::endl;
				else
					std::cerr << "Failed to open " << fn << ", errno=" << errn << std::endl;
			} ) );
		return 2;
	}
	catch( io_error const & e )
	{
		//handle_error is given a list of match objects, which it attempts to match (in order) to available exception info.
		//In this case it will first check if both ei_file_name and ei_errno are avialable; if not, it will next check
		//if just ei_errno is available; and if not, the last match will match even if no exception info is available,
		//to print a generic error message.
		handle_error( exp, e,
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
	}
	catch(...)
	{
		//This catch-all is designed to help diagnose logic errors (main should be able to deal with any failures).
		std::cerr << "Unknown error, cryptic information follows." << std::endl;
		current_exception_diagnostic_print(std::cerr,exp);
		return 4;
	}
	return 0;
	}
