#ifndef BOOST_LEAF_B146ADCE1C7A11E9A455E6F387D1CC0E
#define BOOST_LEAF_B146ADCE1C7A11E9A455E6F387D1CC0E

// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/error.hpp>

namespace boost { namespace leaf {

	class error_category: public std::error_category
	{
		bool equivalent( int,  std::error_condition const & ) const noexcept
		{
			return false;
		}

		bool equivalent( std::error_code const &, int ) const noexcept
		{
			return false;
		}

		char const * name() const noexcept
		{
			return "LEAF error, use with leaf::handle_some or leaf::handle_all.";
		}

		std::string message( int condition ) const
		{
			return name();
		}
	};

	namespace leaf_detail
	{
		inline error_category const & get_error_category() noexcept
		{
			static error_category cat;
			return cat;
		}

		inline error_category const & get_error_category0() noexcept
		{
			static error_category cat;
			return cat;
		}
	}

	inline std::error_code error_id::to_error_code() const noexcept
	{
		return id_ ?
			std::error_code( int(id_), leaf_detail::get_error_category() ) :
			std::error_code( -1, leaf_detail::get_error_category0() );
	}

	inline bool succeeded( std::error_code const & ec )
	{
		return !ec;
	}

	inline error_id get_error_id( std::error_code const & ec ) noexcept
	{
		std::error_category const & cat =  ec.category();
		if( &cat==&leaf_detail::get_error_category() )
			return error_id(ec.value());
		else if( &cat==&leaf_detail::get_error_category0() )
		{
			assert(ec.value()==-1);
			return error_id(0);
		}
		else
			return leaf::next_error();
	}

} }

#endif
