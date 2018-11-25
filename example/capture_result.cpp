//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//This is a simple program that demonstrates the use of LEAF to transport e-objects between threads,
//without using exception handling. See capture_eh.cpp for the exception-handling variant.

#include <boost/leaf/all.hpp>
#include <vector>
#include <string>
#include <future>
#include <iterator>
#include <iostream>
#include <algorithm>

namespace leaf = boost::leaf;

//Define several e-types.
struct e_thread_id { std::thread::id value; };
struct e_failure_info1 { std::string value; };
struct e_failure_info2 { int value; };
struct e_failure_info3 { long value; };
struct e_failure_info4 { float value; };

//A type that represents a successfully returned result from a task.
struct task_result { };

//This is a test task which succeeds or fails depending on its argument.
leaf::result<task_result> task( bool succeed )
{
	if( succeed )
		return task_result(); //Simulate successful result.
	else
		return leaf::error(
			e_thread_id{std::this_thread::get_id()},
			e_failure_info1{"info"},
			e_failure_info2{42},
			e_failure_info4{42} );
}

//Launch the specified number of asynchronous tasks. In case an asynchronous task fails, its e-objects
//(initially stored in exp) are captured in a leaf::result<task_result>, which transports them to the main thread.
template <class... E>
std::vector<std::future<leaf::result<task_result>>> launch_async_tasks( int thread_count )
{
	std::vector<std::future<leaf::result<task_result>>> fut;
	std::generate_n( std::inserter(fut,fut.end()), thread_count, [ ]
	{
		return std::async( std::launch::async, [ ]
		{
			leaf::expect<E...> exp;
			return capture(exp,task((rand()%4)!=0));
		} );
	} );
	return fut;
}

int main()
{
	//Launch tasks, transport the specified e-types. For demonstration, note that the task provides
	//failure_info4 which we don't care about, and that we say we could use failure_info3, but which
	//the task doesn't provide. So, we'll only get failed_thread_id, failure_info1 and failure_info2.
	auto fut = launch_async_tasks<e_thread_id, e_failure_info1, e_failure_info2, e_failure_info3>(42);

	//Collect results or deal with failures.
	for( auto & f : fut )
	{
		f.wait();

		//Storage for e-objects.
		leaf::expect<e_thread_id, e_failure_info1, e_failure_info2, e_failure_info3> exp;

		//Get the task result, check for success.
		if( leaf::result<task_result> r = f.get() )
		{
			//Success! Use *r to access task_result.
			std::cout << "Success!" << std::endl;
		}
		else
		{
			//Failure! Handle error, print failure info.
			bool matched = handle_error( exp, r,
				[ ] ( e_failure_info1 const & v1, e_failure_info2 const & v2, e_thread_id const & tid )
				{
					std::cerr << "Error in thread " << tid.value << "! failure_info1: " << v1.value << ", failure_info2: " << v2.value << std::endl;
				} );
			assert(matched);
		}
	}
}
