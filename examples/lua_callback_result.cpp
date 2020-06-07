// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This is a simple program that shows how to load error objects out
// of a C-callback, and converting them to leaf::result<T> as soon as control
// reaches C++.

extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
}
#include <boost/leaf/handle_error.hpp>
#include <boost/leaf/result.hpp>
#include <boost/leaf/on_error.hpp>
#include <iostream>
#include <stdlib.h>

namespace leaf = boost::leaf;

enum do_work_error_code
{
	ec1=1,
	ec2
};

struct e_lua_pcall_error { int value; };
struct e_lua_error_message { std::string value; };


// This is a C callback with a specific signature, callable from programs written in Lua.
// If it succeeds, it returns an int answer, by pushing it onto the Lua stack. But "sometimes"
// it fails, in which case it calls luaL_error. This causes the Lua interpreter to abort and pop
// back into the C++ code which called it (see call_lua below).
int do_work( lua_State * L ) noexcept
{
	bool success=rand()%2; // "Sometimes" do_work fails.
	if( success )
	{
		lua_pushnumber(L,42); // Success, push the result on the Lua stack, return to Lua.
		return 1;
	}
	else
	{
		// Generate a new error_id and associate a do_work_error_code with it. Normally, we'd
		// return this in a leaf::result<T>, but the do_work function signature (required by Lua)
		// does not permit this.
		leaf::new_error(ec1);

		// Tell the Lua interpreter to abort the Lua program.
		return luaL_error(L,"do_work_error");
	}
}


std::shared_ptr<lua_State> init_lua_state() noexcept
{
	// Create a new lua_State, we'll use std::shared_ptr for automatic cleanup.
	std::shared_ptr<lua_State> L(lua_open(),&lua_close);

	// Register the do_work function (above) as a C callback, under the global
	// Lua name "do_work". With this, calls from Lua programs to do_work
	// will land in the do_work C function we've registered.
	lua_register( &*L, "do_work", &do_work );

	// Pass some Lua code as a C string literal to Lua. This creates a global Lua
	// function called "call_do_work", which we will later ask Lua to execute.
	luaL_dostring( &*L, "\
\n      function call_do_work()\
\n          return do_work()\
\n      end" );

	return L;
}


// Here we will ask Lua to execute the function call_do_work, which is written
// in Lua, and returns the value from do_work, which is written in C++ and
// registered with the Lua interpreter as a C callback.

// If do_work succeeds, we return the resulting int answer in leaf::result<int>.
// If it fails, we'll communicate that failure to our caller.
leaf::result<int> call_lua( lua_State * L )
{
	// Ask the Lua interpreter to call the global Lua function call_do_work.
	lua_getfield( L, LUA_GLOBALSINDEX, "call_do_work" );
	leaf::augment_id augment;
	if( int err=lua_pcall(L,0,1,0) ) // Ask Lua to call the global function call_do_work.
	{
		auto load = leaf::on_error( e_lua_error_message{lua_tostring(L,1)} );
		lua_pop(L,1);

		// get_error will return the error_id generated in our Lua callback. This is the
		// same error_id the preload (above) uses as well.
		return augment.get_error( e_lua_pcall_error{err} );
	}
	else
	{
		// Success! Just return the int answer.
		int answer=lua_tonumber(L,-1);
		lua_pop(L,1);
		return answer;
	}
}

int main()
{
	std::shared_ptr<lua_State> L=init_lua_state();

	for( int i=0; i!=10; ++i )
	{
		leaf::try_handle_all(

			[&]() -> leaf::result<void>
			{
				LEAF_AUTO(answer, call_lua(&*L));
				std::cout << "do_work succeeded, answer=" << answer << '\n';
				return { };
			},

			[]( do_work_error_code e )
			{
				std::cout << "Got do_work_error_code = " << e <<  "!\n";
			},

			[]( e_lua_pcall_error const & err, e_lua_error_message const & msg )
			{
				std::cout << "Got e_lua_pcall_error, Lua error code = " << err.value << ", " << msg.value << "\n";
			},

			[]( leaf::error_info const & unmatched )
			{
				std::cerr <<
					"Unknown failure detected" << std::endl <<
					"Cryptic diagnostic information follows" << std::endl <<
					unmatched;
			} );
	}

	return 0;
}
