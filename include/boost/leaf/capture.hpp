//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_42603B6CB97111E893DFE71A0D39171A
#define UUID_42603B6CB97111E893DFE71A0D39171A

#include <boost/leaf/detail/tl_slot.hpp>
#include <vector>

namespace
boost
	{
	namespace
	leaf
		{
		class
		capture
			{
			capture( capture const & ) = delete;
			capture & operator=( capture const & ) = delete;
			typedef std::vector<std::shared_ptr<void const> > container_t;
			container_t const info_;
			public:
			capture() noexcept:
				info_( [ ]
					{
					using namespace leaf_detail;
					container_t v;
					tl_slot_base::enumerate_put( [&v]( tl_slot_base & p )
						{
						if( std::shared_ptr<void const> c = p.capture_if_has_value() )
							v.push_back(c);
						} );
					return v;
					} () )
				{
				}
			};
		}
	}

#endif
