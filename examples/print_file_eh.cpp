// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This is the program presented in https://zajo.github.io/leaf/#introduction-eh.

// It reads a text file in a buffer and prints it to std::cout, using LEAF to handle errors.
// This version uses exception handling. The version that does not use exception
// handling is in print_file_result.cpp.

#include <boost/leaf/exception.hpp>
#include <boost/leaf/handle_exception.hpp>
#include <boost/leaf/on_error.hpp>
#include <boost/leaf/common.hpp>
#include <iostream>
#include <memory>
#include <stdio.h>

namespace leaf = boost::leaf;


// Exception type hierarchy.
struct print_file_error : virtual std::exception { };
struct command_line_error : virtual print_file_error { };
struct bad_command_line : virtual command_line_error { };
struct input_error : virtual print_file_error { };
struct input_file_error : virtual input_error { };
struct input_file_open_error : virtual input_file_error { };
struct input_file_size_error : virtual input_file_error { };
struct input_file_read_error : virtual input_file_error { };
struct input_eof_error : virtual input_file_error { };


// We will handle all failures in our main function, but first, here are the declarations of the functions it calls, each
// communicating failures by throwing exceptions

// Parse the command line, return the file name.
char const * parse_command_line( int argc, char const * argv[] );

// Open a file for reading.
std::shared_ptr<FILE> file_open( char const * file_name );

// Return the size of the file.
int file_size( FILE & f );

// Read size bytes from f into buf.
void file_read( FILE & f, void * buf, int size );


int main( int argc, char const * argv[] )
{
	// Configure std::cout to throw on error.
	std::cout.exceptions(std::ostream::failbit | std::ostream::badbit);

	return leaf::try_catch(
		[&]
		{
			char const * file_name = parse_command_line(argc,argv);

			auto load = leaf::on_error( leaf::e_file_name{file_name} );

			std::shared_ptr<FILE> f = file_open( file_name );

			int s = file_size(*f);

			std::string buffer( 1 + s, '\0' );
			file_read(*f,&buffer[0],buffer.size()-1);

			auto load2 = leaf::on_error([] { return leaf::e_errno{errno}; } );
			std::cout << buffer;
			std::cout.flush();

			return 0;
		},

		// Each of the lambdas below is an error handler. LEAF will consider them, in order, and call the first one that matches
		// the available error objects.

		// This handler will be called if the error includes:
		// - a caught exception of type input_file_open_error, and
		// - an object of type leaf::e_errno that has .value equal to ENOENT, and
		// - an object of type leaf::e_file_name.
		[]( leaf::catch_<input_file_open_error>, leaf::match<leaf::e_errno,ENOENT>, leaf::e_file_name const & fn )
		{
			std::cerr << "File not found: " << fn.value << std::endl;
			return 1;
		},

		// This handler will be called if the error includes:
		// - a caught exception of type input_file_open_error, and
		// - an object of type leaf::e_errno (regardless of its .value), and
		// - an object of type leaf::e_file_name.
		[]( leaf::catch_<input_file_open_error>, leaf::e_errno const & errn, leaf::e_file_name const & fn )
		{
			std::cerr << "Failed to open " << fn.value << ", errno=" << errn << std::endl;
			return 2;
		},

		// This handler will be called if the error includes:
		// - a caught exception of type input_error, and
		// - an object of type leaf::e_errno (regardless of its .value), and
		// - an object of type leaf::e_file_name.
		[]( leaf::catch_<input_error>, leaf::e_errno const & errn, leaf::e_file_name const & fn )
		{
			std::cerr << "Failed to access " << fn.value << ", errno=" << errn << std::endl;
			return 3;
		},

		// This handler will be called if the error includes:
		// - a caught exception of type std::ostream::failure, and
		// - an object of type leaf::e_errno (regardless of its .value),
		[]( leaf::catch_<std::ostream::failure>, leaf::e_errno const & errn )
		{
			std::cerr << "Output error, errno=" << errn << std::endl;
			return 4;
		},

		// This handler will be called if the error includes a caught exception of type bad_command_line.
		[]( leaf::catch_<bad_command_line> )
		{
			std::cout << "Bad command line argument" << std::endl;
			return 5;
		},

		// This last handler matches any error: it prints diagnostic information to help debug logic errors in the program, since it
		// failed to match  an appropriate error handler to the error condition it encountered. In this program this handler will
		// never be called.
		[]( leaf::error_info const & unmatched )
		{
			std::cerr <<
				"Unknown failure detected" << std::endl <<
				"Cryptic diagnostic information follows" << std::endl <<
				unmatched;
			return 6;
		} );
}


// Implementations of the functions called by main:


// Parse the command line, return the file name.
char const * parse_command_line( int argc, char const * argv[] )
{
	if( argc==2 )
		return argv[1];
	else
		throw leaf::exception(bad_command_line());
}


// Open a file for reading.
std::shared_ptr<FILE> file_open( char const * file_name )
{
	if( FILE * f = fopen(file_name,"rb") )
		return std::shared_ptr<FILE>(f,&fclose);
	else
		throw leaf::exception(input_file_open_error(), leaf::e_errno{errno});
}


// Return the size of the file.
int file_size( FILE & f )
{
	// All exceptions escaping this function will automatically load errno.
	auto load = leaf::on_error( [] { return leaf::e_errno{errno}; } );

	if( fseek(&f,0,SEEK_END) )
		throw leaf::exception(input_file_size_error());

	int s = ftell(&f);
	if( s==-1L )
		throw leaf::exception(input_file_size_error());

	if( fseek(&f,0,SEEK_SET) )
		throw leaf::exception(input_file_size_error());

	return s;
}


// Read size bytes from f into buf.
void file_read( FILE & f, void * buf, int size )
{
	int n = fread(buf,1,size,&f);

	if( ferror(&f) )
		throw leaf::exception( input_file_read_error(), leaf::e_errno{errno} );

	if( n!=size )
		throw leaf::exception(input_eof_error());
}
