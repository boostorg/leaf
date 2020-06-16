// Copyright (c) 2018-2020 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This is the program presented in https://zajo.github.io/leaf/#introduction-eh.

// It reads a text file in a buffer and prints it to std::cout, using LEAF to handle errors.
// This version does not use exception handling. The version that does use exception
// handling is in print_file_eh.cpp.

#include <boost/leaf/result.hpp>
#include <boost/leaf/handle_error.hpp>
#include <boost/leaf/on_error.hpp>
#include <boost/leaf/common.hpp>
#include <iostream>
#include <memory>
#include <stdio.h>

namespace leaf = boost::leaf;


struct bad_command_line { };

// Input errors:
struct input_error { };
struct open_error { };
struct read_error { };
struct size_error { };
struct eof_error { };

// Output errors:
struct output_error { };
struct cout_error { };


// We will handle all failures in our main function, but first, here are the declarations of the functions it calls, each
// communicating failures using leaf::result<T>:

// Parse the command line, return the file name.
leaf::result<char const *> parse_command_line( int argc, char const * argv[] );

// Open a file for reading.
leaf::result<std::shared_ptr<FILE>> file_open( char const * file_name );

// Return the size of the file.
leaf::result<int> file_size( FILE & f );

// Read size bytes from f into buf.
leaf::result<void> file_read( FILE & f, void * buf, int size );


// The main function, which handles all errors.
int main( int argc, char const * argv[] )
{
	return leaf::try_handle_all(

		[&]() -> leaf::result<int>
		{
			BOOST_LEAF_AUTO(file_name, parse_command_line(argc,argv));

			auto load = leaf::on_error( leaf::e_file_name{file_name} );

			BOOST_LEAF_AUTO(f, file_open(file_name));

			BOOST_LEAF_AUTO(s, file_size(*f));

			std::string buffer( 1 + s, '\0' );
			BOOST_LEAF_CHECK(file_read(*f, &buffer[0], buffer.size()-1));

			auto load2 = leaf::on_error( output_error{}, cout_error{}, [] { return leaf::e_errno{errno}; } );
			std::cout << buffer;
			std::cout.flush();
			if( std::cout.fail() )
				return leaf::new_error();

			return 0;
		},

		// Each of the lambdas below is an error handler. LEAF will consider them, in order, and call the first one that matches
		// the available error objects.

		// This handler will be called if the error includes:
		// - an object of type input_error, and
		// - an object of type open_error, and
		// - an object of type leaf::e_errno that has .value equal to ENOENT, and
		// - an object of type leaf::e_file_name.
		[]( input_error, open_error, leaf::match<leaf::e_errno,ENOENT>, leaf::e_file_name const & fn )
		{
			std::cerr << "File not found: " << fn.value << std::endl;
			return 1;
		},

		// This handler will be called if the error includes:
		// - an object of type input_error, and
		// - an object of type open_error, and
		// - an object of type leaf::e_errno (regardless of its .value), and
		// - an object of type leaf::e_file_name.
		[]( input_error, open_error, leaf::e_errno const & errn, leaf::e_file_name const & fn )
		{
			std::cerr << "Failed to open " << fn.value << ", errno=" << errn << std::endl;
			return 2;
		},

		// This handler will be called if the error includes:
		// - an object of type input_error, and
		// - an optional object of type leaf::e_errno (regardless of its .value), and
		// - an object of type leaf::e_file_name.
		[]( input_error, leaf::e_errno const * errn, leaf::e_file_name const & fn )
		{
			std::cerr << "Failed to access " << fn.value;
			if( errn )
				std::cerr << ", errno=" << *errn;
			std::cerr << std::endl;
			return 3;
		},

		// This handler will be called if the error includes:
		// - an object of type output_error, and
		// - an object of type leaf::e_errno (regardless of its .value),
		[]( output_error, leaf::e_errno const & errn )
		{
			std::cerr << "Output error, errno=" << errn << std::endl;
			return 4;
		},

		// This handler will be called if the error includes bad_command_line
		[]( bad_command_line )
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
leaf::result<char const *> parse_command_line( int argc, char const * argv[] )
{
	if( argc==2 )
		return argv[1];
	else
		return leaf::new_error(bad_command_line{});
}


// Open a file for reading.
leaf::result<std::shared_ptr<FILE>> file_open( char const * file_name )
{
	auto load = leaf::on_error( input_error{}, open_error{}, [] { return leaf::e_errno{errno}; } );

	if( FILE * f = fopen(file_name,"rb") )
		return std::shared_ptr<FILE>(f,&fclose);
	else
		return leaf::new_error();
}


// Return the size of the file.
leaf::result<int> file_size( FILE & f )
{
	auto load = leaf::on_error( input_error{}, size_error{}, [] { return leaf::e_errno{errno}; } );

	if( fseek(&f,0,SEEK_END) )
		return leaf::new_error();

	int s = ftell(&f);
	if( s==-1L )
		return leaf::new_error();

	if( fseek(&f,0,SEEK_SET) )
		return leaf::new_error();

	return s;
}


// Read size bytes from f into buf.
leaf::result<void> file_read( FILE & f, void * buf, int size )
{
	auto load = leaf::on_error( input_error{}, read_error{} );

	int n = fread(buf,1,size,&f);

	if( ferror(&f) )
		return leaf::new_error(leaf::e_errno{errno});

	if( n!=size )
		return leaf::new_error();

	return { };
}
