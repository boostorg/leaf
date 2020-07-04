#ifndef BOOST_LEAF_PRED_HPP_INCLUDED
#define BOOST_LEAF_PRED_HPP_INCLUDED

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

#include <boost/leaf/detail/handler_argument_traits.hpp>
#include <system_error>

#if __cplusplus >= 201703L
#	define BOOST_LEAF_MATCH_ARGS(et,v1,v) auto v1, auto... v
#else
#	define BOOST_LEAF_MATCH_ARGS(et,v1,v) typename leaf_detail::et::type v1, typename leaf_detail::et::type... v
#endif
#define BOOST_LEAF_ESC(...) __VA_ARGS__

namespace boost { namespace leaf {

	namespace leaf_detail
	{
#if __cplusplus >= 201703L
		template <class MatchType, class T>
		BOOST_LEAF_CONSTEXPR BOOST_LEAF_ALWAYS_INLINE bool cmp_value_pack( MatchType const & e, bool (*P)(T) noexcept ) noexcept
		{
			BOOST_LEAF_ASSERT(P != 0);
			return P(e);
		}

		template <class MatchType, class T>
		BOOST_LEAF_CONSTEXPR BOOST_LEAF_ALWAYS_INLINE bool cmp_value_pack( MatchType const & e, bool (*P)(T) )
		{
			BOOST_LEAF_ASSERT(P != 0);
			return P(e);
		}
#endif

		template <class MatchType, class V>
		BOOST_LEAF_CONSTEXPR BOOST_LEAF_ALWAYS_INLINE bool cmp_value_pack( MatchType const & e, V v )
		{
			return e == v;
		}

		template <class MatchType, class VCar, class... VCdr>
		BOOST_LEAF_CONSTEXPR BOOST_LEAF_ALWAYS_INLINE bool cmp_value_pack( MatchType const & e, VCar car, VCdr ... cdr )
		{
			return cmp_value_pack(e, car) || cmp_value_pack(e, cdr...);
		}
	}

	////////////////////////////////////////

	template <class E, class Enum = E>
	struct condition
	{
		static_assert(std::is_error_condition_enum<Enum>::value || std::is_error_code_enum<Enum>::value, "leaf::condition<E, Enum> requires Enum to be registered either with std::is_error_condition_enum or std::is_error_code_enum.");
	};

	template <class Enum>
	struct condition<Enum, Enum>
	{
		static_assert(std::is_error_condition_enum<Enum>::value || std::is_error_code_enum<Enum>::value, "leaf::condition<Enum> requires Enum to be registered either with std::is_error_condition_enum or std::is_error_code_enum.");
	};

#if __cplusplus >= 201703L
	template <class ErrorCodeEnum>
	BOOST_LEAF_CONSTEXPR inline bool category( std::error_code const & ec ) noexcept
	{
		static_assert(std::is_error_code_enum<ErrorCodeEnum>::value, "leaf::category requires an error code enum");
		return &ec.category() == &std::error_code(ErrorCodeEnum{}).category();
	}
#endif

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class T>
		struct match_enum_type
		{
			using type = T;
		};

		template <class Enum>
		struct match_enum_type<condition<Enum, Enum>>
		{
			using type = Enum;
		};

		template <class E, class Enum>
		struct match_enum_type<condition<E, Enum>>
		{
			static_assert(sizeof(Enum) == 0, "leaf::condition<E, Enum> should be used with leaf::match_value<>, not with leaf::match<>");
		};
	}

	template <class E, BOOST_LEAF_MATCH_ARGS(match_enum_type<E>, V1, V)>
	struct match
	{
		E matched;

		template <class T>
		BOOST_LEAF_CONSTEXPR static bool evaluate(T && x)
		{
			return leaf_detail::cmp_value_pack(std::forward<T>(x), V1, V...);
		}
	};

	template <class Enum, BOOST_LEAF_MATCH_ARGS(BOOST_LEAF_ESC(match_enum_type<condition<Enum, Enum>>), V1, V)>
	struct match<condition<Enum, Enum>, V1, V...>
	{
		std::error_code const & matched;

		BOOST_LEAF_CONSTEXPR static bool evaluate(std::error_code const & e) noexcept
		{
			return leaf_detail::cmp_value_pack(e, V1, V...);
		}
	};

	namespace leaf_detail
	{
		template <class E, BOOST_LEAF_MATCH_ARGS(match_enum_type<E>, V1, V)>
		struct is_predicate<match<E, V1, V...>>: std::true_type
		{
		};
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class E>
		struct match_value_enum_type
		{
			using type = typename std::remove_reference<decltype(std::declval<E>().value)>::type;
		};

		template <class E, class Enum>
		struct match_value_enum_type<condition<E, Enum>>
		{
			using type = Enum;
		};

		template <class Enum>
		struct match_value_enum_type<condition<Enum, Enum>>
		{
			static_assert(sizeof(Enum)==0, "leaf::condition<Enum> should be used with leaf::match<>, not with leaf::match_value<>");
		};
	}

	template <class E, BOOST_LEAF_MATCH_ARGS(match_value_enum_type<E>, V1, V)>
	struct match_value
	{
		E const & matched;

		BOOST_LEAF_CONSTEXPR static bool evaluate(E const & e) noexcept
		{
			return leaf_detail::cmp_value_pack(e.value, V1, V...);
		}
	};

	template <class E, class Enum, BOOST_LEAF_MATCH_ARGS(BOOST_LEAF_ESC(match_value_enum_type<condition<E, Enum>>), V1, V)>
	struct match_value<condition<E, Enum>, V1, V...>
	{
		E const & matched;

		BOOST_LEAF_CONSTEXPR static bool evaluate(E const & e)
		{
			return leaf_detail::cmp_value_pack(e.value, V1, V...);
		}
	};

	namespace leaf_detail
	{
		template <class E, BOOST_LEAF_MATCH_ARGS(match_value_enum_type<E>, V1, V)>
		struct is_predicate<match_value<E, V1, V...>>: std::true_type
		{
		};
	}

	////////////////////////////////////////

#if __cplusplus >= 201703L
	template <auto, auto, auto...>
	struct match_member;

	template <class T, class E, T E::* P, auto V1, auto... V>
	struct match_member<P, V1, V...>
	{
		E const & matched;

		BOOST_LEAF_CONSTEXPR static bool evaluate(E const & e) noexcept
		{
			return leaf_detail::cmp_value_pack(e.*P, V1, V...);
		}
	};

	namespace leaf_detail
	{
		template <auto P, auto V1, auto... V>
		struct is_predicate<match_member<P, V1, V...>>: std::true_type
		{
		};
	}

#endif

	////////////////////////////////////////

	template <class P>
	struct if_not
	{
		decltype(std::declval<P>().matched) matched;

		template <class E>
		BOOST_LEAF_CONSTEXPR static bool evaluate(E && e) noexcept
		{
			return !P::evaluate(std::forward<E>(e));
		}
	};

	namespace leaf_detail
	{
		template <class P>
		struct is_predicate<if_not<P>>: std::true_type
		{
		};
	}

} }

#endif
