#ifndef BOOST_LEAF_73685B76115611E9950D61678B7F4AFC
#define BOOST_LEAF_73685B76115611E9950D61678B7F4AFC

// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/static_store.hpp>

namespace boost { namespace leaf {

	template <class TryBlock, class... Handler>
	typename std::remove_reference<decltype(std::declval<TryBlock>()().value())>::type handle_all( TryBlock && try_block, Handler && ... handler )
	{
		using namespace leaf_detail;
		using R = decltype(std::declval<TryBlock>()());
		static_assert(is_result_type<R>::value, "The try_block passed to handle_all must be registered with leaf::is_result_type.");
		typename deduce_static_store<typename handler_args_set<Handler...>::type>::type ss;
		ss.set_reset(true);
		if( auto r = std::forward<TryBlock>(try_block)() )
			return r.value();
		else
			return ss.handle_error(error_info(r.error()), std::forward<Handler>(handler)...);
	}

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

} }

#endif
