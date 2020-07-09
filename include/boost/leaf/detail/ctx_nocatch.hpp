#ifndef BOOST_LEAF_DETAIL_CTX_NOCATCH_HPP_INCLUDED
#define BOOST_LEAF_DETAIL_CTX_NOCATCH_HPP_INCLUDED

// Copyright (c) 2018-2020 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if defined(__clang__)
#	pragma clang system_header
#elif (__GNUC__*100+__GNUC_MINOR__>301) && !defined(BOOST_LEAF_ENABLE_WARNINGS)
#	pragma GCC system_header
#elif defined(_MSC_VER) && !defined(BOOST_LEAF_ENABLE_WARNINGS)
#	pragma warning(push,1)
#endif

#ifndef BOOST_LEAF_NO_EXCEPTIONS
#	error This header requires exception handling to be disabled
#endif

namespace boost { namespace leaf {

	template <class... E>
	template <class TryBlock, class... H>
	BOOST_LEAF_CONSTEXPR BOOST_LEAF_ALWAYS_INLINE
	typename std::decay<decltype(std::declval<TryBlock>()().value())>::type
	context<E...>::
	try_handle_all( TryBlock && try_block, H && ... h )
	{
		using namespace leaf_detail;
		static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The return type of the try_block passed to a try_handle_all function must be registered with leaf::is_result_type");
		auto active_context = activate_context(*this);
		if( auto r = std::forward<TryBlock>(try_block)() )
			return r.value();
		else
		{
			error_id id = r.error();
			this->deactivate();
			using R = typename std::decay<decltype(std::declval<TryBlock>()().value())>::type;
			return this->template handle_error<R>(std::move(id), std::forward<H>(h)...);
		}
	}

	template <class... E>
	template <class TryBlock, class... H>
	BOOST_LEAF_NODISCARD BOOST_LEAF_CONSTEXPR BOOST_LEAF_ALWAYS_INLINE
	typename std::decay<decltype(std::declval<TryBlock>()())>::type
	context<E...>::
	try_handle_some( TryBlock && try_block, H && ... h )
	{
		using namespace leaf_detail;
		static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The return type of the try_block passed to a try_handle_some function must be registered with leaf::is_result_type");
		auto active_context = activate_context(*this);
		if( auto r = std::forward<TryBlock>(try_block)() )
			return r;
		else
		{
			error_id id = r.error();
			this->deactivate();
			using R = typename std::decay<decltype(std::declval<TryBlock>()())>::type;
			auto rr = this->template handle_error<R>(std::move(id), std::forward<H>(h)..., [&r]()->R { return std::move(r); });
			if( !rr )
				this->propagate();
			return rr;
		}
	}

} }

#endif
