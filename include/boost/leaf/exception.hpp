//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_87F274C4D4BA11E89928D55AC82C3C47
#define UUID_87F274C4D4BA11E89928D55AC82C3C47

#include <boost/leaf/expect.hpp>
#include <exception>

namespace
boost
	{
	namespace
	leaf
		{
		template <class P,class... E>
		decltype(P::value) const *
		peek( expect<E...> const & exp, std::exception const & e )
			{
			if( auto err = dynamic_cast<error const *>(&e) )
				return peek<P>(exp,*err);
			else
				{
				assert(current_error()!=0);
				return peek<P>(exp,*current_error());
				}
			}
		template <class... M,class... E>
		void
		handle_error( expect<E...> const & exp, std::exception const & e, M && ... m )
			{
			if( auto err = dynamic_cast<error const *>(&e) )
				handle_error(exp,*err,m...);
			else
				{
				assert(current_error()!=0);
				handle_error(exp,*current_error(),m...);
				}
			}
		template <class... E>
		void
		diagnostic_print( std::ostream & os, expect<E...> const & exp, std::exception const & e )
			{
			if( auto err = dynamic_cast<error const *>(&e) )
				diagnostic_print(os,exp,*err);
			else
				{
				assert(current_error()!=0);
				diagnostic_print(os,exp);
				}
			}
		}
	}

#endif
