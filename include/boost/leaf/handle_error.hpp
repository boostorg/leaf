#ifndef LEAF_539464A021D411E9BC8A79361E29EE6E
#define LEAF_539464A021D411E9BC8A79361E29EE6E

// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/handle.hpp>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <class... E>
		template <class TryBlock, class... H>
		inline typename std::decay<decltype(std::declval<TryBlock>()().value())>::type nocatch_context<E...>::try_handle_all( TryBlock && try_block, H && ... h )
		{
			context_activator active_context(*this, on_deactivation::do_not_propagate);
			return this->try_handle_all_( std::forward<TryBlock>(try_block), std::forward<H>(h)... );
		}

		template <class... E>
		template <class TryBlock, class RemoteH>
		inline typename std::decay<decltype(std::declval<TryBlock>()().value())>::type nocatch_context<E...>::remote_try_handle_all( TryBlock && try_block, RemoteH && h )
		{
			context_activator active_context(*this, on_deactivation::do_not_propagate);
			return this->remote_try_handle_all_( std::forward<TryBlock>(try_block), std::forward<RemoteH>(h) );
		}

		template <class... E>
		template <class TryBlock, class... H>
		inline typename std::decay<decltype(std::declval<TryBlock>()())>::type nocatch_context<E...>::try_handle_some( TryBlock && try_block, H && ... h )
		{
			context_activator active_context(*this, on_deactivation::propagate_if_uncaught_exception);
			return this->try_handle_some_( active_context, std::forward<TryBlock>(try_block), std::forward<H>(h)... );
		}

		template <class... E>
		template <class TryBlock, class RemoteH>
		inline typename std::decay<decltype(std::declval<TryBlock>()())>::type nocatch_context<E...>::remote_try_handle_some( TryBlock && try_block, RemoteH && h )
		{
			context_activator active_context(*this, on_deactivation::propagate_if_uncaught_exception);
			return this->remote_try_handle_some_( active_context, std::forward<TryBlock>(try_block), std::forward<RemoteH>(h) );
		}
	}

} }

#endif
