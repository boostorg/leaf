#ifndef BOOST_LEAF_14440B9CF07011E88377FD4B0ABE8030
#define BOOST_LEAF_14440B9CF07011E88377FD4B0ABE8030

//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/mp11.hpp>
#include <tuple>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <class...>
		using void_t = void;

		template<class F,class V=void>
		struct function_traits
		{
		};

		template<class F>
		struct function_traits<F,void_t<decltype(&F::operator())>>
		{
		private:

			typedef function_traits<decltype(&F::operator())> tr;

		public:

			typedef typename tr::return_type return_type;
			static constexpr int arity = tr::arity - 1;

			using mp_args = typename leaf_detail_mp11::mp_rest<typename tr::mp_args>;

			template <int I>
			struct arg:
				tr::template arg<I+1>
			{
			};
		};

		template<class R, class... A>
		struct function_traits<R(A...)>
		{
			typedef R return_type;
			static constexpr int arity = sizeof...(A);

			using mp_args = leaf_detail_mp11::mp_list<A...>;

			template <int I>
			struct arg
			{
				static_assert(I < arity, "I out of range");
				typedef typename std::tuple_element<I,std::tuple<A...>>::type type;
			};
		};

		template<class F> struct function_traits<F&> : function_traits<F> { };
		template<class F> struct function_traits<F&&> : function_traits<F> { };
		template<class R, class... A> struct function_traits<R(*)(A...)> : function_traits<R(A...)> { };
		template<class C, class R, class... A> struct function_traits<R(C::*)(A...)> : function_traits<R(C&,A...)> { };
		template<class C, class R, class... A> struct function_traits<R(C::*)(A...) const> : function_traits<R(C const &,A...)> { };
		template<class C, class R> struct function_traits<R(C::*)> : function_traits<R(C&)> { };
	} //namespace leaf_detail

} }

#endif
