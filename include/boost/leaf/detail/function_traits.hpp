//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_14440B9CF07011E88377FD4B0ABE8030
#define UUID_14440B9CF07011E88377FD4B0ABE8030

#include <tuple>
#include <type_traits>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <class... T> struct mp_list { };
		template <class, class... T> struct mp_pop_front { using type = mp_list<T...>; };
		template <class L> struct mp_list_pop_front;
		template <class... T> struct mp_list_pop_front<mp_list<T...>> { using type = typename mp_pop_front<T...>::type; };

		template<class F>
		struct function_traits
		{
		private:
			typedef function_traits<decltype(&F::operator())> tr;
		public:
			typedef typename tr::ret ret;
			static constexpr int arity = tr::arity - 1;
			using mp_args = typename mp_list_pop_front<typename tr::mp_args>::type;

			template <int I>
			struct arg:
				tr::template arg<I+1>
			{
			};
		};

		template<class R, class... Args>
		struct function_traits<R(Args...)>
		{
			typedef R ret;
			static constexpr int arity = sizeof...(Args);

			using mp_args = mp_list<Args...>;

			template <int I>
			struct arg
			{
				static_assert(I < arity, "I out of range");
				typedef typename std::tuple_element<I,std::tuple<Args...>>::type type;
				typedef typename std::remove_cv<typename std::remove_reference<type>::type>::type type_;
			};
		};

		template<class F> struct function_traits<F&> : public function_traits<F> { };
		template<class F> struct function_traits<F&&> : public function_traits<F> { };
		template<class R, class... Args> struct function_traits<R(*)(Args...)> : public function_traits<R(Args...)> { };
		template<class C, class R, class... Args> struct function_traits<R(C::*)(Args...)> : public function_traits<R(C&,Args...)> { };
		template<class C, class R, class... Args> struct function_traits<R(C::*)(Args...) const> : public function_traits<R(C&,Args...)> { };
		template<class C, class R> struct function_traits<R(C::*)> : public function_traits<R(C&)> { };

	} //namespace leaf_detail

} }

#endif
