#ifndef BOOST_LEAF_7062AB340F9411E9A7CFDBC88C7F4AFC
#define BOOST_LEAF_7062AB340F9411E9A7CFDBC88C7F4AFC

//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/error.hpp>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		inline void slot_base::diagnostic_output( std::ostream & os, error const * e )
		{
			for( slot_base const * p = first(); p; p=p->next_ )
				if( p->slot_diagnostic_output(os,e) )
					os << std::endl;
		}
	}

	inline void diagnostic_output( std::ostream & os, error const & e )
	{
		os << "leaf::error serial number: " << e << std::endl;
		leaf_detail::slot_base::diagnostic_output(os,&e);
	}

	inline void diagnostic_output( std::ostream & os )
	{
		leaf_detail::slot_base::diagnostic_output(os,0);
	}

	template <class T>
	class result;

	template <class T>
	void diagnostic_output( std::ostream & os, result<T> const & r )
	{
		assert(!r);
		if( r.which_==leaf_detail::result_variant::err )
			return diagnostic_output(os,r.err_);
		else
		{
			assert(r.which_==leaf_detail::result_variant::cap);
			return diagnostic_output(os,r.cap_);
		}
	}

} }

#endif
