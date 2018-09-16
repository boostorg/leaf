//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_E4B56A60B87011E890CB55B30D39171A
#define UUID_E4B56A60B87011E890CB55B30D39171A

#include <boost/leaf/detail/tl_slot.hpp>

namespace
boost
	{
	namespace
	leaf
		{
		class
		diagnostic_information
			{
			diagnostic_information( diagnostic_information const & );
			diagnostic_information & operator=( diagnostic_information const & );
			public:
			diagnostic_information()
				{
				}
			friend
			std::ostream &
			operator<<( std::ostream & os, diagnostic_information const & )
				{
				using namespace leaf_detail;
				tl_slot_base::enumerate_put( [&os]( tl_slot_base const & info )
					{
					if( info.diagnostic_print(os) )
						os << std::endl;
					} );
				return os;
				}
			};
		}
	}

#endif
