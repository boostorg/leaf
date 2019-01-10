#ifndef BOOST_LEAF_2416C558123711E9B9D9691F8C7F4AFC
#define BOOST_LEAF_2416C558123711E9B9D9691F8C7F4AFC

//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

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
		template <class F, class... E>
		class result_trap
		{
			F f_;

		public:

			explicit result_trap( F && f ) noexcept:
				f_(std::move(f))
			{
			}

			template <class... A>
			decltype(std::declval<F>()(std::declval<A>()...)) operator()( A && ... a ) const noexcept
			{
				static_store<E...> ss;
				ss.set_reset(true);
				if( auto r = f_(std::forward<A>(a)...) )
					return r;
				else
					return decltype(r)( std::make_shared<dynamic_store_impl<E...>>(r.error(),std::move(ss)) );
			}
		};
	}

	template <class... E, class F>
	leaf_detail::result_trap<F,E...> capture_result( F && f ) noexcept
	{
		return leaf_detail::result_trap<F,E...>(std::move(f));
	}

} }

#endif
