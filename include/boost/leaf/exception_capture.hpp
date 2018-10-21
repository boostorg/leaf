//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_BC24FB98B2DE11E884419CF5AD35F1A2
#define UUID_BC24FB98B2DE11E884419CF5AD35F1A2

#include <boost/leaf/error_capture.hpp>
#include <boost/leaf/expect.hpp>
#include <typeinfo>

namespace
boost
	{
	namespace
	leaf
		{
		namespace
		leaf_detail
			{
			class
			captured_exception:
				public std::exception
				{
				std::exception_ptr ex_;
				error_capture cap_;
				public:
				captured_exception( std::exception_ptr && ex, error_capture && cap ) noexcept:
					ex_(std::move(ex)),
					cap_(std::move(cap))
					{
					assert(ex_);
					}
				explicit
				captured_exception( std::exception_ptr && ex ) noexcept:
					ex_(std::move(ex))
					{
					assert(ex_);
					}
				[[noreturn]]
				void
				rethrow_original_exception()
					{
					if( cap_ )
						set_current_error(cap_.propagate());
					std::rethrow_exception(ex_);
					}
				friend
				void
				diagnostic_print( std::ostream & os, captured_exception const & ce )
					{
					diagnostic_print(os,ce.cap_);
					}
				};
			template <class F,class... E>
			class
			exception_trap
				{
				F f_;
				public:
				explicit
				exception_trap( F && f ) noexcept:
					f_(std::move(f))
					{
					}
				template <class... A>
				decltype(std::declval<F>()(std::declval<A>()...))
				operator()( A && ... a )
					{
					expect<E...> exp;
					try
						{
						return f_(std::forward<A>(a)...);
						}
					catch( error const & e )
						{
						throw captured_exception(std::current_exception(),capture(exp,e));
						}
					catch(...)
						{
						if( error const * e = current_error() )
							throw captured_exception(std::current_exception(),capture(exp,*e));
						else
							throw captured_exception(std::current_exception());
						}
					}
				};
			}
		template <class... E,class F>
		leaf_detail::exception_trap<F,E...>
		capture_exception( F && f ) noexcept
			{
			return leaf_detail::exception_trap<F,E...>(std::move(f));
			}
		template <class Future>
		decltype(std::declval<Future>().get())
		get( Future && f )
			{
			try
				{
				return std::forward<Future>(f).get();
				}
			catch( leaf_detail::captured_exception & ex )
				{
				ex.rethrow_original_exception();
				}
			}
		////////////////////////////////////////
		template <class... E>
		void
		current_exception_diagnostic_print( std::ostream & os, expect<E...> const & exp )
			{
			os << "Current Exception Diagnostic Information:" << std::endl;
			try
				{
				throw;
				}
			catch( std::exception const & ex )
				{
				os <<
					"Exception dynamic type: " << typeid(ex).name() << std::endl <<
					"std::exception::what(): " << ex.what() << std::endl;
				}
			catch( ... )
				{
				os << "Unknown exception type (not a std::exception)" << std::endl;
				}
			try
				{
				throw;
				}
			catch( leaf_detail::captured_exception const & e )
				{
				diagnostic_print(os,e);
				}
			catch( error const & e )
				{
				diagnostic_print(os,exp,e);
				}
			catch( ... )
				{
				diagnostic_print(os,exp);
				}
			}
		}
	}

#endif
