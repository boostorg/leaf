//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef EBA7EF10B6F311E8AAD493990D39171A
#define EBA7EF10B6F311E8AAD493990D39171A

#include <boost/leaf/detail/print.hpp>
#include <string>
#include <cstring>
#include <cerrno>

#define LEAF_SOURCE_LOCATION ::boost::leaf::ei_source_location{::boost::leaf::ei_source_location::loc(__FILE__,__LINE__,__FUNCTION__)}

namespace
boost
	{
	namespace
	leaf
		{
		struct
		ei_source_location
			{
			struct
			loc
				{
				char const * const file;
				int const line;
				char const * const function;
				loc( char const * file, int line, char const * function ):
					file(file),
					line(line),
					function(function)
					{
					assert(file!=0);
					assert(line>0);
					assert(function!=0);
					}
				};
			loc value;
			friend
			std::ostream &
			operator<<( std::ostream & os, ei_source_location const & x )
				{
				return os << "At " << x.value.file << '(' << x.value.line << ") in function " << x.value.function << std::endl;
				}
			};
		////////////////////////////////////////
		struct ei_api_function { char const * value; };
		struct ei_file_name { std::string value; };
		////////////////////////////////////////
		struct
		ei_errno
			{
			int value;
			friend
			inline
			std::ostream &
			operator<<( std::ostream & os, ei_errno const & err )
				{
				using namespace std;
				os << type<ei_errno>() << " = " << err.value << ", \"" << std::strerror(err.value) << '"';
				return os;
				}
			};
		inline
		ei_errno
		get_errno() noexcept
			{
			using namespace std;
			return ei_errno{errno};
			}
		}
	}

#endif
