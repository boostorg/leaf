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
		class diagnostic_information_;
		static constexpr diagnostic_information_ const * diagnostic_information = 0;
		inline
		std::ostream &
		operator<<( std::ostream & os, diagnostic_information_ const * const & )
			{
			using namespace leaf_detail;
			char const * function=0;
			char const * file=0;
			int line=-1;
			tl_slot_base::enumerate_put( [&os,&function,&file,&line]( tl_slot_base const & info )
				{
				if( &info==&tl_slot<ei_source_location<in_function>>::tl_instance() )
					function = static_cast<tl_slot<ei_source_location<in_function>> const &>(info).value().value;
				if( &info==&tl_slot<ei_source_location<in_file>>::tl_instance() )
					file = static_cast<tl_slot<ei_source_location<in_file>> const &>(info).value().value;
				if( &info==&tl_slot<ei_source_location<at_line>>::tl_instance() )
					line = static_cast<tl_slot<ei_source_location<at_line>> const &>(info).value().value;
				if( info.diagnostic_print(os) )
					os << std::endl;
				} );
			if( file )
				if( line==-1 )
					os << "In " << file << std::endl;
				else
					os << "At " << file << '(' << line << ')' << std::endl;
			if( function )
				os << "In function " << function << std::endl;
			return os;
			}
		}
	}

#endif
