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
#include <cassert>

namespace boost { namespace leaf {

	struct e_api_function { char const * value; };

	struct e_file_name { std::string value; };

	struct e_errno
	{
		int value;

		friend std::ostream & operator<<( std::ostream & os, e_errno const & err ) {
			using namespace std;
			return os << type<e_errno>() << " = " << err.value << ", \"" << std::strerror(err.value) << '"';
		}
	};

} }

#endif
