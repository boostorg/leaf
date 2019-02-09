#ifndef BOOST_LEAF_E72958AC28C711E9998B3465EBB1FB94
#define BOOST_LEAF_E72958AC28C711E9998B3465EBB1FB94

// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/error.hpp>
#include <thread>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <int I, class Tuple>
		struct tuple_for_each
		{
			static void activate( Tuple & tup ) noexcept
			{
				tuple_for_each<I-1,Tuple>::activate(tup);
				std::get<I-1>(tup).activate();
			}

			static void deactivate( Tuple & tup, bool propagate_errors ) noexcept
			{
				std::get<I-1>(tup).deactivate(propagate_errors);
				tuple_for_each<I-1,Tuple>::deactivate(tup, propagate_errors);
			}

			static void print( std::ostream & os, Tuple const & tup ) noexcept
			{
				tuple_for_each<I-1,Tuple>::print(os,tup);
				std::get<I-1>(tup).print(os);
			}
		};

		template <class Tuple>
		struct tuple_for_each<0, Tuple>
		{
			static void activate( Tuple & ) noexcept { }
			static void deactivate( Tuple &, bool ) noexcept { }
			static void print( std::ostream &, Tuple const & ) noexcept { }
		};
	}

	////////////////////////////////////////////

	namespace leaf_detail
	{
		class e_unexpected_count;
		class e_unexpected_info;

		template <class L>
		struct unexpected_requested;

		template <template <class ...> class L>
		struct unexpected_requested<L<>>
		{
			constexpr static bool value = false;
		};

		template <template <class...> class L>
		struct unexpected_requested<L<e_unexpected_count>>
		{
			constexpr static bool value = true;
		};

		template <template <class...> class L>
		struct unexpected_requested<L<e_unexpected_info>>
		{
			constexpr static bool value = true;
		};

		template <template <class...> class L, class Car, class... Cdr>
		struct unexpected_requested<L<Car,Cdr...>>
		{
			constexpr static bool value = unexpected_requested<L<Cdr...>>::value;
		};
	}

	////////////////////////////////////////////

	class error_info;
	class diagnostic_info;
	class verbose_diagnostic_info;

	namespace leaf_detail
	{
		template <class T> struct translate_type_impl { using type = typename std::decay<T>::type; };

		template <> struct translate_type_impl<diagnostic_info>;
		template <> struct translate_type_impl<diagnostic_info const>;
		template <> struct translate_type_impl<diagnostic_info const *>;
		template <> struct translate_type_impl<diagnostic_info const &> { using type = e_unexpected_count; };

		template <> struct translate_type_impl<verbose_diagnostic_info>;
		template <> struct translate_type_impl<verbose_diagnostic_info const>;
		template <> struct translate_type_impl<verbose_diagnostic_info const *>;
		template <> struct translate_type_impl<verbose_diagnostic_info const &> { using type = e_unexpected_info; };

		struct e_original_ec;
		template <> struct translate_type_impl<std::error_code> { using type = e_original_ec; };
		template <> struct translate_type_impl<std::error_code const> { using type = e_original_ec; };
		template <> struct translate_type_impl<std::error_code const *> { using type = e_original_ec; };
		template <> struct translate_type_impl<std::error_code const &> { using type = e_original_ec; };

		template <class T>
		using translate_type = typename translate_type_impl<T>::type;

		template <class... T>
		struct translate_list_impl;

		template <template<class...> class L, class... T>
		struct translate_list_impl<L<T...>>
		{
			using type = leaf_detail_mp11::mp_list<translate_type<T>...>;
		};

		template <class L> using translate_list = typename translate_list_impl<L>::type;

		template <class T> struct does_not_participate_in_context_deduction: std::false_type { };
		template <> struct does_not_participate_in_context_deduction<error_info>: std::true_type { };
		template <> struct does_not_participate_in_context_deduction<std::error_code>: std::true_type { };
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

		template <template<class...> class L, class... E>
		struct deduce_e_tuple_impl<L<E...>>
		{
			using type = std::tuple<slot<E>...>;
		};

		template <class L>
		using deduce_e_tuple = typename deduce_e_tuple_impl<L>::type;
	}

	////////////////////////////////////////////

	template <class... E>
	class context: public polymorphic_context
	{
		context( context const & ) = delete;
		context & operator=( context const & ) = delete;
		using Tup = leaf_detail::deduce_e_tuple<leaf_detail::transform_e_type_list<leaf_detail_mp11::mp_list<E...>>>;
		Tup tup_;

		std::thread::id thread_id_;

	public:

		context() noexcept
		{
		}

		context( context && x ) noexcept:
			tup_(std::move(x.tup_))
		{
			assert(thread_id_ == std::thread::id());
		}

		~context() noexcept
		{
			assert(thread_id_ == std::thread::id());
		}

		std::thread::id const & thread_id() const noexcept
		{
			return thread_id_;
		}

		Tup const & tup() const noexcept
		{
			return tup_;
		}

		void activate() noexcept final override
		{
			using namespace leaf_detail;
			assert(thread_id_ == std::thread::id());
			tuple_for_each<std::tuple_size<Tup>::value,Tup>::activate(tup_);
			if( unexpected_requested<Tup>::value )
				++tl_unexpected_enabled_counter();
			thread_id_ = std::this_thread::get_id();
		}

		void deactivate( bool propagate_errors ) noexcept final override
		{
			using namespace leaf_detail;
			assert(thread_id_ == std::this_thread::get_id());
			thread_id_ = std::thread::id();
			if( unexpected_requested<Tup>::value )
				--tl_unexpected_enabled_counter();
			tuple_for_each<std::tuple_size<Tup>::value,Tup>::deactivate(tup_, propagate_errors);
		}

		void print( std::ostream & os ) final override
		{
			leaf_detail::tuple_for_each<std::tuple_size<Tup>::value,Tup>::print(os, tup_);
		}
	};

} }

#endif
