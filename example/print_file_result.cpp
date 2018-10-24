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

//Errors
enum class print_file_error
{
	file_open,
	file_size,
	file_read,
	eof,
	cout_write
};

struct e_print_file_error { print_file_error value; };

leaf::result<std::shared_ptr<FILE>> file_open( char const * file_name )
{
	if( FILE * f = fopen(file_name,"rb") )
		return std::shared_ptr<FILE>(f,&fclose);
	else
		return leaf::error( e_print_file_error{print_file_error::file_open}, e_file_name{file_name}, e_errno{errno} );
}

leaf::result<int> file_size( FILE & f )
{
	auto propagate = leaf::defer(&leaf::get_errno);

	if( fseek(&f,0,SEEK_END) )
		return leaf::error( e_print_file_error{print_file_error::file_size} );

	int s = ftell(&f);
	if( s==-1L )
		return leaf::error( e_print_file_error{print_file_error::file_size} );

	if( fseek(&f,0,SEEK_SET) )
		return leaf::error( e_print_file_error{print_file_error::file_size} );

	return s;
}

leaf::result<void> file_read( FILE & f, void * buf, int size )
{
	int n = fread(buf,1,size,&f);
	if( ferror(&f) )
		return leaf::error( e_print_file_error{print_file_error::file_read}, e_errno{errno} );

	if( n!=size )
		return leaf::error( e_print_file_error{print_file_error::eof} );

	return { };
}

leaf::result<void> print_file( char const * file_name )
{
	LEAF_AUTO(f,file_open(file_name));

	leaf::preload( e_file_name{file_name} );

	LEAF_AUTO(s,file_size(*f));

	std::string buffer( 1+s, '\0' );
	LEAF_CHECK(file_read(*f,&buffer[0],buffer.size()-1));

	std::cout << buffer;
	std::cout.flush();
	if( std::cout.fail() )
		return leaf::error( e_print_file_error{print_file_error::cout_write} );

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

	//We expect e_print_file_error, e_file_name and e_errno error info to arrive with errors handled in this function.
	leaf::expect<e_print_file_error, e_file_name, e_errno> exp;

	if( auto r = print_file(fn) )
	{
		return 0; //Success, we're done!
	}
	else
	{
		switch( auto ec = *leaf::peek<e_print_file_error>(exp,r) ) //Switch based on the reported e_print_file_error.
		{
			case print_file_error::file_open:
			{
				//handle_error is given a list of match objects (in this case only one), which it attempts to match (in order) to
				//available error info (if none can be matched, it returns false). When a match is found, the corresponding
				//lambda is invoked.
				bool matched = handle_error( exp, r,
					leaf::match<e_file_name,e_errno>( [ ] ( std::string const & fn, int errn )
					{
						if( errn==ENOENT )
							std::cerr << "File not found: " << fn << std::endl;
						else
							std::cerr << "Failed to open " << fn << ", errno=" << errn << std::endl;
					} ) );
				assert(matched);
				return 2;
			}

			case print_file_error::cout_write:
			case print_file_error::file_size:
			case print_file_error::file_read:
			case print_file_error::eof:
			{
				//handle_error is given a list of match objects, which it attempts to match (in order) to available error info.
				//In this case it will first check if both e_file_name and e_errno are avialable; if not, it will next check
				//if just e_errno is available; and if not, the last match will match even if no error info is available,
				//to print a generic error message.
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
					} ) );
				assert(matched);
				return 3;
			}

			default:
				//This catch-all case is designed to help diagnose logic errors (missing case labels in the switch statement).
				std::cerr << "Unknown error code " << int(ec) << ", cryptic information follows." << std::endl;
				diagnostic_print(std::cerr,exp,r);
				return 4;
		}
	}
}
