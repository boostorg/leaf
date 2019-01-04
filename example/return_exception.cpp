//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//This example demonstrates how to transport exceptions thrown by a low level function
//through an intermediate context that is not exception-safe, to be handled in a high level
//function which may or may not be exception-safe.

//An real-world example for this use case is when a C API (which may not throw) is implemented
//using a C++ library that throws exceptions. As demonstrated below, these exception objects are
//intercepted and reported by leaf::result<>.

#include <boost/leaf/all.hpp>
#include <exception>
#include <iostream>

namespace leaf = boost::leaf;

class error_base: public virtual std::exception { };
class error_a: public virtual error_base { };
class error_b: public virtual error_base { };
class error_c: public virtual error_base { };

//Lower-level library function which throws exceptions.
int compute_answer_throws()
{
	switch( rand()%4 )
	{
		default: return 42;
		case 1: throw error_a();
		case 2: throw error_b();
		case 3: throw error_c();
	}
}

//A wrapper function that provides a noexcept interface to compute_answer_throws (above),
//by transporting different exception types using leaf::result<int>.
leaf::result<int> compute_answer() noexcept
{
	//We define special handling for error_a and error_b: exception objects of these types
	//will be returned directly as LEAF errors...
	try
	{
		return compute_answer_throws();
	}
	catch( error_a const & e )
	{
		return leaf::error(e);
	}
	catch( error_b const & e )
	{
		return leaf::error(e);
	}
	catch(...)
	{
		//...but "unknown" exceptions are still captured and transported using std::exception_ptr.
		//This is less efficient, because it will likely allocate memory dynamically.
		return leaf::error(std::current_exception());
	}
}

//Intermediate caller of compute_answer.
leaf::result<void> print_answer() noexcept
{
	if( leaf::result<int> r = compute_answer() )
	{
		std::cout << "Answer: " << r.value() << std::endl;
		return { };
	}
	else
		return r.error();
}

int main()
{
	//Exercise print_answer a few times and handle errors. Note that the exception objects that
	//compute_answer_throws throws will land in the exp object, rather than arrive as exceptions...
	for( int i=0; i!=10; ++i )
	{
		leaf::expect<error_a,error_b,std::exception_ptr> exp;
		if( leaf::result<void> r = print_answer() )
			continue;
		else
		{
			exp.handle_error( r,

				[ ]( error_a const & e )
				{
					std::cerr << "Error A!" << std::endl;
				},

				[ ]( error_b const & e )
				{
					std::cerr << "Error B!" << std::endl;
				},

				//...except for error_c errors, which (for demonstration) are captured as exceptions into
				//std::exception_ptr, together with any other unknow exception. Presumably this should
				//never happen, therefore at this point we treat this situation a a logic error: we print
				//diagnostic information and bail out.
				[ ]( std::exception_ptr const & ep )
				{
					assert(ep);
					try
					{
						std::rethrow_exception(ep);
					}
					catch(...)
					{
						leaf::diagnostic_output_current_exception(std::cerr);
					}
				}

			);
		}
	}
	return 0;
}
