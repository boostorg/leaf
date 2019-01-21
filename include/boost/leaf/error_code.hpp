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
	}

	inline std::error_code get_error_code( error_id id ) noexcept
	{
		return id?
			std::error_code(id.id_, leaf_detail::get_error_category()) :
			std::error_code();
	}

	inline error_id get_error_id( std::error_code ec ) noexcept
	{
		if( ec )
			if( &ec.category()==&leaf_detail::get_error_category() )
			{
				assert(ec);
				return error_id{ec.value()};
			}
			else
				return leaf::next_error();
		else
			return success();
	}

} }

#endif
