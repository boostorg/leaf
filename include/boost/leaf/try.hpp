#ifndef BOOST_LEAF_87F274C4D4BA11E89928D55AC82C3C47
#define BOOST_LEAF_87F274C4D4BA11E89928D55AC82C3C47

// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/static_store.hpp>
#include <boost/leaf/detail/print_exception_info.hpp>

namespace boost { namespace leaf {

	inline error_id get_error( std::exception const & ex ) noexcept
	{
		if( auto err = dynamic_cast<error_id const *>(&ex) )
			return *err;
		else
			return next_error();
	}

	namespace leaf_detail
	{
		template <class TryBlock, class R = decltype(std::declval<TryBlock>()()), bool ReturnsResultType = is_result_type<R>::value>
		struct call_try_block;

		template <class TryBlock, class R>
		struct call_try_block<TryBlock, R, false>
		{
			template <class StaticStore, class... Handler>
			static R call( StaticStore & ss, TryBlock && try_block, Handler && ... )
			{
				ss.set_reset(true);
				return std::forward<TryBlock>(try_block)();
			}
		};

		template <class TryBlock, class R>
		struct call_try_block<TryBlock, R, true>
		{
			template <class StaticStore, class... Handler>
			static R call( StaticStore & ss, TryBlock && try_block, Handler && ... handler )
			{
				if( auto r = std::forward<TryBlock>(try_block)() )
				{
					ss.set_reset(true);
					return r;
				}
				else
				{
					auto rr = ss.handle_error(error_info(r.error()), handler_wrapper<R,Handler>(std::forward<Handler>(handler))..., [&r] { return r; } );
					if( rr )
						ss.set_reset(true);
					return rr;
				}
			}
		};
	}

	template <class TryBlock, class... Handler>
	decltype(std::declval<TryBlock>()()) try_( TryBlock && try_block, Handler && ... handler )
	{
		using namespace leaf_detail;
		typename deduce_static_store<typename handler_args_set<Handler...>::type>::type ss;
		auto throw_ = [ ]() -> typename function_traits<TryBlock>::return_type { throw; };
		try
		{
			return call_try_block<TryBlock>::call(ss, std::forward<TryBlock>(try_block), std::forward<Handler>(handler)...);
		}
		catch( captured_exception & cap )
		{
			try
			{
				cap.unload_and_rethrow_original_exception();
				assert(0);
			}
			catch( std::system_error const & ex )
			{
				return ss.handle_error(error_info(ex.code(), &ex, &cap, &print_exception_info), std::forward<Handler>(handler)..., std::move(throw_));
			}
			catch( std::exception const & ex )
			{
				if( error_id const * id = dynamic_cast<error_id const *>(&ex) )
					return ss.handle_error(error_info(*id, &ex, &cap, &print_exception_info), std::forward<Handler>(handler)..., std::move(throw_));
				else
					return ss.handle_error(error_info(make_error_id(next_id()), &ex, &cap, &print_exception_info), std::forward<Handler>(handler)..., std::move(throw_));
			}
			catch(...)
			{
				return ss.handle_error(error_info(make_error_id(next_id()), 0, &cap, &print_exception_info), std::forward<Handler>(handler)..., std::move(throw_));
			}
		}
		catch( std::system_error const & ex )
		{
			return ss.handle_error(error_info(ex.code(), &ex, 0, &print_exception_info), std::forward<Handler>(handler)..., std::move(throw_));
		}
		catch( std::exception const & ex )
		{
			if( error_id const * id = dynamic_cast<error_id const *>(&ex) )
				return ss.handle_error(error_info(*id, &ex, 0, &print_exception_info), std::forward<Handler>(handler)..., std::move(throw_));
			else
				return ss.handle_error(error_info(make_error_id(next_id()), &ex, 0, &print_exception_info), std::forward<Handler>(handler)..., std::move(throw_));
		}
		catch(...)
		{
			return ss.handle_error(error_info(make_error_id(next_id()), 0, 0, &print_exception_info), std::forward<Handler>(handler)..., std::move(throw_));
		}
	}

} }

#endif
