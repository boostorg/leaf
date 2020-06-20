#ifndef UUID_BOOST_LEAF_BOOST_EXCEPTION_SUPPORT_INCLUDED
#define UUID_BOOST_LEAF_BOOST_EXCEPTION_SUPPORT_INCLUDED

// Copyright (c) 2018-2020 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/handle_exception.hpp>
#include <boost/exception/get_error_info.hpp>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <class Tag, class T> struct requires_catch<boost::error_info<Tag, T>>: std::true_type { };
		template <class Tag, class T> struct requires_catch<boost::error_info<Tag, T> const &>: std::true_type { };
		template <class Tag, class T> struct requires_catch<boost::error_info<Tag, T> const *>: std::true_type { };
		template <class Tag, class T> struct requires_catch<boost::error_info<Tag, T> &> { static_assert(sizeof(boost::error_info<Tag, T>)==0, "mutable boost::error_info reference arguments are not supported"); };
		template <class Tag, class T> struct requires_catch<boost::error_info<Tag, T> *> { static_assert(sizeof(boost::error_info<Tag, T>)==0, "mutable boost::error_info pointer arguments are not supported"); };

		template <class SlotsTuple, class Tag, class T>
		struct check_one_argument<SlotsTuple, boost::error_info<Tag, T>>
		{
			static boost::error_info<Tag, T> * check( SlotsTuple & tup, error_info const & ei ) noexcept
			{
				if( ei.exception_caught() )
					if( boost::exception const * be = dynamic_cast<boost::exception const *>(ei.exception()) )
						if( auto * x = boost::get_error_info<boost::error_info<Tag, T>>(*be) )
						{
							auto & sl = std::get<tuple_type_index<slot<boost::error_info<Tag, T>>,SlotsTuple>::value>(tup);
							return &sl.put(ei.error().value(), boost::error_info<Tag, T>(*x));
						}
				return 0;
			}
		};
	}

} }

#endif
