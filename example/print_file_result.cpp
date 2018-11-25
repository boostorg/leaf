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


//We could define our own e-types, but for this example the ones
//defined in <boost/leaf/common.hpp> are a perfect match.
using leaf::e_file_name;
using leaf::e_errno;


//Error codes
enum error_code
{
	input_file_open_error=1,
	input_file_size_error,
	input_file_read_error,
	input_eof_error,
	cout_error
};
namespace boost { namespace leaf {
	template<> struct is_error_type<error_code>: std::true_type { };
} }


leaf::result<std::shared_ptr<FILE>> file_open( char const * file_name )
{
	if( FILE * f = fopen(file_name,"rb") )
		return std::shared_ptr<FILE>(f,&fclose);
	else
		return leaf::error( input_file_open_error, e_file_name{file_name}, e_errno{errno} );
}


leaf::result<int> file_size( FILE & f )
{
	auto propagate = leaf::defer([ ] { return e_errno{errno}; } );

	if( fseek(&f,0,SEEK_END) )
		return leaf::error( input_file_size_error );

	int s = ftell(&f);
	if( s==-1L )
		return leaf::error( input_file_size_error );

	if( fseek(&f,0,SEEK_SET) )
		return leaf::error( input_file_size_error );

	return s;
}


leaf::result<void> file_read( FILE & f, void * buf, int size )
{
	int n = fread(buf,1,size,&f);
	if( ferror(&f) )
		return leaf::error( input_file_read_error, e_errno{errno} );

	if( n!=size )
		return leaf::error( input_eof_error );

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
		return leaf::error( cout_error );

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

	//We expect error_code, e_file_name and e_errno objects to be associated
	//with errors handled in this function. They will be stored inside of exp.
	leaf::expect<error_code, e_file_name, e_errno> exp;

	if( auto r = print_file(fn) )
	{
		return 0; //Success, we're done!
	}
	else
	{
		//Probe exp for the error_code object associated with the error stored in r.
		switch( auto ec = *leaf::peek<error_code>(exp,r) )
		{
			case input_file_open_error:
			{
				//handle_error takes a list of functions (in this case only one). It attempts to
				//match each function (in order) to objects currently available in exp, which
				//are associated with the error value stored in r. If no function can be matched,
				//handle_error returns false. Otherwise the matched function is invoked with
				//the matching corresponding error objects.
				bool matched = handle_error( exp, r,

					[ ] ( e_file_name const & fn, e_errno const & errn )
					{
						if( errn.value==ENOENT )
							std::cerr << "File not found: " << fn.value << std::endl;
						else
							std::cerr << "Failed to open " << fn.value << ", errno=" << errn << std::endl;
					}

				);
				assert(matched);
				return 2;
			}

			case input_file_size_error:
			case input_file_read_error:
			case input_eof_error:
			{
				//In this case handle_error is given 3 functions. It will first check if both
				//e_file_name and e_errno, associated with r, are avialable in exp; if not, it will
				//next check if just e_errno is available; and if not, the last function (which
				//takes no arguments) will always match to print a generic error message.
				bool matched = handle_error( exp, r,

					[ ] ( e_file_name const & fn, e_errno const & errn )
					{
						std::cerr << "Failed to access " << fn.value << ", errno=" << errn << std::endl;
					},

					[ ] ( e_errno const & errn )
					{
						std::cerr << "I/O error, errno=" << errn << std::endl;
					},

					[ ]
					{
						std::cerr << "I/O error" << std::endl;
					}

				);
				assert(matched);
				return 3;
			}

			case cout_error:
			{
				//Report failure to write to std::cout, print the relevant errno.
				bool matched = handle_error( exp, r,

					[ ] ( e_errno const & errn )
					{
						std::cerr << "Output error, errno=" << errn << std::endl;
					}

				);
				assert(matched);
				return 4;
			}

			//This catch-all case helps diagnose logic errors (presumably, missing case labels
			//in the switch statement).
			default:
			{
				std::cerr << "Unknown error code " << ec << ", cryptic information follows." << std::endl; //<7>
				diagnostic_output(std::cerr,exp,r);
				return 5;
			}
		}
	}
}
