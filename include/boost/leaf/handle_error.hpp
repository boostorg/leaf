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
		LEAF_CONSTEXPR LEAF_ALWAYS_INLINE typename std::decay<decltype(std::declval<TryBlock>()().value())>::type nocatch_context<E...>::try_handle_all( TryBlock && try_block, H && ... h )
		{
			using namespace leaf_detail;
			static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The return type of the try_block passed to a try_handle_all function must be registered with leaf::is_result_type");
			auto active_context = activate_context(*this);
			if( auto r = std::forward<TryBlock>(try_block)() )
				return r.value();
			else
				return this->handle_all_(r, std::forward<H>(h)...);
		}

		template <class... E>
		template <class TryBlock, class RemoteH>
		LEAF_CONSTEXPR LEAF_ALWAYS_INLINE typename std::decay<decltype(std::declval<TryBlock>()().value())>::type nocatch_context<E...>::remote_try_handle_all( TryBlock && try_block, RemoteH && h )
		{
			using namespace leaf_detail;
			static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The return type of the try_block passed to a remote_try_handle_all function must be registered with leaf::is_result_type");
			auto active_context = activate_context(*this);
			if( auto r = std::forward<TryBlock>(try_block)() )
				return r.value();
			else
				return this->remote_handle_all_(r, std::forward<RemoteH>(h));
		}

		template <class... E>
		template <class TryBlock, class... H>
		LEAF_CONSTEXPR LEAF_ALWAYS_INLINE typename std::decay<decltype(std::declval<TryBlock>()())>::type nocatch_context<E...>::try_handle_some( TryBlock && try_block, H && ... h )
		{
			using namespace leaf_detail;
			static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The return type of the try_block passed to a try_handle_some function must be registered with leaf::is_result_type");
			auto active_context = activate_context(*this);
			if( auto r = std::forward<TryBlock>(try_block)() )
				return r;
			else
			{
				auto rr = this->handle_some_(std::move(r), std::forward<H>(h)...);
				if( !rr )
					this->propagate();
				return rr;
			}
		}

		template <class... E>
		template <class TryBlock, class RemoteH>
		LEAF_CONSTEXPR LEAF_ALWAYS_INLINE typename std::decay<decltype(std::declval<TryBlock>()())>::type nocatch_context<E...>::remote_try_handle_some( TryBlock && try_block, RemoteH && h )
		{
			using namespace leaf_detail;
			static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The return type of the try_block passed to a remote_try_handle_some function must be registered with leaf::is_result_type");
			auto active_context = activate_context(*this);
			if( auto r = std::forward<TryBlock>(try_block)() )
				return r;
			else
			{
				auto rr = remote_handle_some_(std::move(r), std::forward<RemoteH>(h));
				if( !rr )
					this->propagate();
				return rr;
			}
		}
	}

} }

#endif
