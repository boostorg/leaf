#ifndef BOOST_LEAF_73685B76115611E9950D61678B7F4AFC
#define BOOST_LEAF_73685B76115611E9950D61678B7F4AFC

// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/static_store.hpp>

namespace boost { namespace leaf {

	template <class TryBlock, class... Handler>
	decltype(std::declval<TryBlock>()()) handle_some( TryBlock && try_block, Handler && ... handler ) noexcept
	{
		using namespace leaf_detail;
		using R = decltype(std::declval<TryBlock>()());
		static_assert(is_result_type<R>::value, "The try_block passed to handle_some must be registered with leaf::is_result_type.");
		deduce_static_store<handler_args_set<Handler...>> ss;
		if( auto r = std::forward<TryBlock>(try_block)() )
		{
			ss.set_reset(true);
			return r;
		}
		else
		{
			auto rr = ss.handle_error_(error_info(r.error()), handler_wrapper<R,Handler>(std::forward<Handler>(handler))..., [&r] { return r; } );
			if( rr )
				ss.set_reset(true);
			return rr;
		}
	}

	////////////////////////////////////////

	struct error_in_remote_handle_some: error_info
	{
		void const * const ss_;

		error_in_remote_handle_some( void const * ss, error_id const & id ) noexcept:
			error_info(id),
			ss_(ss)
		{
			assert(ss_!=0);
		}

		error_in_remote_handle_some( void const * ss, std::error_code const & ec ) noexcept:
			error_info(ec),
			ss_(ss)
		{
			assert(ss_!=0);
		}
	};

	template <class TryBlock, class Handler>
	decltype(std::declval<TryBlock>()()) remote_handle_some( TryBlock && try_block, Handler && handler ) noexcept
	{
		using namespace leaf_detail;
		using R = decltype(std::declval<TryBlock>()());
		static_assert(is_result_type<R>::value, "The try_block passed to handle_some must be registered with leaf::is_result_type.");
		deduce_static_store<handler_args_list<fn_return_type<Handler>>> ss;
		if( auto r = std::forward<TryBlock>(try_block)() )
		{
			ss.set_reset(true);
			return r;
		}
		else
		{
			auto rr = std::forward<Handler>(handler)(error_in_remote_handle_some(&ss, r.error())).get();
			if( rr )
				ss.set_reset(true);
			return rr;
		}
	}

	template <class... Handler>
	typename leaf_detail::handle_error_dispatch<Handler...>::result_type handle_error( error_in_remote_handle_some const & error, Handler && ... handler ) noexcept
	{
		using namespace leaf_detail;
		return handle_error_dispatch<Handler...>::handle(error, std::forward<Handler>(handler)... );
	}

} }

#endif
