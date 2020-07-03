#ifndef BOOST_LEAF_DETAIL_HANDLER_ARGUMENT_TRAITS_HPP_INCLUDED
#define BOOST_LEAF_DETAIL_HANDLER_ARGUMENT_TRAITS_HPP_INCLUDED

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

#include <boost/leaf/config.hpp>
#include <boost/leaf/detail/mp11.hpp>
#include <utility>
#include <exception>

namespace boost { namespace leaf {

	class error_info;
	class diagnostic_info;
	class verbose_diagnostic_info;

	////////////////////////////////////////

	namespace leaf_detail
	{
		struct diagnostic_info_;
		struct verbose_diagnostic_info_;

#if BOOST_LEAF_DIAGNOSTICS
		struct e_unexpected_count;
		struct e_unexpected_info;
#endif

		template <class T>
		using is_predicate_impl = decltype(T::evaluate(std::declval<T>().matched));

		template <class T>
		struct is_predicate
		{
			constexpr static bool value = leaf_detail_mp11::mp_valid<is_predicate_impl, typename std::decay<T>::type>::value;
		};

		template <class T>
		struct is_exception: std::is_base_of<std::exception, typename std::decay<T>::type>
		{
		};

		template <class E>
		struct handler_argument_traits;

		template <class E, bool RequiresCatch = is_exception<E>::value, bool IsPredicate = is_predicate<E>::value>
		struct handler_argument_traits_defaults;

		template <class E, bool RequiresCatch>
		struct handler_argument_traits_defaults<E, RequiresCatch, false>
		{
			using error_type = typename std::decay<E>::type;
			constexpr static bool requires_catch = RequiresCatch;
			constexpr static bool always_available = false;

			template <class Tup>
			BOOST_LEAF_CONSTEXPR static error_type const * check( Tup const &, error_info const & ) noexcept;

			template <class Tup>
			BOOST_LEAF_CONSTEXPR static error_type * check( Tup &, error_info const & ) noexcept;

			template <class Tup>
			BOOST_LEAF_CONSTEXPR static E get( Tup & tup, error_info const & ei ) noexcept
			{
				return *check(tup, ei);
			}

			static_assert(!std::is_same<E, error_info>::value, "Handlers must take leaf::error_info arguments by const &");
			static_assert(!std::is_same<E, diagnostic_info>::value, "Handlers must take leaf::diagnostic_info arguments by const &");
			static_assert(!std::is_same<E, verbose_diagnostic_info>::value, "Handlers must take leaf::verbose_diagnostic_info arguments by const &");
		};

		template <class Pred>
		struct handler_argument_traits_defaults<Pred, false, true>: handler_argument_traits<typename std::decay<decltype(std::declval<typename std::decay<Pred>::type>().matched)>::type>
		{
			static_assert(!std::is_reference<Pred>::value && !std::is_pointer<Pred>::value, "Handlers must take Pred arguments by value");

			using base = handler_argument_traits<typename std::decay<decltype(std::declval<typename std::decay<Pred>::type>().matched)>::type>;
			static_assert(!base::always_available, "Predicates can't use types that are always_available");

			template <class Tup>
			BOOST_LEAF_CONSTEXPR static bool check( Tup & tup, error_info const & ei ) noexcept
			{
				auto e = base::check(tup, ei);
				return e && Pred::evaluate(*e);
			};

			template <class Tup>
			BOOST_LEAF_CONSTEXPR static Pred get( Tup const & tup, error_info const & ei ) noexcept
			{
				return Pred{*base::check(tup, ei)};
			}
		};

		template <class E>
		struct handler_argument_always_available
		{
			using error_type = E;
			constexpr static bool requires_catch = false;
			constexpr static bool always_available = true;

			template <class Tup>
			BOOST_LEAF_CONSTEXPR static bool check( Tup &, error_info const & ) noexcept
			{
				return true;
			};
		};

		template <class E>
		struct handler_argument_traits: handler_argument_traits_defaults<E>
		{
		};

		template <class E>
		struct handler_argument_traits<E &&>
		{
			static_assert(sizeof(E) == 0, "Error handlers may not take rvalue ref arguments");
		};

		template <class E>
		struct handler_argument_traits<E *>: handler_argument_always_available<typename std::remove_const<E>::type>
		{
			template <class Tup>
			BOOST_LEAF_CONSTEXPR static E * get( Tup & tup, error_info const & ei) noexcept
			{
				return handler_argument_traits_defaults<E>::check(tup, ei);
			}
		};

		template <>
		struct handler_argument_traits<error_info const &>: handler_argument_always_available<void>
		{
			template <class Tup>
			BOOST_LEAF_CONSTEXPR static error_info const & get( Tup const &, error_info const & ei ) noexcept
			{
				return ei;
			}
		};

		template <>
#if BOOST_LEAF_DIAGNOSTICS
		struct handler_argument_traits<diagnostic_info const &>: handler_argument_always_available<e_unexpected_count>
#else
		struct handler_argument_traits<diagnostic_info const &>: handler_argument_always_available<void>
#endif
		{
			template <class Tup>
			BOOST_LEAF_CONSTEXPR static diagnostic_info_ get( Tup const & tup, error_info const & ei ) noexcept;
		};

		template <>
#if BOOST_LEAF_DIAGNOSTICS
		struct handler_argument_traits<verbose_diagnostic_info const &>: handler_argument_always_available<e_unexpected_info>
#else
		struct handler_argument_traits<verbose_diagnostic_info const &>: handler_argument_always_available<void>
#endif
		{
			template <class Tup>
			BOOST_LEAF_CONSTEXPR static verbose_diagnostic_info_ get( Tup const & tup, error_info const & ei ) noexcept;
		};

		template <class E>
		struct handler_argument_traits_require_by_value
		{
			static_assert(sizeof(E) == 0, "Error handlers must take this type by value");
		};
	}

} }

#endif
