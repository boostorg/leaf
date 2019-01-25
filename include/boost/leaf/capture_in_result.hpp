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
		template <class F, class mp_args, class... E>
		struct result_trap;

		template <class F, template<class...> class L, class... A, class... E>
		struct result_trap<F, L<A...>,E...>
		{
			F f_;

		public:

			explicit result_trap( F && f ) noexcept:
				f_(std::move(f))
			{
			}

			decltype(std::declval<F>()(std::declval<A>()...)) operator()( A ... a ) const
			{
				static_store<E...> ss;
				ss.set_reset(true);
				if( auto r = f_(std::forward<A>(a)...) )
					return r;
				else
					return decltype(r)( std::make_shared<dynamic_store_impl<E...>>(r.error().value(),std::move(ss)) );
			}
		};
	}

	template <class... E, class F>
	leaf_detail::result_trap<F,typename leaf_detail::function_traits<F>::mp_args,E...> capture_in_result( F && f ) noexcept
	{
		using namespace leaf_detail;
		return result_trap<F,typename function_traits<F>::mp_args,E...>(std::move(f));
	}

} }

#endif
