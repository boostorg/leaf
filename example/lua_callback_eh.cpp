//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//This is a simple program that shows how to propagate leaf::error objects out
//of a C-callback, and converting them to leaf::result<T> as soon as control
//reaches C++.

extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
}
#include <boost/leaf/all.hpp>
#include <iostream>
#include <stdlib.h>

namespace leaf = boost::leaf;

struct lua_failure: std::exception { };

enum do_work_error_code
{
	ec1=1,
	ec2
};
namespace boost { namespace leaf {
	template<> struct is_error_type<do_work_error_code>: std::true_type { };
} }

struct e_lua_pcall_error { int value; };
struct e_lua_error_message { std::string value; };


//This is a C callback function with a specific signature, made accessible to programs
//written in Lua.

//If it succeeds, it returns an int answer, by pushing it onto the Lua stack. But "sometimes"
//it fails, in which case it calls luaL_error. This causes the Lua interpreter to abort and pop
//back into the C++ code which called it (see call_lua below).
int do_work( lua_State * L )
{
	bool success=rand()%2;
	if( success )
	{
		lua_pushnumber(L,42); //Return 42 to the calling Lua program.
		return 1;
	}
	else
	{
		//Remarkably, the Lua interpreter is exception-safe. So, just throw.
		throw leaf::exception<lua_failure>(ec1);
	}
}


std::shared_ptr<lua_State> init_lua_state()
{
	//Create a new lua_State, we'll use std::shared_ptr for automatic cleanup.
	std::shared_ptr<lua_State> L(lua_open(),&lua_close);

	//Register the do_work function (above) as a C callback, under the global
	//Lua name "do_work". With this, calls from Lua programs to do_work
	//will land in the do_work C function we've registered.
	lua_register( &*L, "do_work", &do_work );

	//Pass some Lua code as a C string literal to Lua. This creates a global Lua
	//function called "call_do_work", which we will later ask Lua to execute.
	luaL_dostring( &*L, "\
\n      function call_do_work()\
\n          return do_work()\
\n      end" );

	return L;
}


//Here we will ask Lua to execute the function call_do_work, which is written
//in Lua, and returns the value from do_work, which is written in C++ and
//registered with the Lua interpreter as a C callback.

//If do_work succeeds, we return the resulting int answer. If it fails, we'll
//communicate that failure to our caller.
int call_lua( lua_State * L )
{
	//Ask the Lua interpreter to call the global Lua function call_do_work.
	lua_getfield( L, LUA_GLOBALSINDEX, "call_do_work" );
	if( int err=lua_pcall(L,0,1,0) )
	{
		//Something went wrong with the call, so we'll throw lua_failure.
		//This is definitely not a do_work failure, because it throws on error.
		auto propagate = leaf::preload( e_lua_error_message{lua_tostring(L,1)} );
		lua_pop(L,1);
		throw leaf::exception<lua_failure>( e_lua_pcall_error{err} );
	}
	else
	{
		//Success! Just return the int answer.
		int answer=lua_tonumber(L,-1);
		lua_pop(L,1);
		return answer;
	}
}

int main() noexcept
{
	std::shared_ptr<lua_State> L=init_lua_state();
	leaf::expect<do_work_error_code,e_lua_pcall_error,e_lua_error_message> exp;
	for( int i=0; i!=10; ++i )
		try
		{
			int r = call_lua(&*L);
			std::cout << "do_work succeeded, answer=" << r << '\n';
		}
		catch( lua_failure const & e )
		{
			handle_exception( exp, e,

				//Handle do_work failures:
				[ ]( do_work_error_code const & e )
				{
					std::cout << "Got do_work_error_code = " << e <<  "!\n";
				},

				//Handle all other lua_pcall failures:
				[ ]( e_lua_pcall_error const & err, e_lua_error_message const & msg )
				{
					std::cout << "Got e_lua_pcall_error, Lua error code = " << err.value << ", " << msg.value << "\n";
				}
			);
		}
	return 0;
}
