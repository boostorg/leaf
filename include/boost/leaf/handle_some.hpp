#ifndef BOOST_LEAF_73685B76115611E9950D61678B7F4AFC
#define BOOST_LEAF_73685B76115611E9950D61678B7F4AFC

// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/static_store.hpp>

namespace boost { namespace leaf {

	template <class TryBlock, class... Handler>
	decltype(std::declval<TryBlock>()()) handle_some( TryBlock && try_block, Handler && ... handler )
	{
		using namespace leaf_detail;
		using R = decltype(std::declval<TryBlock>()());
		static_assert(is_result_type<R>::value, "The try_block passed to handle_some must be registered with leaf::is_result_type.");
		typename deduce_static_store<typename handler_args_set<Handler...>::type>::type ss;
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

	namespace leaf_detail
	{
		template <class TryBlock, class HandlersTuple, std::size_t ... I>
		decltype(std::declval<TryBlock>()()) handle_some_tuple( leaf_detail_mp11::index_sequence<I...>, TryBlock && try_block, HandlersTuple & handlers )
		{
			return handle_some( std::forward<TryBlock>(try_block), std::get<I>(handlers)... );
		}
	}

	template <class TryBlock, class... Handler>
	decltype(std::declval<TryBlock>()()) handle_some( TryBlock && try_block, std::tuple<Handler...> & handlers )
	{
		return leaf_detail::handle_some_tuple( leaf_detail_mp11::make_index_sequence<std::tuple_size<std::tuple<Handler...>>::value>(), std::forward<TryBlock>(try_block), handlers );
	}

} }

#endif
