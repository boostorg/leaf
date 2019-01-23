#ifndef BOOST_LEAF_73685B76115611E9950D61678B7F4AFC
#define BOOST_LEAF_73685B76115611E9950D61678B7F4AFC

// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/static_store.hpp>
#include <boost/leaf/error.hpp>

namespace boost { namespace leaf {

	namespace leaf_detail {
		inline int handle_get_err_id( error_id const & err ) noexcept
		{
			return err.value();
		}
		inline int handle_get_err_id( std::error_code const & ec ) noexcept
		{
			return is_error_id(ec) ? ec.value() : 0;
		}
	}

	template <class TryBlock, class... Handler>
	typename std::remove_reference<decltype(std::declval<TryBlock>()().value())>::type handle_all( TryBlock && try_block, Handler && ... handler )
	{
		using namespace leaf_detail;
		typename deduce_static_store<typename handler_args_set<Handler...>::type>::type ss;
		ss.set_reset(true);
		if( auto r = std::forward<TryBlock>(try_block)() )
			return r.value();
		else
			return ss.handle_error(error_info(handle_get_err_id(r.error())), &r, std::forward<Handler>(handler)...);
	}

	namespace leaf_detail
	{
		template <class TryReturn, class F, class HandlerReturn=typename function_traits<F>::return_type, class=typename function_traits<F>::mp_args>
		struct handler_wrapper;

		template <class TryReturn, class F, class HandlerReturn, template<class...> class L, class... A>
		struct handler_wrapper<TryReturn, F, HandlerReturn, L<A...>>
		{
			F f_;
			explicit handler_wrapper( F && f ) noexcept:
				f_(std::forward<F>(f))
			{
			}
			TryReturn operator()( A... a ) const
			{
				return f_(std::forward<A>(a)...);
			}
		};

		template <class TryReturn, class F, template<class...> class L, class... A>
		struct handler_wrapper<TryReturn, F, void, L<A...>>
		{
			F f_;
			explicit handler_wrapper( F && f ) noexcept:
				f_(std::forward<F>(f))
			{
			}
			TryReturn operator()( A... a ) const
			{
				f_(std::forward<A>(a)...);
				return { };
			}
		};
	}

	template <class TryBlock, class... Handler>
	decltype(std::declval<TryBlock>()()) handle_some( TryBlock && try_block, Handler && ... handler )
	{
		using namespace leaf_detail;
		using R = decltype(std::declval<TryBlock>()());
		typename deduce_static_store<typename handler_args_set<Handler...>::type>::type ss;
		if( auto r = std::forward<TryBlock>(try_block)() )
		{
			ss.set_reset(true);
			return r;
		}
		else
		{
			auto rr = ss.handle_error(error_info(handle_get_err_id(r.error())), &r, handler_wrapper<R,Handler>(std::forward<Handler>(handler))..., [&r] { return r; } );
			if( rr )
				ss.set_reset(true);
			return rr;
		}
	}

} }

#endif
