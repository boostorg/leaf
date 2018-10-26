//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_87F274C4D4BA11E89928D55AC82C3C47
#define UUID_87F274C4D4BA11E89928D55AC82C3C47

#include <boost/leaf/expect.hpp>
#include <exception>

#define LEAF_THROW(e) ::boost::leaf::throw_exception(e,LEAF_SOURCE_LOCATION)

namespace
boost
	{
	namespace
	leaf
		{
		namespace
		leaf_detail
			{
			inline void enforce_std_exception( std::exception const & ) { }
			template <class Ex>
			class
			exception:
				public Ex,
				public error
				{
				public:
				exception( Ex && ex, error && e ) noexcept:
					Ex(std::move(ex)),
					error(std::move(e))
					{
					enforce_std_exception(*this);
					}
				};
			}
		template <class... E,class Ex>
		[[noreturn]]
		void
		throw_exception( Ex && ex, E && ... e )
			{
			throw leaf_detail::exception<Ex>(std::move(ex),error(std::move(e)...));
			}
		template <class... E,class Ex>
		[[noreturn]]
		void
		throw_exception( Ex && ex, error const & err, E && ... e )
			{
			throw leaf_detail::exception<Ex>(std::move(ex),err.propagate(std::move(e)...));
			}
		////////////////////////////////////////
		template <class P,class... E>
		decltype(P::value) const *
		peek( expect<E...> const & exp, std::exception const & e ) noexcept
			{
			if( auto err = dynamic_cast<error const *>(&e) )
				return peek<P>(exp,*err);
			else
				return peek<P>(exp,error::peek_next_error());
			}
		template <class... M,class... E>
		void
		handle_exception( expect<E...> & exp, std::exception const & e, M && ... m )
			{
			if( auto err = dynamic_cast<error const *>(&e) )
				{
				if( handle_error(exp,*err,m...) )
					return;
				}
			else
				{
				if( handle_error(exp,error::peek_next_error(),m...) )
					return;
				}
			throw;
			}
		template <class... E>
		void
		diagnostic_print( std::ostream & os, expect<E...> const & exp, std::exception const & e )
			{
			if( auto err = dynamic_cast<error const *>(&e) )
				diagnostic_print(os,exp,*err);
			else
				diagnostic_print(os,exp,error::peek_next_error());
			}
		}
	}

#endif
