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
		class res_make
		{
		protected:

			template <class... E>
			static std::shared_ptr<dynamic_store> alloc(int err_id, static_store<E...> && ss)
			{
				assert(err_id!=0);
				return std::make_shared<dynamic_store_impl<E...>>(err_id, std::move(ss));
			}
		};

		template <class Alloc>
		class res_alloc
		{
			Alloc a_;

			protected:

			explicit res_alloc( Alloc const & a ):
				a_(a)
			{
			}

			template <class... E>
			std::shared_ptr<dynamic_store> alloc(int err_id, static_store<E...> && ss) const
			{
				assert(err_id!=0);
				return std::allocate_shared<dynamic_store_impl<E...>>(a_, err_id, std::move(ss));
			}
		};

		template <class AllocHelper, class F, class mp_args, class mp_e_types>
		class result_trap;

		template <class AllocHelper, class F, template <class...> class L1, template <class...> class L2, class... A, class... E>
		class result_trap<AllocHelper, F, L1<A...>, L2<E...>>: AllocHelper
		{
			F f_;

		public:

			result_trap( F f ):
				f_(f)
			{
			}

			template <class Alloc>
			result_trap( F f, Alloc const & a ):
				AllocHelper(a),
				f_(f)
			{
			}

			decltype(std::declval<F>()(std::declval<A>()...)) operator()( A ... a ) const
			{
				typename deduce_static_store<typename error_type_set<e_original_ec, E...>::type>::type ss;
				ss.set_reset(true);
				if( auto r = f_(std::forward<A>(a)...) )
					return r;
				else
					return decltype(r)( this->alloc(r.error().value(), std::move(ss)) );
			}
		};
	}

	template <class... E, class F>
	leaf_detail::result_trap<leaf_detail::res_make, F, typename leaf_detail::function_traits<F>::mp_args, leaf_detail_mp11::mp_list<E...>>
	capture_in_result_explicit( F && f ) noexcept
	{
		return f;
	}

	template <class... E, class F, class Alloc>
	leaf_detail::result_trap<leaf_detail::res_alloc<Alloc>, F, typename leaf_detail::function_traits<F>::mp_args, leaf_detail_mp11::mp_list<E...>>
	capture_in_result_explicit_alloc( Alloc const & a, F && f ) noexcept
	{
		return leaf_detail::result_trap<leaf_detail::res_alloc<Alloc>, F, typename leaf_detail::function_traits<F>::mp_args, leaf_detail_mp11::mp_list<E...>>(
			std::forward<F>(f), a);
	}

	template <class HandlerTypeList, class F>
	leaf_detail::result_trap<leaf_detail::res_make, F, typename leaf_detail::function_traits<F>::mp_args, typename leaf_detail::handler_args_list<HandlerTypeList>::type>
	capture_in_result( F && f, HandlerTypeList const * = 0 ) noexcept
	{
		return f;
	}

	template <class HandlerTypeList, class F, class Alloc>
	leaf_detail::result_trap<leaf_detail::res_alloc<Alloc>, F, typename leaf_detail::function_traits<F>::mp_args, typename leaf_detail::handler_args_list<HandlerTypeList>::type>
	capture_in_result_alloc( Alloc const & a, F && f, HandlerTypeList const * = 0 ) noexcept
	{
		return leaf_detail::result_trap<leaf_detail::res_alloc<Alloc>, F, typename leaf_detail::function_traits<F>::mp_args, typename leaf_detail::handler_args_list<HandlerTypeList>::type>(
			std::forward<F>(f), a);
	}

} }

#endif
