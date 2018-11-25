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


//We could define our own e-types, but for this example the ones defined in
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
		throw leaf::exception<input_file_open_error>( e_file_name{file_name}, e_errno{errno} );
}


int file_size( FILE & f )
{
	//All exceptions escaping this function will automatically propagate errno.
	auto propagate = leaf::defer( [ ] { return e_errno{errno}; } );

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
		throw leaf::exception<input_file_read_error>( e_errno{errno} );

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
	//Configure std::cout to throw on error.
	std::cout.exceptions ( std::ostream::failbit | std::ostream::badbit );

	//We expect e_file_name and e_errno objects to be associated with errors
	//handled in this function. They will be stored inside of exp.
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
	catch( input_file_open_error const & e )
	{
		//handle_exception takes a list of functions (in this case only one). It attempts to
		//match each function (in order) to objects currently available in exp, which
		//are associated with the error value stored in e. If no function can be matched,
		//handle_exception returns false. Otherwise the matched function is invoked with
		//the corresponding available error objects.
		handle_exception( exp, e,

			[ ] ( e_file_name const & fn, e_errno const & errn )
			{
				if( errn.value==ENOENT )
					std::cerr << "File not found: " << fn.value << std::endl;
				else
					std::cerr << "Failed to open " << fn.value << ", errno=" << errn << std::endl;
			}

		);
		return 2;
	}
	catch( input_error const & e )
	{
		//In this case handle_exception is given 3 functions. It will first check if both
		//e_file_name and e_errno, associated with e, are avialable in exp; if not, it will
		//next check if just e_errno is available; and if not, the last function (which
		//takes no arguments) will always match to print a generic error message.
		handle_exception( exp, e,

			[ ] ( e_file_name const & fn, e_errno const & errn )
			{
				std::cerr << "Input error, " << fn.value << ", errno=" << errn << std::endl;
			},

			[ ] ( e_errno const & errn )
			{
				std::cerr << "Input error, errno=" << errn << std::endl;
			},

			[ ]
			{
				std::cerr << "Input error" << std::endl;
			}

		);
		return 3;
	}
	catch( std::ostream::failure const & e )
	{
		//Report failure to write to std::cout, print the relevant errno.
		handle_exception( exp, e,

			[ ] ( e_errno const & errn )
			{
				std::cerr << "Output error, errno=" << errn << std::endl;
			}

		);
		return 4;
	}
	catch(...)
	{
		//This catch-all case helps diagnose logic errors (presumably, missing catch).
		std::cerr << "Unknown error, cryptic information follows." << std::endl;
		current_exception_diagnostic_output(std::cerr,exp);
		return 5;
	}
}
