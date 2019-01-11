//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//This example demonstrates the basic use of LEAF to augment error conditions with
//additional information when using exceptions to report failures. See print_file_result.cpp
//for the variant that doesn't use exceptions.

#include <boost/leaf/throw.hpp>
#include <boost/leaf/try.hpp>
#include <boost/leaf/preload.hpp>
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
		throw leaf::exception( input_file_open_error(), e_file_name{file_name}, e_errno{errno} );
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
		throw leaf::exception( input_file_read_error(), e_errno{errno} );

	if( n!=size )
		throw input_eof_error();
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

	return leaf::try_(
		[&]
		{
			char const * file_name = parse_command_line(argc,argv);
			std::shared_ptr<FILE> f = file_open( file_name );

			auto propagate1 = leaf::preload( e_file_name{file_name} );

			std::string buffer( 1+file_size(*f), '\0' );
			file_read(*f,&buffer[0],buffer.size()-1);

			auto propagate2 = leaf::defer([ ] { return e_errno{errno}; } );
			std::cout << buffer;
			std::cout.flush();

			return 0;
		},

		[ ]( leaf::catch_<bad_command_line> )
		{
			std::cout << "Bad command line argument" << std::endl;
			return 1;
		},

		[ ]( leaf::catch_<input_file_open_error>, leaf::match<e_errno,ENOENT>, e_file_name const & fn )
		{
			std::cerr << "File not found: " << fn.value << std::endl;
			return 2;
		},

		[ ]( leaf::catch_<input_file_open_error>, e_errno const & errn, e_file_name const & fn )
		{
			std::cerr << "Failed to open " << fn.value << ", errno=" << errn << std::endl;
			return 3;
		},

		[ ]( leaf::catch_<input_error>, e_errno const & errn, e_file_name const & fn )
		{
			std::cerr << "Failed to access " << fn.value << ", errno=" << errn << std::endl;
			return 4;
		},

		[ ]( leaf::catch_<std::ostream::failure>, e_errno const & errn )
		{
			std::cerr << "Output error, errno=" << errn << std::endl;
			return 5;
		},

		[ ]( leaf::error_info const & ei )
		{
			std::cerr <<
				"Unknown failure detected" << std::endl <<
				"Cryptic diagnostic information follows" << std::endl <<
				ei;
			return 6;
		} );
}
