#ifndef BOOST_LEAF_87F274C4D4BA11E89928D55AC82C3C47
#define BOOST_LEAF_87F274C4D4BA11E89928D55AC82C3C47

// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

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

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class TryBlock, class... Handler, class StaticStore>
		decltype(std::declval<TryBlock>()()) call_try_block( result_tag<decltype(std::declval<TryBlock>()()), false>, StaticStore & ss, TryBlock && try_block, Handler && ... )
		{
			ss.set_reset(true);
			return std::forward<TryBlock>(try_block)();
		}

		template <class TryBlock, class... Handler, class StaticStore>
		decltype(std::declval<TryBlock>()()) call_try_block( result_tag<decltype(std::declval<TryBlock>()()), true>, StaticStore & ss, TryBlock && try_block, Handler && ... handler )
		{
			if( auto r = std::forward<TryBlock>(try_block)() )
			{
				ss.set_reset(true);
				return r;
			}
			else
			{
				auto rr = ss.handle_error_(error_info(r.error()), handler_wrapper<decltype(std::declval<TryBlock>()()), Handler>(std::forward<Handler>(handler))..., [&r] { return r; } );
				if( rr )
					ss.set_reset(true);
				return rr;
			}
		}
	}

	template <class TryBlock, class... Handler>
	decltype(std::declval<TryBlock>()()) try_( TryBlock && try_block, Handler && ... handler )
	{
		using namespace leaf_detail;
		deduce_static_store<handler_args_set<Handler...>> ss;
		auto throw_ = [ ]() -> typename function_traits<TryBlock>::return_type { throw; };
		try
		{
			return call_try_block( result_tag<decltype(std::declval<TryBlock>()())>(), ss, std::forward<TryBlock>(try_block), std::forward<Handler>(handler)...);
		}
		catch( capturing_exception & cap )
		{
			try
			{
				cap.unload_and_rethrow_original_exception();
				assert(0);
			}
			catch( std::system_error const & ex )
			{
				return ss.handle_error_(error_info(ex.code(), exception_info(&ex, &cap, &print_exception_info)), std::forward<Handler>(handler)..., std::move(throw_));
			}
			catch( std::exception const & ex )
			{
				if( error_id const * id = dynamic_cast<error_id const *>(&ex) )
					return ss.handle_error_(error_info(*id, exception_info(&ex, &cap, &print_exception_info)), std::forward<Handler>(handler)..., std::move(throw_));
				else
					return ss.handle_error_(error_info(make_error_id(next_id()), exception_info(&ex, &cap, &print_exception_info)), std::forward<Handler>(handler)..., std::move(throw_));
			}
			catch(...)
			{
				return ss.handle_error_(error_info(make_error_id(next_id()), exception_info(0, &cap, &print_exception_info)), std::forward<Handler>(handler)..., std::move(throw_));
			}
		}
		catch( std::system_error const & ex )
		{
			return ss.handle_error_(error_info(ex.code(), exception_info(&ex, 0, &print_exception_info)), std::forward<Handler>(handler)..., std::move(throw_));
		}
		catch( std::exception const & ex )
		{
			if( error_id const * id = dynamic_cast<error_id const *>(&ex) )
				return ss.handle_error_(error_info(*id, exception_info(&ex, 0, &print_exception_info)), std::forward<Handler>(handler)..., std::move(throw_));
			else
				return ss.handle_error_(error_info(make_error_id(next_id()), exception_info(&ex, 0, &print_exception_info)), std::forward<Handler>(handler)..., std::move(throw_));
		}
		catch(...)
		{
			return ss.handle_error_(error_info(make_error_id(next_id()), exception_info(0, 0, &print_exception_info)), std::forward<Handler>(handler)..., std::move(throw_));
		}
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class TryBlock, class Handler, class StaticStore>
		decltype(std::declval<TryBlock>()()) error_call_try_block( result_tag<decltype(std::declval<TryBlock>()()), false>, StaticStore & ss, TryBlock && try_block, Handler && )
		{
			ss.set_reset(true);
			return std::forward<TryBlock>(try_block)();
		}

		template <class TryBlock, class Handler, class StaticStore>
		decltype(std::declval<TryBlock>()()) error_call_try_block( result_tag<decltype(std::declval<TryBlock>()()), true>, StaticStore & ss, TryBlock && try_block, Handler && handler )
		{
			if( auto r = std::forward<TryBlock>(try_block)() )
			{
				ss.set_reset(true);
				return r;
			}
			else
			{
				auto rr = std::forward<Handler>(handler)(error(&ss, r.error())).get();
				if( rr )
					ss.set_reset(true);
				return rr;
			}
		}
	}

	struct error_in_capture_try_: error_info
	{
		void const * const ss_;

		error_in_capture_try_( void const * ss, error_id const & id, leaf_detail::exception_info const & ex ) noexcept:
			error_info(id, ex),
			ss_(ss)
		{
			assert(ss_!=0);
		}

		error_in_capture_try_( void const * ss, std::error_code const & ec, leaf_detail::exception_info const & ex ) noexcept:
			error_info(ec, ex),
			ss_(ss)
		{
			assert(ss_!=0);
		}
	};

	template <class TryBlock, class Handler>
	decltype(std::declval<TryBlock>()()) capture_try_( TryBlock && try_block, Handler && handler )
	{
		using namespace leaf_detail;
		deduce_static_store<handler_args_list<typename function_traits<Handler>::return_type>> ss;
		try
		{
			return error_call_try_block( result_tag<decltype(std::declval<TryBlock>()())>(), ss, std::forward<TryBlock>(try_block), std::forward<Handler>(handler));
		}
		catch( capturing_exception & cap )
		{
			try
			{
				cap.unload_and_rethrow_original_exception();
				assert(0);
			}
			catch( std::system_error const & ex )
			{
				return std::forward<Handler>(handler)(error_in_capture_try_(&ss, ex.code(), exception_info(&ex, &cap, &print_exception_info))).get();
			}
			catch( std::exception const & ex )
			{
				if( error_id const * id = dynamic_cast<error_id const *>(&ex) )
					return std::forward<Handler>(handler)(error_in_capture_try_(&ss, *id, exception_info(&ex, &cap, &print_exception_info))).get();
				else
					return std::forward<Handler>(handler)(error_in_capture_try_(&ss, make_error_id(next_id()), exception_info(&ex, &cap, &print_exception_info))).get();
			}
			catch(...)
			{
				return std::forward<Handler>(handler)(error_in_capture_try_(&ss, make_error_id(next_id()), exception_info(0, &cap, &print_exception_info))).get();
			}
		}
		catch( std::system_error const & ex )
		{
			return std::forward<Handler>(handler)(error_in_capture_try_(&ss, ex.code(), exception_info(&ex, 0, &print_exception_info))).get();
		}
		catch( std::exception const & ex )
		{
			if( error_id const * id = dynamic_cast<error_id const *>(&ex) )
				return std::forward<Handler>(handler)(error_in_capture_try_(&ss, *id, exception_info(&ex, 0, &print_exception_info))).get();
			else
				return std::forward<Handler>(handler)(error_in_capture_try_(&ss, make_error_id(next_id()), exception_info(&ex, 0, &print_exception_info))).get();
		}
		catch(...)
		{
			return std::forward<Handler>(handler)(error_in_capture_try_(&ss, make_error_id(next_id()), exception_info(0, 0, &print_exception_info))).get();
		}
	}

	template <class... Handler>
	typename leaf_detail::handle_error_dispatch<Handler...>::result_type handle_error( error_in_capture_try_ const & err, Handler && ... handler )
	{
		using namespace leaf_detail;
		return handle_error_dispatch<Handler...>::handle_try_(err, std::forward<Handler>(handler)... );
	}

} }

#endif
