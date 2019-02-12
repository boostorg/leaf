#ifndef BOOST_LEAF_539464A021D411E9BC8A79361E29EE6E
#define BOOST_LEAF_539464A021D411E9BC8A79361E29EE6E

// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/handle.hpp>

namespace boost { namespace leaf {

	template <class... E, class R, class... H>
	typename std::decay<decltype(std::declval<R>().value())>::type handle_all( context<E...> const & ctx, R const & r, H && ... h ) noexcept
	{
		using namespace leaf_detail;
		using Ret = typename std::decay<decltype(std::declval<R>().value())>::type;
		static_assert(is_result_type<R>::value, "The R type used with handle_all must be registered with leaf::is_result_type");
		assert(!r);
		return handle_error_<Ret>(ctx.tup(), error_info(ctx, r.error()), std::forward<H>(h)...);
	}

	template <class... E, class R, class... H>
	R handle_some( context<E...> const & ctx, R const & r, H && ... h ) noexcept
	{
		using namespace leaf_detail;
		static_assert(is_result_type<R>::value, "The R type used with handle_some must be registered with leaf::is_result_type");
		assert(!r);
		return handle_error_<R>(ctx.tup(), error_info(ctx, r.error()), std::forward<H>(h)...,
			[&r]{ return r; });
	}

	template <class TryBlock, class... H>
	typename std::decay<decltype(std::declval<TryBlock>()().value())>::type try_handle_all( TryBlock && try_block, H && ... h ) noexcept
	{
		using namespace leaf_detail;
		static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The try_block passed to try_handle_all must be registered with leaf::is_result_type");
		context_type_from_handlers<H...> ctx;
		context_activator active_context(ctx, context_activator::on_deactivation::propagate_if_uncaught_exception);
		if( auto r = std::forward<TryBlock>(try_block)() )
			return r.value();
		else
			return handle_all(ctx, r, std::forward<H>(h)...);
	}

	template <class TryBlock, class... H>
	typename std::decay<decltype(std::declval<TryBlock>()())>::type try_handle_some( TryBlock && try_block, H && ... h ) noexcept
	{
		using namespace leaf_detail;
		static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The try_block passed to try_handle_some must be registered with leaf::is_result_type");
		context_type_from_handlers<H...> ctx;
		context_activator active_context(ctx, context_activator::on_deactivation::propagate_if_uncaught_exception);
		if( auto r = std::forward<TryBlock>(try_block)() )
			return r;
		else
		{
			auto rr = handle_some(ctx, r, std::forward<H>(h)...);
			if( !rr )
				active_context.set_on_deactivate(context_activator::on_deactivation::propagate);
			return rr;
		}
	}

	////////////////////////////////////////

	template <class TryBlock, class RemoteH>
	typename std::decay<decltype(std::declval<TryBlock>()().value())>::type remote_try_handle_all( TryBlock && try_block, RemoteH && h ) noexcept
	{
		using namespace leaf_detail;
		static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The try_block passed to remote_try_handle_all must be registered with leaf::is_result_type.");
		context_type_from_remote_handler<RemoteH> ctx;
		context_activator active_context(ctx, context_activator::on_deactivation::propagate_if_uncaught_exception);
		if( auto r = std::forward<TryBlock>(try_block)() )
			return r.value();
		else
			return std::forward<RemoteH>(h)(error_info(ctx, r.error())).get();
	}

	template <class TryBlock, class RemoteH>
	typename std::decay<decltype(std::declval<TryBlock>()())>::type remote_try_handle_some( TryBlock && try_block, RemoteH && h ) noexcept
	{
		using namespace leaf_detail;
		static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The try_block passed to remote_try_handle_some must be registered with leaf::is_result_type.");
		context_type_from_remote_handler<RemoteH> ctx;
		context_activator active_context(ctx, context_activator::on_deactivation::propagate_if_uncaught_exception);
		if( auto r = std::forward<TryBlock>(try_block)() )
			return r;
		else
		{
			auto rr = std::forward<RemoteH>(h)(error_info(ctx, r.error())).get();
			if( !rr )
				active_context.set_on_deactivate(context_activator::on_deactivation::propagate);
			return rr;
		}
	}

	namespace leaf_detail
	{
		template <class R, class... H>
		struct remote_error_dispatch_impl
		{
			using result_type = handler_result<H...>;

			static result_type handle( error_info const & err, H && ... h ) noexcept
			{
				using Ctx = context_type_from_handlers<H...>;
				return { handle_error_<R>(static_cast<Ctx const &>(err.ctx_).tup(), err, std::forward<H>(h)...) };
			}
		};

		template <class... H>
		struct remote_error_dispatch_impl<void, H...>
		{
			using result_type = handler_result_void<H...>;

			static result_type handle( error_info const & err, H && ... h ) noexcept
			{
				using Ctx = context_type_from_handlers<H...>;
				handle_error_<void>(static_cast<Ctx const &>(err.ctx_).tup(), err, std::forward<H>(h)...);
				return { };
			}
		};

		template <class... H>
		using remote_error_dispatch = remote_error_dispatch_impl<handler_pack_return<H...>, H...>;
	}

	template <class... H>
	typename leaf_detail::remote_error_dispatch<H...>::result_type remote_handle_all( error_info const & err, H && ... h ) noexcept
	{
		return leaf_detail::remote_error_dispatch<H...>::handle(err, std::forward<H>(h)... );
	}

	template <class... H>
	typename leaf_detail::remote_error_dispatch<H...>::result_type remote_handle_some( error_info const & err, H && ... h ) noexcept
	{
		return leaf_detail::remote_error_dispatch<H...>::handle(err, std::forward<H>(h)...,
			[&err]{ return err.error_code(); } );
	}

} }

#endif
