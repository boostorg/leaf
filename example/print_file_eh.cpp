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


//We could define our own error info types, but for this example the ones defined in
//<boost/leaf/common.hpp> are a perfect match.
using leaf::e_file_name;
using leaf::e_errno;


//Exception type hierarchy.
struct print_file_error : virtual std::exception { };
struct command_line_error : virtual print_file_error { };
struct bad_command_line : virtual command_line_error { };
struct input_error : virtual print_file_error { };
struct input_file_error : virtual input_error { };
struct input_file_open_error : virtual input_file_error { };
struct input_file_size_error : virtual input_file_error { };
struct input_file_read_error : virtual input_file_error { };
struct input_eof_error : virtual input_file_error { };


std::shared_ptr<FILE> file_open( char const * file_name )
{
	if( FILE * f = fopen(file_name,"rb") )
		return std::shared_ptr<FILE>(f,&fclose);
	else
		leaf::throw_exception( input_file_open_error(), e_file_name{file_name}, e_errno{errno} );
}


int file_size( FILE & f )
{
	//All exceptions escaping this function will automatically propagate errno.
	auto propagate = leaf::defer([ ] { return e_errno{errno}; } );

	if( fseek(&f,0,SEEK_END) )
		throw input_file_size_error();

	int s = ftell(&f);
	if( s==-1L )
		throw input_file_size_error();

	if( fseek(&f,0,SEEK_SET) )
		throw input_file_size_error();

	return s;
}


void file_read( FILE & f, void * buf, int size )
{
	int n = fread(buf,1,size,&f);

	if( ferror(&f) )
		leaf::throw_exception( input_file_read_error(), e_errno{errno} );

	if( n!=size )
		throw input_eof_error();
}


void print_file( char const * file_name )
{
	std::shared_ptr<FILE> f = file_open( file_name );

	auto propagate1 = leaf::preload( e_file_name{file_name} );

	std::string buffer( 1+file_size(*f), '\0' );
	file_read(*f,&buffer[0],buffer.size()-1);

	auto propagate2 = leaf::defer([ ] { return e_errno{errno}; } );
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
 
	//We expect e_file_name and e_errno objects to arrive with errors handled in this function.
	//They will be stored inside exp.
	leaf::expect<e_file_name, e_errno> exp;

	try
	{
		print_file(parse_command_line(argc,argv));
		return 0;
	}
	catch( bad_command_line const & )
	{
		std::cout << "Bad command line argument" << std::endl;
		return 1;
	}
	catch( input_file_open_error const & ex )
	{
		//handle_exception takes a list of match objects (in this case only one), each given a set of e_ types. It
		//attempts to match each set (in order) to objects of e_ types, associated with ex, available in exp.
		//If no set can be matched, handle_exception rethrows the current exception. When a match is found,
		//handle_exception calls the corresponding lambda, passing the .value of each of the e_ types from
		//the matched set.
		handle_exception( exp, ex,

			leaf::match<e_file_name, e_errno>( [ ] ( std::string const & fn, int errn )
			{
				if( errn==ENOENT )
					std::cerr << "File not found: " << fn << std::endl;
				else
					std::cerr << "Failed to open " << fn << ", errno=" << errn << std::endl;
			} )

		);
		return 2;
	}
	catch( input_error const & ex )
	{
		//In this case handle_exception is given 3 match sets. It will first check if both e_file_name and e_errno,
		//associated with ex, are avialable in exp; if not, it will next check if just e_errno is available; and if
		//not, the last (empty) set will always match to print a generic error message.
		handle_exception( exp, ex,

			leaf::match<e_file_name, e_errno>( [ ] ( std::string const & fn, int errn )
			{
				std::cerr << "Input error, " << fn << ", errno=" << errn << std::endl;
			} ),

			leaf::match<e_errno>( [ ] ( int errn )
			{
				std::cerr << "Input error, errno=" << errn << std::endl;
			} ),

			leaf::match<>( [ ]
			{
				std::cerr << "Input error" << std::endl;
			} )

		);
		return 3;
	}
	catch( std::ostream::failure const & ex )
	{
		//Report failure to write to std::cout, print the relevant errno, if available.
		handle_exception( exp, ex,

			leaf::match<e_errno>( [ ] ( int errn )
			{
				std::cerr << "Output error, errno=" << errn << std::endl;
			} )

		);
		return 4;
	}
	catch(...)
	{
		//This catch-all is designed to help diagnose logic errors (main should be able to deal with any failures).
		std::cerr << "Unknown error, cryptic information follows." << std::endl;
		current_exception_diagnostic_print(std::cerr,exp);
		return 5;
	}
}
