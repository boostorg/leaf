//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef EBA7EF10B6F311E8AAD493990D39171A
#define EBA7EF10B6F311E8AAD493990D39171A

#include <string>
#include <cstring>
#include <cerrno>

namespace
boost
	{
	namespace
	leaf
		{
		struct xi_api_function { char const * value; };
		struct xi_file_name { std::string value; };

		struct
		xi_errno
			{
			int value;
			friend
			inline
			std::ostream &
			operator<<( std::ostream & os, xi_errno const & err )
				{
				using namespace std;
				os << type<xi_errno>() << " = " << err.value << ", \"" << strerror(err.value) << '"';
				return os;
				}
			};
		inline
		xi_errno
		get_errno()
			{
			using namespace std;
			return xi_errno{errno};
			}

		struct throw_function;
		struct throw_file;
		struct throw_line;
		template <class Tag> struct xi_source_location { char const * value; };
		template <> struct xi_source_location<throw_line> { int value; };
		}
	}

#define xi_THROW_LOCATION\
	::boost::leaf::xi_source_location<::boost::leaf::throw_function> {__FUNCTION__},\
	::boost::leaf::xi_source_location<::boost::leaf::throw_file> {__FILE__},\
	::boost::leaf::xi_source_location<::boost::leaf::throw_line> {__LINE__}

#endif
