//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_BC24FB98B2DE11E884419CF5AD35F1A2
#define UUID_BC24FB98B2DE11E884419CF5AD35F1A2

#include <boost/leaf/error_capture.hpp>
#include <boost/leaf/expect.hpp>
#include <typeinfo>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		class captured_exception:
			public std::exception,
			error_capture
		{
			std::exception_ptr ex_;
			bool has_error_;

		public:

			captured_exception( std::exception_ptr && ex, error_capture && cap, bool has_error ) noexcept:
				error_capture(std::move(cap)),
				ex_(std::move(ex)),
				has_error_(has_error)
			{
				assert(ex_);
			}

			[[noreturn]]
			void rethrow_original_exception()
			{
				if( !has_error_ )
				{
					set_error(next_error_value());
					has_error_ = true;
				}
				unload();
				std::rethrow_exception(ex_);
			}

			friend void diagnostic_output( std::ostream & os, captured_exception const & ce )
			{
				diagnostic_output(os,static_cast<error_capture const &>(ce));
			}
		};

		////////////////////////////////////////

		template <class F, class... E>
		class exception_trap
		{
			F f_;

		public:

			constexpr explicit exception_trap( F && f ) noexcept:
				f_(std::move(f))
			{
			}

			template <class... A>
			decltype(std::declval<F>()(std::declval<A>()...)) operator()( A && ... a )
			{
				expect<E...> exp;
				try
				{
					return f_(std::forward<A>(a)...);
				}
				catch( error const & e )
				{
					throw captured_exception(std::current_exception(),capture(exp,e),true);
				}
				catch(...)
				{
					throw captured_exception(std::current_exception(),capture(exp,error()),false);
				}
			}
		};
	} //leaf_detail

	template <class... E, class F>
	constexpr leaf_detail::exception_trap<F,E...> capture_exception( F && f ) noexcept
	{
		return leaf_detail::exception_trap<F,E...>(std::move(f));
	}

	template <class Future>
	decltype(std::declval<Future>().get()) get( Future && f )
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
	void current_exception_diagnostic_output( std::ostream & os, expect<E...> const & exp )
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
			diagnostic_output(os,e);
		}
		catch( error const & e )
		{
			diagnostic_output(os,exp,e);
		}
		catch( ... )
		{
			diagnostic_output(os,exp);
		}
	}

} }

#endif
