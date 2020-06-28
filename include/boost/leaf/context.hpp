#ifndef BOOST_LEAF_CONTEXT_HPP_INCLUDED
#define BOOST_LEAF_CONTEXT_HPP_INCLUDED

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

#include <boost/leaf/error.hpp>

namespace boost { namespace leaf {

	class error_info;
	class diagnostic_info;
	class verbose_diagnostic_info;

	namespace leaf_detail
	{
		template <int I, class Tuple>
		struct tuple_for_each
		{
			BOOST_LEAF_CONSTEXPR static void activate( Tuple & tup ) noexcept
			{
				static_assert(!std::is_same<error_info, typename std::decay<decltype(std::get<I-1>(tup))>::type>::value, "Bug in LEAF: context type deduction");
				tuple_for_each<I-1,Tuple>::activate(tup);
				std::get<I-1>(tup).activate();
			}

			BOOST_LEAF_CONSTEXPR static void deactivate( Tuple & tup ) noexcept
			{
				static_assert(!std::is_same<error_info, typename std::decay<decltype(std::get<I-1>(tup))>::type>::value, "Bug in LEAF: context type deduction");
				std::get<I-1>(tup).deactivate();
				tuple_for_each<I-1,Tuple>::deactivate(tup);
			}

			BOOST_LEAF_CONSTEXPR static void propagate( Tuple & tup ) noexcept
			{
				static_assert(!std::is_same<error_info, typename std::decay<decltype(std::get<I-1>(tup))>::type>::value, "Bug in LEAF: context type deduction");
				auto & sl = std::get<I-1>(tup);
				sl.propagate();
				tuple_for_each<I-1,Tuple>::propagate(tup);
			}

			BOOST_LEAF_CONSTEXPR static void propagate_captured( Tuple & tup, int err_id ) noexcept
			{
				static_assert(!std::is_same<error_info, typename std::decay<decltype(std::get<I-1>(tup))>::type>::value, "Bug in LEAF: context type deduction");
				auto & sl = std::get<I-1>(tup);
				if( sl.has_value(err_id) )
					load_slot(err_id, std::move(sl).value(err_id));
				tuple_for_each<I-1,Tuple>::propagate_captured(tup, err_id);
			}

			static void print( std::ostream & os, void const * tup, int key_to_print )
			{
				BOOST_LEAF_ASSERT(tup != 0);
				tuple_for_each<I-1,Tuple>::print(os, tup, key_to_print);
				std::get<I-1>(*static_cast<Tuple const *>(tup)).print(os, key_to_print);
			}
		};

		template <class Tuple>
		struct tuple_for_each<0, Tuple>
		{
			BOOST_LEAF_CONSTEXPR static void activate( Tuple & ) noexcept { }
			BOOST_LEAF_CONSTEXPR static void deactivate( Tuple & ) noexcept { }
			BOOST_LEAF_CONSTEXPR static void propagate( Tuple & tup ) noexcept { }
			BOOST_LEAF_CONSTEXPR static void propagate_captured( Tuple & tup, int ) noexcept { }
			static void print( std::ostream &, void const *, int ) { }
		};
	}

	////////////////////////////////////////////

	namespace leaf_detail
	{
		class e_unexpected_count;
		class e_unexpected_info;

		template <class T> struct requires_unexpected { constexpr static bool value = false; };
		template <class T> struct requires_unexpected<T const> { constexpr static bool value = requires_unexpected<T>::value; };
		template <class T> struct requires_unexpected<T const &> { constexpr static bool value = requires_unexpected<T>::value; };
		template <class T> struct requires_unexpected<T const *> { constexpr static bool value = requires_unexpected<T>::value; };
		template <> struct requires_unexpected<e_unexpected_count> { constexpr static bool value = true; };
		template <> struct requires_unexpected<e_unexpected_info> { constexpr static bool value = true; };

		template <class L>
		struct unexpected_requested;

		template <template <class ...> class L>
		struct unexpected_requested<L<>>
		{
			constexpr static bool value = false;
		};

		template <template <class...> class L, template <class> class S, class Car, class... Cdr>
		struct unexpected_requested<L<S<Car>, S<Cdr>...>>
		{
			constexpr static bool value = requires_unexpected<Car>::value || unexpected_requested<L<S<Cdr>...>>::value;
		};
	}

	////////////////////////////////////////////

	namespace leaf_detail
	{
		template <class T>
		struct has_member_value;

		template <class Enum, bool = has_member_value<Enum>::value>
		struct match_traits;

		template <class A, bool RequiresCatch = std::is_base_of<std::exception, typename std::decay<A>::type>::value>
		struct handler_argument_traits_defaults
		{
			using error_type = typename std::decay<A>::type;
			constexpr static bool requires_catch = RequiresCatch;
			constexpr static bool always_available = false;

			template <class Tup>
			BOOST_LEAF_CONSTEXPR static error_type const * check( Tup const &, error_info const & ) noexcept;

			template <class Tup>
			BOOST_LEAF_CONSTEXPR static error_type * check( Tup &, error_info const & ) noexcept;

			template <class Tup>
			BOOST_LEAF_CONSTEXPR static A get( Tup & tup, error_info const & ei ) noexcept
			{
				return *check(tup, ei);
			}
		};

		template <class A>
		struct handler_argument_always_available
		{
			using error_type = A;
			constexpr static bool requires_catch = false;
			constexpr static bool always_available = true;

			template <class Tup>
			BOOST_LEAF_CONSTEXPR static bool check( Tup &, error_info const & ) noexcept
			{
				return true;
			};
		};

		template <class A>
		struct handler_argument_traits: handler_argument_traits_defaults<A>
		{
			template <class Tup>
			BOOST_LEAF_CONSTEXPR static A get( Tup & tup, error_info const & ei ) noexcept
			{
				return *handler_argument_traits_defaults<A>::check(tup, ei);
			}
		};

		template <class A>
		struct handler_argument_traits<A *>: handler_argument_always_available<typename std::remove_const<A>::type>
		{
			template <class Tup>
			static A * get( Tup & tup, error_info const & ei) noexcept
			{
				return handler_argument_traits_defaults<A>::check(tup, ei);
			}
		};

		template <>
		struct handler_argument_traits<error_info const &>: handler_argument_always_available<void>
		{
			template <class Tup>
			static error_info const & get( Tup const &, error_info const & ei ) noexcept
			{
				return ei;
			}
		};

		template <>
		struct handler_argument_traits<diagnostic_info>: handler_argument_always_available<e_unexpected_count>
		{
			template <class Tup>
			BOOST_LEAF_CONSTEXPR static diagnostic_info get( Tup const & tup, error_info const & ei ) noexcept;
		};

		template <>
		struct handler_argument_traits<diagnostic_info const &>: handler_argument_traits<diagnostic_info>
		{
		};

		template <>
		struct handler_argument_traits<verbose_diagnostic_info>: handler_argument_always_available<e_unexpected_info>
		{
			template <class Tup>
			BOOST_LEAF_CONSTEXPR static verbose_diagnostic_info get( Tup const & tup, error_info const & ei ) noexcept;
		};

		template <>
		struct handler_argument_traits<verbose_diagnostic_info const &>: handler_argument_traits<verbose_diagnostic_info>
		{
		};

		template <class P, class A, bool RequiresCatch = false>
		struct handler_argument_predicate
		{
			using error_type = typename handler_argument_traits<A>::error_type;
			constexpr static bool requires_catch = RequiresCatch;
			constexpr static bool always_available = false;

			template <class Tup>
			BOOST_LEAF_CONSTEXPR static bool check( Tup & tup, error_info const & ei ) noexcept
			{
				if( auto * a = handler_argument_traits<A>::check(tup, ei) )
					if( P(*a) )
						return true;
				return false;
			};

			template <class Tup>
			BOOST_LEAF_CONSTEXPR static P get( Tup const & tup, error_info const & ei ) noexcept
			{
				auto * a = handler_argument_traits<A>::check(tup, ei);
				BOOST_LEAF_ASSERT(a != 0);
				P p(*a);
				BOOST_LEAF_ASSERT(p);
				return p;
			}
		};
	}

	template <class... Ex>
	class catch_;

	namespace leaf_detail
	{
		template <class... Ex>
		struct handler_argument_traits<catch_<Ex...>>: handler_argument_predicate<catch_<Ex...>, std::exception, true>
		{
		};

		template <class... Ex>
		struct handler_argument_traits<catch_<Ex...> const &>
		{
			static_assert(sizeof(catch_<Ex...>) == 0, "Error handlers must take leaf::catch_<> by value");
		};

		template <class... Ex>
		struct handler_argument_traits<catch_<Ex...> &>
		{
			static_assert(sizeof(catch_<Ex...>) == 0, "Error handlers must take leaf::catch_<> by value");
		};
	}

#if __cplusplus >= 201703L

	template <class E, auto V1, auto... V>
	class match;

	namespace leaf_detail
	{
		template <class E, auto V1, auto... V>
		struct handler_argument_traits<match<E, V1, V...>>: handler_argument_predicate<match<E, V1, V...>, typename match_traits<E>::error_type>
		{
		};

		template <class E, auto V1, auto... V>
		struct handler_argument_traits<match<E, V1, V...> const &>
		{
			static_assert(sizeof(E) == 0, "Error handlers must take leaf::match<> by value");
		};

		template <class E, auto V1, auto... V>
		struct handler_argument_traits<match<E, V1, V...> &>
		{
			static_assert(sizeof(E) == 0, "Error handlers must take leaf::match<> by value");
		};
	}

#else

	template <class E, typename leaf_detail::match_traits<E>::enum_type V1, typename leaf_detail::match_traits<E>::enum_type... V>
	class match;

	namespace leaf_detail
	{
		template <class E, typename match_traits<E>::enum_type V1, typename match_traits<E>::enum_type... V>
		struct handler_argument_traits<match<E, V1, V...>>: handler_argument_predicate<match<E, V1, V...>, typename match_traits<E>::error_type>
		{
		};

		template <class E, typename match_traits<E>::enum_type V1, typename match_traits<E>::enum_type... V>
		struct handler_argument_traits<match<E, V1, V...> const &>
		{
			static_assert(sizeof(E) == 0, "Error handlers must take leaf::match<> by value");
		};

		template <class E, typename match_traits<E>::enum_type V1, typename match_traits<E>::enum_type... V>
		struct handler_argument_traits<match<E, V1, V...> &>
		{
			static_assert(sizeof(E) == 0, "Error handlers must take leaf::match<> by value");
		};
	}

#endif

	namespace leaf_detail
	{
		template <class T>
		using translate_type = typename handler_argument_traits<T>::error_type;

		template <class... T>
		struct translate_list_impl;

		template <template<class...> class L, class... T>
		struct translate_list_impl<L<T...>>
		{
			using type = leaf_detail_mp11::mp_list<translate_type<T>...>;
		};

		template <class L> using translate_list = typename translate_list_impl<L>::type;

		template <class T> struct does_not_participate_in_context_deduction: std::false_type { };
		template <> struct does_not_participate_in_context_deduction<void>: std::true_type { };

		template <class L>
		struct transform_e_type_list_impl;

		template <template<class...> class L, class... T>
		struct transform_e_type_list_impl<L<T...>>
		{
			using type =
				leaf_detail_mp11::mp_remove_if<
					leaf_detail_mp11::mp_unique<
						translate_list<L<T...>>
					>,
					does_not_participate_in_context_deduction
				>;
		};

		template <class L> using transform_e_type_list = typename transform_e_type_list_impl<L>::type;

		template <class L>
		struct deduce_e_tuple_impl;

		template <template <class...> class L, class... E>
		struct deduce_e_tuple_impl<L<E...>>
		{
			using type = std::tuple<slot<E>...>;
		};

		template <class... E>
		using deduce_e_tuple = typename deduce_e_tuple_impl<transform_e_type_list<leaf_detail_mp11::mp_list<E...>>>::type;
	}

	////////////////////////////////////////////

	namespace leaf_detail
	{
		template <class... E>
		class context_base
		{
			context_base( context_base const & ) = delete;
			context_base & operator=( context_base const & ) = delete;

		public:

			using Tup = deduce_e_tuple<E...>;

		private:

			Tup tup_;
#if !defined(BOOST_LEAF_NO_THREADS) && !defined(NDEBUG)
			std::thread::id thread_id_;
#endif
			bool is_active_;

		protected:

			BOOST_LEAF_CONSTEXPR error_id propagate_captured_errors( error_id err_id ) noexcept
			{
				tuple_for_each<std::tuple_size<Tup>::value,Tup>::propagate_captured(tup_, err_id.value());
				return err_id;
			}

			BOOST_LEAF_CONSTEXPR context_base( context_base && x ) noexcept:
				tup_(std::move(x.tup_)),
				is_active_(false)
			{
				BOOST_LEAF_ASSERT(!x.is_active());
			}

		public:

			BOOST_LEAF_CONSTEXPR context_base() noexcept:
				is_active_(false)
			{
			}

			~context_base() noexcept
			{
				BOOST_LEAF_ASSERT(!is_active());
			}

			BOOST_LEAF_CONSTEXPR Tup const & tup() const noexcept
			{
				return tup_;
			}

			BOOST_LEAF_CONSTEXPR Tup & tup() noexcept
			{
				return tup_;
			}

			BOOST_LEAF_CONSTEXPR void activate() noexcept
			{
				using namespace leaf_detail;
				BOOST_LEAF_ASSERT(!is_active());
				tuple_for_each<std::tuple_size<Tup>::value,Tup>::activate(tup_);
#if BOOST_LEAF_DIAGNOSTICS
				if( unexpected_requested<Tup>::value )
					++tl_unexpected_enabled_counter();
#endif
#if !defined(BOOST_LEAF_NO_THREADS) && !defined(NDEBUG)
				thread_id_ = std::this_thread::get_id();
#endif
				is_active_ = true;
			}

			BOOST_LEAF_CONSTEXPR void deactivate() noexcept
			{
				using namespace leaf_detail;
				BOOST_LEAF_ASSERT(is_active());
				is_active_ = false;
#if !defined(BOOST_LEAF_NO_THREADS) && !defined(NDEBUG)
				BOOST_LEAF_ASSERT(std::this_thread::get_id() == thread_id_);
				thread_id_ = std::thread::id();
#endif
#if BOOST_LEAF_DIAGNOSTICS
				if( unexpected_requested<Tup>::value )
					--tl_unexpected_enabled_counter();
#endif
				tuple_for_each<std::tuple_size<Tup>::value,Tup>::deactivate(tup_);
			}

			BOOST_LEAF_CONSTEXPR void propagate() noexcept
			{
				tuple_for_each<std::tuple_size<Tup>::value,Tup>::propagate(tup_);
			}

			BOOST_LEAF_CONSTEXPR bool is_active() const noexcept
			{
				return is_active_;
			}

			void print( std::ostream & os ) const
			{
				tuple_for_each<std::tuple_size<Tup>::value,Tup>::print(os, &tup_, 0);
			}

			template <class R, class... H>
			BOOST_LEAF_CONSTEXPR R handle_error( error_id, H && ... ) const;

			template <class R, class... H>
			BOOST_LEAF_CONSTEXPR R handle_error( error_id, H && ... );

			template <class TryBlock, class... H>
			decltype(std::declval<TryBlock>()()) try_catch_( TryBlock &&, H && ... );
		};

		template <class... E>
		struct catch_requested;

		template <>
		struct catch_requested<>
		{
			constexpr static bool value = false;
		};

		template <class Car, class... Cdr>
		struct catch_requested<Car, Cdr...>
		{
			constexpr static bool value = handler_argument_traits<Car>::requires_catch || catch_requested<Cdr...>::value;
		};

		template <bool CatchRequested, class... E>
		struct select_context_base_impl;

		template <class...>
		class nocatch_context;

		template <class... E>
		struct select_context_base_impl<false, E...>
		{
			using type = nocatch_context<E...>;
		};

		template <class...>
		class catch_context;

		template <class... E>
		struct select_context_base_impl<true, E...>
		{
			using type = catch_context<E...>;
		};

		template <class... E>
		using select_context_base = typename select_context_base_impl<catch_requested<E...>::value, E...>::type;
	}

	template <class... E>
	class context: public leaf_detail::select_context_base<E...>
	{
	};

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class HandlerL>
		struct handler_args_impl;

		template <template <class...> class L, class... H>
		struct handler_args_impl<L<H...>>
		{
			using type = leaf_detail_mp11::mp_append<fn_mp_args<H>...>;
		};

		template <class HandlerL>
		using handler_args = typename handler_args_impl<HandlerL>::type;

		template <class TypeList>
		struct deduce_context_impl;

		template <template <class...> class L, class... E>
		struct deduce_context_impl<L<E...>>
		{
			using type = context<E...>;
		};

		template <class TypeList>
		using deduce_context = typename deduce_context_impl<TypeList>::type;

		template <class H>
		struct fn_mp_args_fwd
		{
			using type = fn_mp_args<H>;
		};

		template <class... H>
		struct fn_mp_args_fwd<std::tuple<H...> &>: fn_mp_args_fwd<std::tuple<H...>> { };

		template <class... H>
		struct fn_mp_args_fwd<std::tuple<H...>>
		{
			using type = leaf_detail_mp11::mp_append<typename fn_mp_args_fwd<H>::type...>;
		};

		template <class... H>
		struct context_type_from_handlers_impl
		{
			using type = deduce_context<leaf_detail_mp11::mp_append<typename fn_mp_args_fwd<H>::type...>>;
		};

		template <class Ctx>
		struct polymorphic_context_impl: polymorphic_context, Ctx
		{
			error_id propagate_captured_errors() noexcept final override { return Ctx::propagate_captured_errors(captured_id_); }
			void activate() noexcept final override { Ctx::activate(); }
			void deactivate() noexcept final override { Ctx::deactivate(); }
			void propagate() noexcept final override { Ctx::propagate(); }
			bool is_active() const noexcept final override { return Ctx::is_active(); }
			void print( std::ostream & os ) const final override { return Ctx::print(os); }
		};
	}

	template <class... H>
	using context_type_from_handlers = typename leaf_detail::context_type_from_handlers_impl<H...>::type;

	////////////////////////////////////////////

	template <class...  H>
	BOOST_LEAF_CONSTEXPR inline context_type_from_handlers<H...> make_context() noexcept
	{
		return { };
	}

	template <class...  H>
	BOOST_LEAF_CONSTEXPR inline context_type_from_handlers<H...> make_context( H && ... ) noexcept
	{
		return { };
	}

	////////////////////////////////////////////

	template <class...  H>
	inline context_ptr make_shared_context() noexcept
	{
		return std::make_shared<leaf_detail::polymorphic_context_impl<context_type_from_handlers<H...>>>();
	}

	template <class...  H>
	inline context_ptr make_shared_context( H && ... ) noexcept
	{
		return std::make_shared<leaf_detail::polymorphic_context_impl<context_type_from_handlers<H...>>>();
	}

} }

#endif
