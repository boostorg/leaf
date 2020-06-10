// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This is the program presented in https://zajo.github.io/leaf/#introduction-result,
// converted to use outcome::result instead of leaf::result.

// It reads a text file in a buffer and prints it to std::cout, using LEAF to handle errors.
// This version does not use exception handling.

#include <boost/outcome/result.hpp>
#include <boost/leaf/handle_error.hpp>
#include <boost/leaf/on_error.hpp>
#include <boost/leaf/common.hpp>
#include <iostream>
#include <memory>
#include <stdio.h>

namespace leaf = boost::leaf;
namespace outcome = boost::outcome_v2;

template <class T>
using result = outcome::std_result<T>;

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

// To enable LEAF to work with outcome::result, we need to specialize the is_result_type template:
namespace boost { namespace leaf {
	template <class T> struct is_result_type<result<T>>: std::true_type { };
} }


// We will handle all failures in our main function, but first, here are the declarations of the functions it calls, each
// communicating failures using result<T>:

// Parse the command line, return the file name.
result<char const *> parse_command_line( int argc, char const * argv[] );

// Open a file for reading.
result<std::shared_ptr<FILE>> file_open( char const * file_name );

// Return the size of the file.
result<int> file_size( FILE & f );

// Read size bytes from f into buf.
result<void> file_read( FILE & f, void * buf, int size );


// The main function, which handles all errors.
int main( int argc, char const * argv[] )
{
	return leaf::try_handle_all(

		[&]() -> result<int>
		{
			LEAF_AUTO(file_name, parse_command_line(argc,argv));

			auto load = leaf::on_error( leaf::e_file_name{file_name} );

			LEAF_AUTO(f, file_open(file_name));

			LEAF_AUTO(s, file_size(*f));

			std::string buffer( 1 + s, '\0' );
			LEAF_CHECK(file_read(*f, &buffer[0], buffer.size()-1));

			std::cout << buffer;
			std::cout.flush();
			if( std::cout.fail() )
				return leaf::new_error( cout_error, leaf::e_errno{errno} ).to_error_code();

			return 0;
		},

		// Each of the lambdas below is an error handler. LEAF will consider them, in order, and call the first one that matches
		// the available error objects.

		// This handler will be called if the error includes:
		// - an object of type error_code equal to input_file_open_error, and
		// - an object of type leaf::e_errno that has .value equal to ENOENT, and
		// - an object of type leaf::e_file_name.
		[]( leaf::match<error_code, input_file_open_error>, leaf::match<leaf::e_errno, ENOENT>, leaf::e_file_name const & fn )
		{
			std::cerr << "File not found: " << fn.value << std::endl;
			return 1;
		},

		// This handler will be called if the error includes:
		// - an object of type error_code equal to input_file_open_error, and
		// - an object of type leaf::e_errno (regardless of its .value), and
		// - an object of type leaf::e_file_name.
		[]( leaf::match<error_code, input_file_open_error>, leaf::e_errno const & errn, leaf::e_file_name const & fn )
		{
			std::cerr << "Failed to open " << fn.value << ", errno=" << errn << std::endl;
			return 2;
		},

		// This handler will be called if the error includes:
		// - an object of type error_code equal to any of input_file_size_error, input_file_read_error, input_eof_error, and
		// - an optional object of type leaf::e_errno (regardless of its .value), and
		// - an object of type leaf::e_file_name.
		[]( leaf::match<error_code, input_file_size_error, input_file_read_error, input_eof_error>, leaf::e_errno const * errn, leaf::e_file_name const & fn )
		{
			std::cerr << "Failed to access " << fn.value;
			if( errn )
				std::cerr << ", errno=" << *errn;
			std::cerr << std::endl;
			return 3;
		},

		// This handler will be called if the error includes:
		// - an object of type error_code equal to cout_error, and
		// - an object of type leaf::e_errno (regardless of its .value),
		[]( leaf::match<error_code, cout_error>, leaf::e_errno const & errn )
		{
			std::cerr << "Output error, errno=" << errn << std::endl;
			return 4;
		},

		// This handler will be called if the error includes an object of type error_code equal to bad_command_line.
		[]( leaf::match<error_code, bad_command_line> )
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
result<char const *> parse_command_line( int argc, char const * argv[] )
{
	if( argc==2 )
		return argv[1];
	else
		return leaf::new_error(bad_command_line).to_error_code();
}


// Open a file for reading.
result<std::shared_ptr<FILE>> file_open( char const * file_name )
{
	if( FILE * f = fopen(file_name,"rb") )
		return std::shared_ptr<FILE>(f,&fclose);
	else
		return leaf::new_error(input_file_open_error, leaf::e_errno{errno}).to_error_code();
}


// Return the size of the file.
result<int> file_size( FILE & f )
{
	// All exceptions escaping this function will automatically load errno.
	auto load = leaf::on_error([] { return leaf::e_errno{errno}; });

	if( fseek(&f,0,SEEK_END) )
		return leaf::new_error(input_file_size_error).to_error_code();

	int s = ftell(&f);
	if( s==-1L )
		return leaf::new_error(input_file_size_error).to_error_code();

	if( fseek(&f,0,SEEK_SET) )
		return leaf::new_error(input_file_size_error).to_error_code();

	return s;
}


// Read size bytes from f into buf.
result<void> file_read( FILE & f, void * buf, int size )
{
	int n = fread(buf,1,size,&f);

	if( ferror(&f) )
		return leaf::new_error(input_file_read_error, leaf::e_errno{errno}).to_error_code();

	if( n!=size )
		return leaf::new_error(input_eof_error).to_error_code();

	return outcome::success();
}
