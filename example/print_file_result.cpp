//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//This example demonstrates the basic use of LEAF to augment error conditions with
//additional information when using result<T> to report failures. See print_file_eh.cpp
//for the variant that uses exception handling.

#include <boost/leaf/handle.hpp>
#include <boost/leaf/preload.hpp>
#include <iostream>
#include <memory>
#include <stdio.h>

namespace leaf = boost::leaf;


//We could define our own e-types, but for this example the ones
//defined in <boost/leaf/common.hpp> are a perfect match.
using leaf::e_file_name;
using leaf::e_errno;


enum error_codes
{
	bad_command_line = 1,
	input_file_open_error,
	input_file_size_error,
	input_file_read_error,
	input_eof_error,
	cout_error
};
namespace boost { namespace leaf {
	template<> struct is_error_type<error_codes>: std::true_type { };
} }


leaf::result<std::shared_ptr<FILE>> file_open( char const * file_name )
{
	if( FILE * f = fopen(file_name,"rb") )
		return std::shared_ptr<FILE>(f,&fclose);
	else
		return leaf::new_error( input_file_open_error, e_file_name{file_name}, e_errno{errno} );
}


leaf::result<int> file_size( FILE & f )
{
	auto propagate = leaf::defer([ ] { return e_errno{errno}; } );

	if( fseek(&f,0,SEEK_END) )
		return leaf::new_error( input_file_size_error );

	int s = ftell(&f);
	if( s==-1L )
		return leaf::new_error( input_file_size_error );

	if( fseek(&f,0,SEEK_SET) )
		return leaf::new_error( input_file_size_error );

	return s;
}


leaf::result<void> file_read( FILE & f, void * buf, int size )
{
	int n = fread(buf,1,size,&f);
	if( ferror(&f) )
		return leaf::new_error( input_file_read_error, e_errno{errno} );

	if( n!=size )
		return leaf::new_error( input_eof_error );

	return { };
}


leaf::result<char const *> parse_command_line( int argc, char const * argv[ ] )
{
	if( argc==2 )
		return argv[1];
	else
		return leaf::new_error(bad_command_line);
}


int main( int argc, char const * argv[ ] )
{
	return leaf::handle_all(

		[&]() -> leaf::result<int>
		{
			LEAF_AUTO(file_name, parse_command_line(argc,argv));

			LEAF_AUTO(f, file_open(file_name));

			auto propagate1 = leaf::preload( e_file_name{file_name} );

			LEAF_AUTO(s, file_size(*f));

			std::string buffer( 1+s, '\0' );
			LEAF_CHECK(file_read(*f,&buffer[0],buffer.size()-1));

			auto propagate2 = leaf::defer([ ] { return e_errno{errno}; } );
			std::cout << buffer;
			std::cout.flush();
			if( std::cout.fail() )
				return leaf::new_error( cout_error );

			return 0;
		},

		[ ]( leaf::match<error_codes, bad_command_line> )
		{
			std::cout << "Bad command line argument" << std::endl;
			return 1;
		},

		[ ]( leaf::match<error_codes, input_file_open_error>, leaf::match<e_errno, ENOENT>, e_file_name const & fn )
		{
			std::cerr << "File not found: " << fn.value << std::endl;
			return 2;
		},

		[ ]( leaf::match<error_codes, input_file_open_error>, e_errno const & errn, e_file_name const & fn )
		{
			std::cerr << "Failed to open " << fn.value << ", errno=" << errn << std::endl;
			return 3;
		},

		[ ](  leaf::match<error_codes, input_file_size_error, input_file_read_error, input_eof_error>, e_errno const & errn, e_file_name const & fn )
		{
			std::cerr << "Failed to access " << fn.value << ", errno=" << errn << std::endl;
			return 4;
		},

		[ ]( leaf::match<error_codes, cout_error>, e_errno const & errn )
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
		}
	);
}
