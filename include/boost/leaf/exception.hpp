#ifndef BOOST_LEAF_87F274C4D4BA11E89928D55AC82C3C47
#define BOOST_LEAF_87F274C4D4BA11E89928D55AC82C3C47

//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/expect.hpp>
#include <boost/leaf/detail/throw.hpp>
#include <boost/leaf/detail/demangle.hpp>

namespace boost { namespace leaf {

	inline error get_error( std::exception const & ex ) noexcept
	{
		if( auto e = dynamic_cast<error const *>(&ex) )
			return *e;
		else
			return next_error_value();
	}

	template <class... E>
	template <class P>
	P const * expect<E...>::peek( std::exception const & ex ) const noexcept
	{
		return this->template peek<P>(get_error(ex));
	}

	template <class... E>
	template <class... F>
	void expect<E...>::handle_exception( std::exception const & ex, F && ... f ) const
	{
		if( this->handle_error(get_error(ex),f...) )
			(void) error();
		else
			throw;
	}

	inline void diagnostic_output( std::ostream & os, std::exception const & ex )
	{
		os <<
			"Exception dynamic type: " << leaf_detail::demangle(typeid(ex).name()) << std::endl <<
			"std::exception::what(): " << ex.what() << std::endl;
		get_error(ex).diagnostic_output(os);
	}

} }

#endif
