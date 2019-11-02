#ifndef LEAF_EBA7EF10B6F311E8AAD493990D39171A
#define LEAF_EBA7EF10B6F311E8AAD493990D39171A

// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/print.hpp>
#include <string>
#include <cerrno>
#include <cassert>
#ifdef _WIN32
#include <Windows.h>
#include <cstring>
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#endif

namespace boost { namespace leaf {

	struct e_api_function { char const * value; };

	struct e_file_name { std::string value; };

	struct e_errno
	{
		int value;

		friend std::ostream & operator<<( std::ostream & os, e_errno const & err )
		{
			return os << type<e_errno>() << ": " << err.value << ", \"" << std::strerror(err.value) << '"';
		}
	};

	struct e_type_info_name { char const * value; };

	struct e_at_line { int value; };

	namespace windows
	{
		struct e_LastError
		{
			unsigned value;

#ifdef _WIN32
			friend std::ostream & operator<<( std::ostream & os, e_LastError const & err )
			{
				struct msg_buf
				{
					LPVOID * p;
					msg_buf(): p(0) { }
					~msg_buf() { if(p) LocalFree(p); }
				};
				msg_buf mb;
				if( FormatMessageA(
					FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
					0,
					err.value,
					MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
					(LPSTR)&mb.p,
					0,
					0) )
				{
					assert(mb.p!=0);
					char * z = std::strchr((LPSTR)mb.p,0);
					if( z[-1] == '\n' )
						*--z = 0;
					if( z[-1] == '\r' )
						*--z = 0;
					return os << type<e_LastError>() << ": " << err.value << ", \"" << (LPCSTR)mb.p << '"';
				}
				return os;
			}
#else
			// TODO : Other platforms
#endif
		};
	}

} }

#endif
