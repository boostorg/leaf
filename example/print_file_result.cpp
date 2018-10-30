//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//This example demonstrates the basic use of LEAF to augment error conditions with
//additional information when using result<T> to report failures. See print_file_eh.cpp
//for the variant that uses exception handling.

#include <boost/leaf/all.hpp>
#include <iostream>
#include <memory>
#include <stdio.h>

namespace leaf = boost::leaf;


//We could define our own error info types, but for this example the ones
//defined in <boost/leaf/common.hpp> are a perfect match.
using leaf::e_file_name;
using leaf::e_errno;


//Error codes
enum
{
	input_file_open_error,
	input_file_size_error,
	input_file_read_error,
	input_eof_error,
	cout_error
};
struct e_error_code { int value; };


leaf::result<std::shared_ptr<FILE>> file_open( char const * file_name )
{
	if( FILE * f = fopen(file_name,"rb") )
		return std::shared_ptr<FILE>(f,&fclose);
	else
		return leaf::error( e_error_code{input_file_open_error}, e_file_name{file_name}, e_errno{errno} );
}


leaf::result<int> file_size( FILE & f )
{
	auto propagate = leaf::defer([ ] { return e_errno{errno}; } );

	if( fseek(&f,0,SEEK_END) )
		return leaf::error( e_error_code{input_file_size_error} );

	int s = ftell(&f);
	if( s==-1L )
		return leaf::error( e_error_code{input_file_size_error} );

	if( fseek(&f,0,SEEK_SET) )
		return leaf::error( e_error_code{input_file_size_error} );

	return s;
}


leaf::result<void> file_read( FILE & f, void * buf, int size )
{
	int n = fread(buf,1,size,&f);
	if( ferror(&f) )
		return leaf::error( e_error_code{input_file_read_error}, e_errno{errno} );

	if( n!=size )
		return leaf::error( e_error_code{input_eof_error} );

	return { };
}


leaf::result<void> print_file( char const * file_name )
{
	LEAF_AUTO(f,file_open(file_name));

	auto propagate = leaf::preload( e_file_name{file_name} );

	LEAF_AUTO(s,file_size(*f));

	std::string buffer( 1+s, '\0' );
	LEAF_CHECK(file_read(*f,&buffer[0],buffer.size()-1));

	std::cout << buffer;
	std::cout.flush();
	if( std::cout.fail() )
		return leaf::error( e_error_code{cout_error} );

	return { };
}


char const * parse_command_line( int argc, char const * argv[ ] )
{
	if( argc!=2 )
		return 0;
	else
		return argv[1];
}


int main( int argc, char const * argv[ ] )
{
	char const * fn = parse_command_line(argc,argv);
	if( !fn )
	{
		std::cout << "Bad command line argument" << std::endl;
		return 1;
	}

	//We expect e_error_code, e_file_name and e_errno objects to arrive with errors handled in this function.
	//They will be stored inside exp.
	leaf::expect<e_error_code, e_file_name, e_errno> exp;

	if( auto r = print_file(fn) )
	{
		return 0; //Success, we're done!
	}
	else
	{
		//Probe exp for objects associated with the error stored in r.
		switch( auto ec = *leaf::peek<e_error_code>(exp,r) )
		{
			case input_file_open_error:
			{
				//handle_error takes a list of match objects (in this case only one), each given a set of e_ types. It
				//attempts to match each set (in order) to objects of e_ types, associated with r, available in exp.
				//If no set can be matched, handle_error returns false. When a match is found, handle_error calls
				//the corresponding lambda, passing the .value of each of the e_ types from the matched set.
				bool matched = handle_error( exp, r,

					leaf::match<e_file_name,e_errno>( [ ] ( std::string const & fn, int errn )
					{
						if( errn==ENOENT )
							std::cerr << "File not found: " << fn << std::endl;
						else
							std::cerr << "Failed to open " << fn << ", errno=" << errn << std::endl;
					} )

				);
				assert(matched);
				return 2;
			}

			case input_file_size_error:
			case input_file_read_error:
			case input_eof_error:
			{
				//In this case handle_error is given 3 match sets. It will first check if both e_file_name and e_errno,
				//associated with r, are avialable in exp; if not, it will next check if just e_errno is available; and if
				//not, the last (empty) set will always match to print a generic error message.
				bool matched = handle_error( exp, r,

					leaf::match<e_file_name,e_errno>( [ ] ( std::string const & fn, int errn )
					{
						std::cerr << "Failed to access " << fn << ", errno=" << errn << std::endl;
					} ),

					leaf::match<e_errno>( [ ] ( int errn )
					{
						std::cerr << "I/O error, errno=" << errn << std::endl;
					} ),

					leaf::match<>( [ ]
					{
						std::cerr << "I/O error" << std::endl;
					} )

				);
				assert(matched);
				return 3;
			}

			case cout_error:
			{
				//Report failure to write to std::cout, print the relevant errno.
				bool matched = handle_error( exp, r,

					leaf::match<e_errno>( [ ] ( int errn )
					{
						std::cerr << "Output error, errno=" << errn << std::endl;
					} )

				);
				assert(matched);
				return 4;
			}

			default:
				//This catch-all case helps diagnose logic errors (presumably, missing case labels in the switch statement).
				std::cerr << "Unknown error code " << int(ec) << ", cryptic information follows." << std::endl;
				diagnostic_print(std::cerr,exp,r);
				return 5;
		}
	}
}
