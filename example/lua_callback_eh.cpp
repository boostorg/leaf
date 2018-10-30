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

struct e_do_work_error { int value; };

struct e_lua_pcall_error { int value; };
struct e_lua_error_message { std::string value; };


//This is a C callback function with a specific signature, made accessible to programs
//written in Lua.

//If it succeeds, it returns an int answer, by pushing it onto the Lua stack. But "sometimes"
//it fails, in which case it calls luaL_error. This causes the Lua interpreter to abort and pop
//back into the C++ code which called it (see call_lua below).
int do_work( lua_State * L ) noexcept
{
	bool success=rand()%2;
	if( success )
	{
		lua_pushnumber(L,42); //Return 42 to the calling Lua program.
		return 1;
	}
	else
	{
		//Tell the Lua interpreter to abort the Lua program. Control will reach the
		//call_lua function which called the Lua interpreter. The e_do_work_error
		//is communicated, through the Lua interpreter, to that function.
		auto propagate = leaf::preload( e_do_work_error{-42} );
		return luaL_error(L,"do_work_error");
	}
}


std::shared_ptr<lua_State> init_lua_state() noexcept
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
		//If this is a do_work failure, the e_do_work object preloaded in
		//do_work will become associated with this exception. If not,
		//we will still need to communicate that the lua_pcall failed with an
		//error code and an error message.
		auto propagate = leaf::preload( e_lua_error_message{lua_tostring(L,1)} );
		lua_pop(L,1);
		leaf::throw_exception( lua_failure(), e_lua_pcall_error{err} );
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
	leaf::expect<e_do_work_error,e_lua_pcall_error,e_lua_error_message> exp;
	for( int i=0; i!=10; ++i )
		try
		{
			int r = call_lua(&*L);
			std::cout << "do_work succeeded, answer=" << r << '\n';
		}
		catch( lua_failure const & e )
		{
			handle_exception( exp, e,

				//Handle e_do_work failures:
				leaf::match<e_do_work_error>( [ ]( int v )
				{
					std::cout << "Got e_do_work_error, value = " << v <<  "!\n";
				} ),

				//Handle all other lua_pcall failures:
				leaf::match<e_lua_pcall_error,e_lua_error_message>( [ ]( int err, std::string const & msg )
				{
					std::cout << "Got e_lua_pcall_error, Lua error code = " << err << ", " << msg << "\n";
				} )
			);
		}
	return 0;
}
