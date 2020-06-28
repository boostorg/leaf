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

#include <boost/leaf/error.hpp>

namespace boost { namespace leaf {

	class error_info;
	class diagnostic_info;
	class verbose_diagnostic_info;

	namespace leaf_detail
	{
		struct diagnostic_info_;
		struct verbose_diagnostic_info_;

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
		struct handler_argument_traits<A &&>
		{
			static_assert(sizeof(A) == 0, "Error handlers may not take rvalue ref arguments");
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
		struct handler_argument_traits<diagnostic_info const &>: handler_argument_always_available<e_unexpected_count>
		{
			template <class Tup>
			BOOST_LEAF_CONSTEXPR static diagnostic_info_ get( Tup const & tup, error_info const & ei ) noexcept;
		};

		template <>
		struct handler_argument_traits<verbose_diagnostic_info const &>: handler_argument_always_available<e_unexpected_info>
		{
			template <class Tup>
			BOOST_LEAF_CONSTEXPR static verbose_diagnostic_info_ get( Tup const & tup, error_info const & ei ) noexcept;
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
	struct match;

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

	template <auto, auto, auto...>
	struct member_eq;

	template <class T, class E, T E::* P, auto V1, auto... V>
	struct member_eq<P, V1, V...>;

	namespace leaf_detail
	{
		template <class T, class E, T E::* P, auto V1, auto... V>
		struct handler_argument_traits<member_eq<P, V1, V...>>: handler_argument_predicate<member_eq<P, V1, V...>, E>
		{
		};

		template <class T, class E, T E::* P, auto V1, auto... V>
		struct handler_argument_traits<member_eq<P, V1, V...> const &>
		{
			static_assert(sizeof(E) == 0, "Error handlers must take leaf::member_eq<> by value");
		};

		template <class T, class E, T E::* P, auto V1, auto... V>
		struct handler_argument_traits<member_eq<P, V1, V...> &>
		{
			static_assert(sizeof(E) == 0, "Error handlers must take leaf::member_eq<> by value");
		};
	}

	template <class T, class E, T (E::* P)(), auto V1, auto... V>
	struct member_eq<P, V1, V...>;

	namespace leaf_detail
	{
		template <class T, class E, T (E::* P)(), auto V1, auto... V>
		struct handler_argument_traits<member_eq<P, V1, V...>>: handler_argument_predicate<member_eq<P, V1, V...>, E>
		{
		};

		template <class T, class E, T (E::* P)(), auto V1, auto... V>
		struct handler_argument_traits<member_eq<P, V1, V...> const &>
		{
			static_assert(sizeof(E) == 0, "Error handlers must take leaf::member_eq<> by value");
		};

		template <class T, class E, T (E::* P)(), auto V1, auto... V>
		struct handler_argument_traits<member_eq<P, V1, V...> &>
		{
			static_assert(sizeof(E) == 0, "Error handlers must take leaf::member_eq<> by value");
		};
	}

#else

	template <class E, typename leaf_detail::match_traits<E>::enum_type V1, typename leaf_detail::match_traits<E>::enum_type... V>
	struct match;

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
} }

// Boost Exception Integration below

namespace boost { template <class Tag,class T> class error_info; }

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <class Tag, class T>
		struct handler_argument_traits<boost::error_info<Tag, T>>
		{
			using error_type = void;
			constexpr static bool requires_catch = true;
			constexpr static bool always_available = false;

			template <class Tup>
			BOOST_LEAF_CONSTEXPR static T * check( Tup &, error_info const & ) noexcept;

			template <class Tup>
			BOOST_LEAF_CONSTEXPR static boost::error_info<Tag, T> get( Tup const &, error_info const & ) noexcept;
		};

		template <class Tag, class T>
		struct handler_argument_traits<boost::error_info<Tag, T> const &>
		{
			static_assert(sizeof(T) == 0, "Error handlers must take boost::error_info<> by value");
		};

		template <class Tag, class T>
		struct handler_argument_traits<boost::error_info<Tag, T> &>
		{
			static_assert(sizeof(T) == 0, "Error handlers must take boost::error_info<> by value");
		};
	}

} }

#endif
