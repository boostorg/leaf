// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This is a short but complete program that reads a text file in a buffer and prints it to std::cout,
// using LEAF to handle errors. It does not use exception handling.

#include <boost/leaf/result.hpp>
#include <boost/leaf/handle_all.hpp>
#include <boost/leaf/preload.hpp>
#include <boost/leaf/common.hpp>
#include <iostream>
#include <memory>
#include <stdio.h>

namespace leaf = boost::leaf;

// First, we need an enum to define our error codes:
enum error_code
{
	bad_command_line = 1,
	input_file_open_error,
	input_file_size_error,
	input_file_read_error,
	input_eof_error,
	cout_error
};

// To enable LEAF to work with our error_code enum, we need to specialize the is_e_type template:
namespace boost { namespace leaf {
	template<> struct is_e_type<error_code>: std::true_type { };
} }


// We will handle all failures in our main function, but first, here are the declarations of the functions it calls, each
// communicating failures using leaf::result<T>:

// Parse the command line, return the file name.
leaf::result<char const *> parse_command_line( int argc, char const * argv[ ] );

// Open a file for reading.
leaf::result<std::shared_ptr<FILE>> file_open( char const * file_name );

// Return the size of the file.
leaf::result<int> file_size( FILE & f );

// Read size bytes from f into buf.
leaf::result<void> file_read( FILE & f, void * buf, int size );


// The main function, which handles all errors.
int main( int argc, char const * argv[ ] )
{
	return leaf::handle_all(

		[&]() -> leaf::result<int>
		{
			LEAF_AUTO(file_name, parse_command_line(argc,argv));

			auto load = leaf::preload( leaf::e_file_name{file_name} );

			LEAF_AUTO(f, file_open(file_name));

			LEAF_AUTO(s, file_size(*f));

			std::string buffer( 1 + s, '\0' );
			LEAF_CHECK(file_read(*f, &buffer[0], buffer.size()-1));

			std::cout << buffer;
			std::cout.flush();
			if( std::cout.fail() )
				return leaf::new_error( cout_error, leaf::e_errno{errno} );

			return 0;
		},

		// Each of the lambdas below is an error handler. LEAF will consider them, in order, and call the first one that matches
		// the available error information.

		// This handler will be called if the error includes:
		// - an object of type error_code equal to input_file_open_error, and
		// - an object of type leaf::e_errno that has .value equal to ENOENT, and
		// - an object of type leaf::e_file_name.
		[ ]( leaf::match<error_code, input_file_open_error>, leaf::match<leaf::e_errno, ENOENT>, leaf::e_file_name const & fn )
		{
			std::cerr << "File not found: " << fn.value << std::endl;
			return 1;
		},

		// This handler will be called if the error includes:
		// - an object of type error_code equal to input_file_open_error, and
		// - an object of type leaf::e_errno (regardless of its .value), and
		// - an object of type leaf::e_file_name.
		[ ]( leaf::match<error_code, input_file_open_error>, leaf::e_errno const & errn, leaf::e_file_name const & fn )
		{
			std::cerr << "Failed to open " << fn.value << ", errno=" << errn << std::endl;
			return 2;
		},

		// This handler will be called if the error includes:
		// - an object of type error_code equal to any of input_file_size_error, input_file_read_error, input_eof_error, and
		// - an object of type leaf::e_errno (regardless of its .value), and
		// - an object of type leaf::e_file_name.
		[ ]( leaf::match<error_code, input_file_size_error, input_file_read_error, input_eof_error>, leaf::e_errno const & errn, leaf::e_file_name const & fn )
		{
			std::cerr << "Failed to access " << fn.value << ", errno=" << errn << std::endl;
			return 3;
		},

		// This handler will be called if the error includes:
		// - an object of type error_code equal to cout_error, and
		// - an object of type leaf::e_errno (regardless of its .value),
		[ ]( leaf::match<error_code, cout_error>, leaf::e_errno const & errn )
		{
			std::cerr << "Output error, errno=" << errn << std::endl;
			return 4;
		},

		// This handler will be called if the error includes an object of type error_code equal to bad_command_line.
		[ ]( leaf::match<error_code, bad_command_line> )
		{
			std::cout << "Bad command line argument" << std::endl;
			return 5;
		},

		// This last handler matches any error: it prints diagnostic information to help debug logic errors in the program, since it
		// failed to match  an appropriate error handler to the error condition it encountered. In this program this handler will
		// never be called.
		[ ]( leaf::error_info const & unmatched )
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
leaf::result<char const *> parse_command_line( int argc, char const * argv[ ] )
{
	if( argc==2 )
		return argv[1];
	else
		return leaf::new_error(bad_command_line);
}


// Open a file for reading.
leaf::result<std::shared_ptr<FILE>> file_open( char const * file_name )
{
	if( FILE * f = fopen(file_name,"rb") )
		return std::shared_ptr<FILE>(f,&fclose);
	else
		return leaf::new_error(input_file_open_error, leaf::e_errno{errno});
}


// Return the size of the file.
leaf::result<int> file_size( FILE & f )
{
	// All exceptions escaping this function will automatically load errno.
	auto load = leaf::defer([ ] { return leaf::e_errno{errno}; });

	if( fseek(&f,0,SEEK_END) )
		return leaf::new_error(input_file_size_error);

	int s = ftell(&f);
	if( s==-1L )
		return leaf::new_error(input_file_size_error);

	if( fseek(&f,0,SEEK_SET) )
		return leaf::new_error(input_file_size_error);

	return s;
}


// Read size bytes from f into buf.
leaf::result<void> file_read( FILE & f, void * buf, int size )
{
	int n = fread(buf,1,size,&f);

	if( ferror(&f) )
		return leaf::new_error(input_file_read_error, leaf::e_errno{errno});

	if( n!=size )
		return leaf::new_error(input_eof_error);

	return { };
}
