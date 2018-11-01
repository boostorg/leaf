//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_87F274C4D4BA11E89928D55AC82C3C47
#define UUID_87F274C4D4BA11E89928D55AC82C3C47

#include <boost/leaf/expect.hpp>
#include <boost/leaf/detail/throw_exception.hpp>

namespace boost { namespace leaf {

	inline error get_error( std::exception const & ex ) noexcept
	{
		if( auto e = dynamic_cast<error const *>(&ex) )
			return *e;
		else
			return peek_next_error();
	}

	template <class P, class... E>
	decltype(P::value) const * peek( expect<E...> const & exp, std::exception const & ex ) noexcept
	{
		return peek<P>(exp,get_error(ex));
	}

	template <class... M, class... E>
	void handle_exception( expect<E...> & exp, std::exception const & ex, M && ... m )
	{
		if( handle_error(exp,get_error(ex),m...) )
			(void) error();
		else
			throw;
	}

	template <class... E>
	void diagnostic_output( std::ostream & os, expect<E...> const & exp, std::exception const & ex )
	{
		diagnostic_output(os,exp,get_error(ex));
	}

} }

#endif
