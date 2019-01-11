#ifndef BOOST_LEAF_73685B76115611E9950D61678B7F4AFC
#define BOOST_LEAF_73685B76115611E9950D61678B7F4AFC

//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/result.hpp>
#include <boost/leaf/detail/static_store.hpp>

namespace boost { namespace leaf {

	template <class TryBlock, class... Handlers>
	typename std::remove_reference<decltype(std::declval<typename leaf_detail::function_traits<TryBlock>::return_type>().value())>::type handle_all( TryBlock && try_block, Handlers && ... handlers ) noexcept
	{
		using namespace leaf_detail;
		typename deduce_static_store<typename handlers_args_set<Handlers...>::type>::type ss;
		ss.set_reset(true);
		if( auto r = std::forward<TryBlock>(try_block)() )
			return *r;
		else
		{
			error const e = r.error();
			return ss.handle_error(error_info(e), std::forward<Handlers>(handlers)...);
		}
	}

	namespace leaf_detail
	{
		template <class R, class F, class = typename function_traits<F>::mp_args>
		struct handler_wrapper;

		template <class R, class F, template<class...> class L, class... A>
		struct handler_wrapper<R, F, L<A...>>
		{
			F f_;
			explicit handler_wrapper( F && f ) noexcept:
				f_(std::forward<F>(f))
			{
			}
			R operator()( A... a )
			{
				return f_(a...);
			}
		};

		template <class F, template<class...> class L, class... A>
		struct handler_wrapper<result<void>, F, L<A...>>
		{
			F f_;
			explicit handler_wrapper( F && f ) noexcept:
				f_(std::forward<F>(f))
			{
			}
			result<void> operator()( A... a )
			{
				f_(a...);
				return { };
			}
		};
	}

	template <class TryBlock, class... Handlers>
	typename leaf_detail::function_traits<TryBlock>::return_type handle_some( TryBlock && try_block, Handlers && ... handlers ) noexcept
	{
		using namespace leaf_detail;
		using R = typename function_traits<TryBlock>::return_type;
		typename deduce_static_store<typename handlers_args_set<Handlers...>::type>::type ss;
		if( auto r = std::forward<TryBlock>(try_block)() )
		{
			ss.set_reset(true);
			return r;
		}
		else
		{
			error const e = r.error();
			if( auto rr = ss.handle_error(error_info(e), handler_wrapper<R,Handlers>(std::forward<Handlers>(handlers))..., [&r] { return r; } ) )
			{
				ss.set_reset(true);
				return rr;
			}
			else
				return rr;
		}
	}

} }

#endif
