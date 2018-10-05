//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//This is a simple program that demonstrates the use of LEAF to transport error info between threads,
//without using exception handling. See transport_eh.cpp for the (simpler) exception-handling variant.

#include <boost/leaf/transport.hpp>
#include <boost/leaf/expect.hpp>
#include <boost/leaf/put.hpp>
#include <string>
#include <future>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>

namespace leaf = boost::leaf;

//Define several error info types.
struct failed_thread_id { std::thread::id value; };
struct failure_info1 { std::string value; };
struct failure_info2 { int value; };

//A type that represents a successfully generated value from a task.
struct task_result { };

//An exception type thrown in case of task failure.
struct failure : virtual std::exception { };

//This is a test task which succeeds or fails depending on its argument. In case of success, it returns
//task_result, in case of error it throws failure.
task_result
task( bool succeed )
	{
	auto put = leaf::preload( failed_thread_id{std::this_thread::get_id()} ); //Report this thread's id (if expected).

	if( succeed )
		return task_result { };
	else
		leaf::throw_with_info( failure(), failure_info1{"info"}, failure_info2{42} ); //Also report both failure_info1 and failure_info2 (if expected).
	}

//Launch the specified number of asynchronous tasks. In case an asynchronous task throws, its error info
//(of the types used to instantiate leaf::transport) will be automatically captured and transported to the
//main thread.
std::vector<std::future<task_result>>
launch_async_tasks( int thread_count )
	{
	std::vector<std::future<task_result>> fut;
	std::generate_n( std::inserter(fut,fut.end()), thread_count, [ ]
		{
		return std::async( std::launch::async,
			leaf::transport<failed_thread_id,failure_info1,failure_info2>( [ ] { return task(rand()%4); } ) );
		} );
	return fut;
	}

int
main()
	{
	//Launch asynchronous tasks.
	auto fut = launch_async_tasks(42);

	//Collect results or deal with failures.
	for( auto & f : fut )
		{
		f.wait();
		try
			{
			//Get the result, or setup any exception info from the task thread into this thread.
			task_result r = leaf::get(f);

			//No exception, consume the result.
			std::cout << "Success!" << std::endl;
			}
		catch( failure const & )
			{
			//Caught exception from the task. Inspect and print the info.
			leaf::available info;

			unwrap( info.match<failure_info1,failure_info2,failed_thread_id>( [ ] ( std::string const & v1, int v2, std::thread::id tid )
				{
				std::cerr << "Error in thread " << tid << "! failure_info1: " << v1 << ", failure_info2: " << v2 << std::endl;
				} ) );
			}
		}
	}
