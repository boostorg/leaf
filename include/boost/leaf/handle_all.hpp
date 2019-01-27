#ifndef BOOST_LEAF_539464A021D411E9BC8A79361E29EE6E
#define BOOST_LEAF_539464A021D411E9BC8A79361E29EE6E

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

	namespace leaf_detail
	{
		template <class TryBlock, class HandlersTuple, std::size_t ... I>
		typename std::remove_reference<decltype(std::declval<TryBlock>()().value())>::type handle_all_tuple( std::index_sequence<I...>, TryBlock && try_block, HandlersTuple & handlers )
		{
			return handle_all( std::forward<TryBlock>(try_block), std::get<I>(handlers)... );
		}
	}

	template <class TryBlock, class... Handler>
	typename std::remove_reference<decltype(std::declval<TryBlock>()().value())>::type handle_all( TryBlock && try_block, std::tuple<Handler...> & handlers )
	{
		return leaf_detail::handle_all_tuple( std::make_index_sequence<std::tuple_size<std::tuple<Handler...>>::value>(), std::forward<TryBlock>(try_block), handlers );
	}

} }

#endif
