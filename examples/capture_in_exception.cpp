// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This is a simple program that demonstrates the use of LEAF to transport E-objects between threads,
// without using exception handling. See capture_eh.cpp for the exception-handling variant.

// See also

#include <boost/leaf/capture.hpp>
#include <boost/leaf/handle_exception.hpp>
#include <boost/leaf/exception.hpp>
#include <vector>
#include <string>
#include <future>
#include <iterator>
#include <iostream>
#include <algorithm>

namespace leaf = boost::leaf;

// Define several e-types.
struct e_thread_id { std::thread::id value; };
struct e_failure_info1 { std::string value; };
struct e_failure_info2 { int value; };

// A type that represents a successfully returned result from a task.
struct task_result { };

 // This is our task function. It produces objects of type task_result, but it may fail...
task_result task()
{
	bool succeed = (rand()%4) !=0; //...at random.
	if( succeed )
		return task_result { };
	else
		throw leaf::exception( std::exception(),
			e_thread_id{std::this_thread::get_id()},
			e_failure_info1{"info"},
			e_failure_info2{42} );
};

int main()
{
	int const task_count = 42;

	// The error_handler is called in this thread (see leaf::error_try_ below), eath time we get a future
	// from a worker that failed. The arguments passed to individual lambdas are transported from
	// the worker thread to the main thread automatically.
	auto error_handler = []( leaf::error_info const & error )
	{
		return leaf::remote_handle_exception( error,
			[]( e_failure_info1 const & v1, e_failure_info2 const & v2, e_thread_id const & tid )
			{
				std::cerr << "Error in thread " << tid.value << "! failure_info1: " << v1.value << ", failure_info2: " << v2.value << std::endl;
			},
			[]( leaf::diagnostic_info const & unmatched )
			{
				std::cerr <<
					"Unknown failure detected" << std::endl <<
					"Cryptic diagnostic information follows" << std::endl <<
					unmatched;
			} );
	};

	// Container to collect the generated std::future objects.
	std::vector<std::future<task_result>> fut;

	// Launch the tasks, but rather than launching the task function directly, we launch a
	// wrapper function which calls leaf::capture, passing a context object that will hold
	// the E-objects loaded from the task in case it throws. The E-types the context is
	// able to hold are automatically deduced from the type of the error_handler function.
	std::generate_n( std::inserter(fut,fut.end()), task_count,
		[&]
		{
			return std::async(
				std::launch::async,
				[&]
				{
					return leaf::capture(leaf::make_shared_context(&error_handler), &task);
				} );
		} );

	// Wait on the futures, get the task results, handle errors.
	for( auto & f : fut )
	{
		f.wait();

		leaf::remote_try_catch(
			[&]
			{
				task_result r = f.get();

				// Success! Use r to access task_result.
				std::cout << "Success!" << std::endl;
				(void) r;
			},
			[&]( leaf::error_info const & error )
			{
				return error_handler(error);
			} );
	}
}
