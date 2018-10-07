//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//This is a simple program that demonstrates the use of LEAF to transport error info between threads,
//without using exception handling. See transport_eh.cpp for the (simpler) exception-handling variant.

#include <boost/leaf/all.hpp>
#include <string>
#include <future>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <boost/variant.hpp>

namespace leaf = boost::leaf;

//Define several error info types.
struct failed_thread_id { std::thread::id value; };
struct failure_info1 { std::string value; };
struct failure_info2 { int value; };

//A type that represents a successfully generated value from a task.
struct task_result { };

//A type that represents an error code.
enum error {
	ok,
	err
};

//A task returns either a successfully generated task_result, or an error.
using result_t = boost::variant<task_result,error>;

//This is a test task which succeeds or fails depending on its argument.
result_t
task( bool succeed )
	{
	auto put = leaf::preload( failed_thread_id{std::this_thread::get_id()} ); //Report this thread's id (if expected).

	if( succeed )
		return task_result { };
	else
		{
		leaf::put( failure_info1{"info"}, failure_info2{42} ); //Also report both failure_info1 and failure_info2 (if expected
		return err;
		}
	}

//A dummy function which simply returns true. Used in the call to leaf::set_has_current_error below.
bool
return_true() noexcept
	{
	return true;
	}

//A wrapper for result_t, which automatically captures all error info in case of a failure.
struct
result_t_with_info
	{
	result_t res;
	leaf::capture cap;
	explicit
	result_t_with_info( result_t && res ):
		res(std::move(res)),
		cap(res.which()>0)
		{
		}
	};

//Launch the specified number of asynchronous tasks. In case an asynchronous task fails, its error info
//(of the types used to instantiate leaf::transport) is captured and wrapped in a result_t_with_info,  and
//transported to the main thread.
std::vector<std::future<result_t_with_info>>
launch_async_tasks( int thread_count )
	{
	std::vector<std::future<result_t_with_info>> fut;
	std::generate_n( std::inserter(fut,fut.end()), thread_count, [ ]
		{
		return std::async( std::launch::async,
			leaf::transport<failed_thread_id,failure_info1,failure_info2>( [ ]
				{
				//Instruct leaf::preload to always put() its payload during stack unwinding, not only if there
				//is an uncaught_exception(). If possible, we should supply a function that evaluates to true
				//if there is a pending error in this thread, false otherwise. Our function always returns true.
				leaf::set_has_current_error(&return_true);

				return result_t_with_info(task(rand()%4));
				} ) );
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
		result_t_with_info v = f.get();
		if( v.res.which()==0 )
			{
			//Success! Just take the task_result.
			task_result r = boost::get<task_result>(v.res);
			std::cout << "Success!" << std::endl;
			}
		else
			{
			//Failure! Release into this thread all info captured in the worker thread.
			assert(v.res.which()==1);
			v.cap.release();

			//Inspect and print the info.
			leaf::available info;

			unwrap( info.match<failure_info1,failure_info2,failed_thread_id>( [ ] ( std::string const & v1, int v2, std::thread::id tid )
				{
				std::cerr << "Error in thread " << tid << "! failure_info1: " << v1 << ", failure_info2: " << v2 << std::endl;
				} ) );
			}
		}
	}
