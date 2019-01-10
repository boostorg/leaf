#ifndef BOOST_LEAF_87F274C4D4BA11E89928D55AC82C3C47
#define BOOST_LEAF_87F274C4D4BA11E89928D55AC82C3C47

//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/throw.hpp>
#include <boost/leaf/detail/static_store.hpp>
#include <boost/leaf/detail/captured_exception.hpp>
#include <boost/leaf/detail/demangle.hpp>

namespace boost { namespace leaf {

	inline error get_error( std::exception const & ex ) noexcept
	{
		if( auto e = dynamic_cast<error const *>(&ex) )
			return *e;
		else
			return next_error_value();
	}

	template <class TryBlock, class... Handlers>
	typename leaf_detail::function_traits<TryBlock>::return_type try_( TryBlock && try_block, Handlers && ... handlers )
	{
		using namespace leaf_detail;
		typename deduce_static_store<typename handlers_args_set<Handlers...>::type>::type ss;
		ss.set_reset(true);
		try
		{
			return std::forward<TryBlock>(try_block)();
		}
		catch( leaf_detail::captured_exception & cap )
		{
			try
			{
				cap.unload_and_rethrow_original_exception();
			}
			catch( std::exception const & ex )
			{
				return ss.handle_error(get_error(ex), &ex, std::forward<Handlers>(handlers)..., [ ]() -> typename leaf_detail::function_traits<TryBlock>::return_type { throw; });
			}
		}
		catch( std::exception const & ex )
		{
			return ss.handle_error(get_error(ex), &ex, std::forward<Handlers>(handlers)..., [ ]() -> typename leaf_detail::function_traits<TryBlock>::return_type { throw; });
		}
		catch( ... )
		{
			return ss.handle_error(next_error_value(), 0, std::forward<Handlers>(handlers)..., [ ]() -> typename leaf_detail::function_traits<TryBlock>::return_type { throw; });
		}
	}

	namespace leaf_detail
	{
		inline void diagnostic_output_std_exception( std::ostream & os, std::exception const & ex )
		{
			os <<
				"Exception dynamic type: " << leaf_detail::demangle(typeid(ex).name()) << std::endl <<
				"std::exception::what(): " << ex.what() << std::endl;
		}

		inline void diagnostic_output_current_exception_no_capture( std::ostream & os )
		{
			try
			{
				throw;
			}
			catch( std::exception const & ex )
			{
				diagnostic_output_std_exception(os,ex);
			}
			catch( ... )
			{
				os << "Unknown exception type (not a std::exception)" << std::endl;
			}
			try
			{
				throw;
			}
			catch( error const & e )
			{
				e.diagnostic_output(os);
			}
			catch( ... )
			{
				global_diagnostic_output(os);
			}
		}

		inline void diagnostic_output_current_exception_( std::ostream & os )
		{
			try
			{
				try
				{
					throw;
				}
				catch( leaf_detail::captured_exception const & cap )
				{
					cap.diagnostic_output( os, &leaf_detail::diagnostic_output_current_exception_no_capture );
				}
			}
			catch(...)
			{
				leaf_detail::diagnostic_output_current_exception_no_capture(os);
			}
		}
	} //namespace leaf_detail

	inline void diagnostic_output_current_exception( std::ostream & os )
	{
		os << "Current Exception Diagnostic Information:" << std::endl;
		leaf_detail::diagnostic_output_current_exception_(os);
	}

	inline void diagnostic_output( std::ostream & os, std::exception const & ex )
	{
		if( leaf_detail::captured_exception const * cap = dynamic_cast<leaf_detail::captured_exception const *>(&ex) )
			cap->diagnostic_output( os, &leaf_detail::diagnostic_output_current_exception_no_capture );
		else
		{
			leaf_detail::diagnostic_output_std_exception(os,ex);
			get_error(ex).diagnostic_output(os);
		}
	}

	inline void diagnostic_output( std::ostream & os, std::exception_ptr const & ep )
	{
		if( ep )
		{
			os << "std::exception_ptr Diagnostic Information:" << std::endl;
			try
			{
				std::rethrow_exception(ep);
			}
			catch(...)
			{
				leaf_detail::diagnostic_output_current_exception_(os);
			}
		}
		else
			os << "Empty" << std::endl;
	}

} }

#endif
