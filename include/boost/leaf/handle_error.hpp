#ifndef BOOST_LEAF_539464A021D411E9BC8A79361E29EE6E
#define BOOST_LEAF_539464A021D411E9BC8A79361E29EE6E

// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/handle.hpp>

namespace boost { namespace leaf {

	template <class TryBlock, class... Handler>
	typename std::decay<decltype(std::declval<TryBlock>()().value())>::type try_handle_all( TryBlock && try_block, Handler && ... handler ) noexcept
	{
		using namespace leaf_detail;
		static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The try_block passed to try_handle_all must be registered with leaf::is_result_type");
		context_type_from_handlers<Handler...> ctx;
		context_activator active_context(ctx, false);
		if( auto r = std::forward<TryBlock>(try_block)() )
			return r.value();
		else
			return leaf_detail::handle_error_(ctx.tup(), error_info(r.error()), std::forward<Handler>(handler)...);
	}

	template <class TryBlock, class... Handler>
	typename std::decay<decltype(std::declval<TryBlock>()())>::type try_handle_some( TryBlock && try_block, Handler && ... handler ) noexcept
	{
		using namespace leaf_detail;
		static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The try_block passed to try_handle_some must be registered with leaf::is_result_type");
		context_type_from_handlers<Handler...> ctx;
		context_activator active_context(ctx, false);
		if( auto r = std::forward<TryBlock>(try_block)() )
			return r;
		else
		{
			auto rr = leaf_detail::handle_error_(ctx.tup(), error_info(r.error()), std::forward<Handler>(handler)..., [&r] { return r; });
			if( !rr )
				active_context.set_propagate_errors(true);
			return rr;
		}
	}

	////////////////////////////////////////

	template <class TryBlock, class RemoteHandler>
	typename std::decay<decltype(std::declval<TryBlock>()().value())>::type remote_try_handle_all( TryBlock && try_block, RemoteHandler && handler ) noexcept
	{
		using namespace leaf_detail;
		static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The try_block passed to remote_try_handle_all must be registered with leaf::is_result_type.");
		context_type_from_remote_handler<RemoteHandler> ctx;
		context_activator active_context(ctx, false);
		if( auto r = std::forward<TryBlock>(try_block)() )
			return r.value();
		else
			return std::forward<RemoteHandler>(handler)(error(&ctx, r.error())).get();
	}

	template <class TryBlock, class RemoteHandler>
	typename std::decay<decltype(std::declval<TryBlock>()())>::type remote_try_handle_some( TryBlock && try_block, RemoteHandler && handler ) noexcept
	{
		using namespace leaf_detail;
		static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The try_block passed to remote_try_handle_some must be registered with leaf::is_result_type.");
		context_type_from_remote_handler<RemoteHandler> ctx;
		context_activator active_context(ctx, false);
		if( auto r = std::forward<TryBlock>(try_block)() )
			return r;
		else
		{
			auto rr = std::forward<RemoteHandler>(handler)(error(&ctx, r.error())).get();
			if( !rr )
				active_context.set_propagate_errors(true);
			return rr;
		}
	}

	namespace leaf_detail
	{
		template <class R, class... Handler>
		struct remote_error_dispatch_impl
		{
			using result_type = handler_result<Handler...>;

			static result_type handle( error const & err, Handler && ... handler ) noexcept
			{
				using Ctx = context_type_from_handlers<Handler...>;
				return { handle_error_(reinterpret_cast<Ctx const *>(err.ctx_)->tup(), error_info(err), std::forward<Handler>(handler)...) };
			}
		};

		template <class... Handler>
		struct remote_error_dispatch_impl<void, Handler...>
		{
			using result_type = handler_result_void<Handler...>;

			static result_type handle( error const & err, Handler && ... handler ) noexcept
			{
				using Ctx = context_type_from_handlers<Handler...>;
				handle_error_(reinterpret_cast<Ctx const *>(err.ctx_)->tup(), error_info(err), std::forward<Handler>(handler)...);
				return { };
			}
		};

		template <class... Handler>
		using remote_error_dispatch = remote_error_dispatch_impl<handler_pack_return<Handler...>, Handler...>;
	}

	template <class... Handler>
	typename leaf_detail::remote_error_dispatch<Handler...>::result_type remote_handle_all( error const & err, Handler && ... handler ) noexcept
	{
		return leaf_detail::remote_error_dispatch<Handler...>::handle(err, std::forward<Handler>(handler)... );
	}

	template <class... Handler>
	typename leaf_detail::remote_error_dispatch<Handler...>::result_type remote_handle_some( error const & err, Handler && ... handler ) noexcept
	{
		return leaf_detail::remote_error_dispatch<Handler...>::handle(err, std::forward<Handler>(handler)...,
			[&err]{ return err.has_code() ? err.get_code() : err.get_error(); } );
	}

} }

#endif
