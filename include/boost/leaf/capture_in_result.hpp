#ifndef BOOST_LEAF_2416C558123711E9B9D9691F8C7F4AFC
#define BOOST_LEAF_2416C558123711E9B9D9691F8C7F4AFC

// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/result.hpp>
#include <boost/leaf/detail/dynamic_store_impl.hpp>

namespace boost { namespace leaf {

	template <class T>
	result<T>::result( std::shared_ptr<leaf_detail::dynamic_store> && cap ) noexcept:
		cap_(std::move(cap)),
		which_(leaf_detail::result_variant::cap)
	{
	}

	inline result<void>::result( std::shared_ptr<leaf_detail::dynamic_store> && cap ) noexcept:
		base(std::move(cap))
	{
	}

	template <class... E, class F, class... A>
	decltype(std::declval<F>()(std::forward<A>(std::declval<A>())...)) capture_in_result_explicit(F && f, A... a)
	{
		using namespace leaf_detail;
		using R = decltype(std::declval<F>()(std::forward<A>(a)...));
		using StaticStore = typename deduce_static_store<typename error_type_set<e_original_ec, E...>::type>::type;
		StaticStore ss;
		if( auto r = std::forward<F>(f)(std::forward<A>(a)...) )
			return r;
		else
			return R(std::make_shared<typename StaticStore::dynamic_store_impl_type>(error_info(r.error()).err_id(), std::move(ss)));
	}

	template <class... E, class Alloc, class F, class... A>
	decltype(std::declval<F>()(std::forward<A>(std::declval<A>())...)) capture_in_result_explicit(Alloc alloc, F && f, A... a)
	{
		using namespace leaf_detail;
		using R = decltype(std::declval<F>()(std::forward<A>(a)...));
		using StaticStore = typename deduce_static_store<typename error_type_set<e_original_ec, E...>::type>::type;
		StaticStore ss;
		if( auto r = std::forward<F>(f)(std::forward<A>(a)...) )
			return r;
		else
			return R(std::allocate_shared<typename StaticStore::dynamic_store_impl_type>(alloc, error_info(r.error()).err_id(), std::move(ss)));
	}

	template <class Handler, class F, class... A>
	decltype(std::declval<F>()(std::forward<A>(std::declval<A>())...)) capture_in_result(F && f, A... a)
	{
		using namespace leaf_detail;
		using R = decltype(std::declval<F>()(std::forward<A>(a)...));
		using StaticStore = typename deduce_static_store<typename leaf_detail::handler_args_list<typename leaf_detail::function_traits<Handler>::return_type>::type>::type;
		StaticStore ss;
		if( auto r = std::forward<F>(f)(std::forward<A>(a)...) )
			return r;
		else
			return R(std::make_shared<typename StaticStore::dynamic_store_impl_type>(error_info(r.error()).err_id(), std::move(ss)));
	}

	template <class Handler, class Alloc, class F, class... A>
	decltype(std::declval<F>()(std::forward<A>(std::declval<A>())...)) capture_in_result(Alloc alloc, F && f, A... a)
	{
		using namespace leaf_detail;
		using R = decltype(std::declval<F>()(std::forward<A>(a)...));
		using StaticStore = typename deduce_static_store<typename leaf_detail::handler_args_list<typename leaf_detail::function_traits<Handler>::return_type>::type>::type;
		StaticStore ss;
		if( auto r = std::forward<F>(f)(std::forward<A>(a)...) )
			return r;
		else
			return R(std::allocate_shared<typename StaticStore::dynamic_store_impl_type>(alloc, error_info(r.error()).err_id(), std::move(ss)));
	}

} }

#endif
