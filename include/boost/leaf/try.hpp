#ifndef BOOST_LEAF_87F274C4D4BA11E89928D55AC82C3C47
#define BOOST_LEAF_87F274C4D4BA11E89928D55AC82C3C47

// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/throw.hpp>
#include <boost/leaf/detail/static_store.hpp>
#include <boost/leaf/detail/print_exception_info.hpp>

namespace boost { namespace leaf {

	inline error_id get_error_id( std::exception const & ex ) noexcept
	{
		if( auto id = dynamic_cast<error_id const *>(&ex) )
			return *id;
		else
			return next_error();
	}

	template <class TryBlock, class... Handler>
	decltype(std::declval<TryBlock>()()) try_( TryBlock && try_block, Handler && ... handler )
	{
		using namespace leaf_detail;
		typename deduce_static_store<typename handler_args_set<Handler...>::type>::type ss;
		ss.set_reset(true);
		try
		{
			return std::forward<TryBlock>(try_block)();
		}
		catch( captured_exception & cap )
		{
			try
			{
				cap.unload_and_rethrow_original_exception();
				assert(0);
				throw;
			}
			catch( std::exception const & ex )
			{
				return ss.handle_error(error_info(get_error_id(ex),&ex,&cap,&print_exception_info), (void *)0, std::forward<Handler>(handler)..., [ ]() -> typename function_traits<TryBlock>::return_type { throw; });
			}
			catch( ... )
			{
				return ss.handle_error(error_info(next_error(),0,&cap,&print_exception_info), (void *)0, std::forward<Handler>(handler)..., [ ]() -> typename function_traits<TryBlock>::return_type { throw; });
			}
		}
		catch( std::exception const & ex )
		{
			return ss.handle_error(error_info(get_error_id(ex),&ex,0,&print_exception_info), (void *)0, std::forward<Handler>(handler)..., [ ]() -> typename function_traits<TryBlock>::return_type { throw; });
		}
		catch( ...  )
		{
			return ss.handle_error(error_info(next_error(),0,0,&print_exception_info), (void *)0, std::forward<Handler>(handler)..., [ ]() -> typename function_traits<TryBlock>::return_type { throw; });
		}
	}

} }

#endif
