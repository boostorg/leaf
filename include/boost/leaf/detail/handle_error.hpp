#ifndef BOOST_LEAF_F55070940BFF11E9A3EB73FBF47612F3
#define BOOST_LEAF_F55070940BFF11E9A3EB73FBF47612F3

//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/error.hpp>
#include <boost/leaf/detail/function_traits.hpp>
#include <tuple>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <class R,bool IsIntegral=std::is_integral<R>::value>
		struct unhandled_error_base_helper;

		template <class R>
		struct unhandled_error_base_helper<R,true>
		{
			static constexpr R value( leaf::error const & ) noexcept { return static_cast<R>(-1); }
		};

		template <class R>
		struct unhandled_error_base_helper<R,false>
		{
			static constexpr R value( leaf::error const & ) noexcept { return R(); }
		};

		template <class R>
		struct unhandled_error_base: unhandled_error_base_helper<R>
		{
		};

		template <>
		struct unhandled_error_base<bool>
		{
			static constexpr bool value( leaf::error const & ) noexcept { return false; }
		};
	}

	template <class T>
	struct uhnandled_error: leaf_detail::unhandled_error_base<T>
	{
	};

	namespace leaf_detail
	{
		template <class F,class R=typename function_traits<F>::return_type>
		struct handler_wrapper_base
		{
			typedef typename function_traits<F>::return_type return_type;

			F f_;

			explicit handler_wrapper_base( F && f ) noexcept:
				f_(std::forward<F>(f))
			{
			}

			template <class... E>
			return_type operator()( E && ... e ) const
			{
				return f_(std::forward<E>(e)...);
			}
		};

		template <class F>
		struct handler_wrapper_base<F,void>
		{
			typedef bool return_type;

			F f_;

			explicit handler_wrapper_base( F && f ) noexcept:
				f_(std::forward<F>(f))
			{
			}

			template <class... E>
			return_type operator()( E && ... e ) const
			{
				f_(std::forward<E>(e)...);
				return true;
			}
		};

		template <class F, class R=void>
		struct handler_wrapper
		{
			typedef void return_type;
		};

		template <class F>
		struct handler_wrapper<F,void_t<typename function_traits<F>::return_type>>: handler_wrapper_base<F>
		{
			explicit handler_wrapper( F && f ) noexcept:
				handler_wrapper_base<F>(std::forward<F>(f))
			{
			}
		};

		template <class... F>
		struct handler_pack_return_type;

		template <class Car,class... Cdr>
		struct handler_pack_return_type<Car,Cdr...>
		{
			typedef typename handler_wrapper<Car>::return_type return_type;
		};
	} //leaf_detail

} }

#endif
