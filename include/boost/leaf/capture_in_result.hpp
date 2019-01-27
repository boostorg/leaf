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

	namespace leaf_detail
	{
		template <class F, class mp_args, class mp_e_types>
		struct result_trap;

		template <class F, template <class...> class L1, template <class...> class L2, class... A, class... E>
		struct result_trap<F, L1<A...>, L2<E...>>
		{
			F f_;

			decltype(std::declval<F>()(std::declval<A>()...)) operator()( A ... a ) const
			{
				typename deduce_static_store<typename error_type_set<e_original_ec, E...>::type>::type ss;
				ss.set_reset(true);
				if( auto r = f_(std::forward<A>(a)...) )
					return r;
				else
					return decltype(r)( std::make_shared<dynamic_store_impl<e_original_ec, E...>>(r.error().value(),std::move(ss)) );
			}
		};
	}

	template <class... E, class F>
	leaf_detail::result_trap<F, typename leaf_detail::function_traits<F>::mp_args, leaf_detail_mp11::mp_list<E...>>
	capture_in_result( F && f ) noexcept
	{
		return {std::move(f)};
	}

	template <template <class...> class Tup, class... Handler, class F>
	leaf_detail::result_trap<F, typename leaf_detail::function_traits<F>::mp_args, typename leaf_detail::handler_args_set<Handler...>::type>
	capture_in_result( F && f, Tup<Handler...> const & ) noexcept
	{
		return {std::move(f)};
	}

} }

#endif
