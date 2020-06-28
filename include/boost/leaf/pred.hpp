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

#include <boost/leaf/handle_error.hpp>

namespace boost { namespace leaf {

	template <class ErrorCodeEnum>
	BOOST_LEAF_CONSTEXPR inline bool category( std::error_code const & ec ) noexcept
	{
		static_assert(std::is_error_code_enum<ErrorCodeEnum>::value, "leaf::category requires an error code enum");
		return &ec.category() == &std::error_code(ErrorCodeEnum{}).category();
	}

	template <class E, class EnumType = E>
	struct condition
	{
		using enum_type = EnumType;
		static_assert(std::is_error_condition_enum<enum_type>::value || std::is_error_code_enum<enum_type>::value, "leaf::condition requires the enum to be registered either with std::is_error_condition or std::is_error_code.");
	};

	namespace leaf_detail
	{
		template <class T> using has_member_value_impl = decltype( std::declval<T>().value );
		template <class T> using has_member_value_fn_impl = decltype( std::declval<T>().value() );

		template <class T>
		struct has_member_value
		{
			enum { value = leaf_detail_mp11::mp_valid<has_member_value_impl, T>::value || leaf_detail_mp11::mp_valid<has_member_value_fn_impl, T>::value };
		};

		template <class Enum>
		struct match_traits<Enum, false>
		{
			using enum_type = Enum;
			using error_type = Enum;
			using match_type = Enum;

			BOOST_LEAF_CONSTEXPR static enum_type get_value( match_type x ) noexcept
			{
				return x;
			}
		};

		template <class E, bool = leaf_detail_mp11::mp_valid<has_member_value_fn_impl, E>::value>
		struct match_traits_value;

		template <class E>
		struct match_traits_value<E, false>
		{
			using enum_type = decltype(std::declval<E>().value);
			using error_type = E;
			using match_type = error_type const &;

			BOOST_LEAF_CONSTEXPR static enum_type const & get_value( match_type x ) noexcept
			{
				return x.value;
			}
		};

		template <class E>
		struct match_traits_value<E, true>
		{
			using enum_type = typename std::remove_reference<decltype(std::declval<E>().value())>::type;
			using error_type = E;
			using match_type = error_type const &;

			BOOST_LEAF_CONSTEXPR static decltype(std::declval<match_type>().value()) get_value( match_type x ) noexcept
			{
				return x.value();
			}
		};

		template <class E>
		struct match_traits<E, true>: match_traits_value<E>
		{
		};

		template <class EnumType>
		struct match_traits<condition<EnumType, EnumType>, false>
		{
			using enum_type = EnumType;
			using error_type = std::error_code;
			using match_type = error_type const &;

			BOOST_LEAF_CONSTEXPR static match_type get_value( match_type x ) noexcept
			{
				return x;
			}
		};

		template <class E, class EnumType>
		struct match_traits<condition<E, EnumType>, false>
		{
			using enum_type = EnumType;
			using error_type = E;
			using match_type = error_type const &;

			BOOST_LEAF_CONSTEXPR static std::error_code const & get_value( match_type x ) noexcept
			{
				return x.value;
			}
		};

		template <>
		struct match_traits<std::error_code, true>
		{
			using enum_type = void;
			using error_type = std::error_code;
			using match_type = error_type const &;

			BOOST_LEAF_CONSTEXPR static match_type get_value( match_type x ) noexcept
			{
				return x;
			}
		};

		// Use match<std::error_code, ...> with an error condition enum to match a specific error condition.
		// This type intentionally left undefined to detect such bugs.
		template <>
		struct match_traits<std::error_condition, true>;

		template <class MatchType>
		inline bool check_value_pack( MatchType const & x, bool (*pred)(MatchType const &) noexcept ) noexcept
		{
			BOOST_LEAF_ASSERT(pred != 0);
			return pred(x);
		}

		template <class MatchType, class V>
		inline bool check_value_pack( MatchType const & x, V v ) noexcept
		{
			return x == v;
		}

		template <class MatchType, class VCar, class... VCdr>
		inline bool check_value_pack( MatchType const & x, VCar car, VCdr ... cdr ) noexcept
		{
			return check_value_pack(x, car) || check_value_pack(x, cdr...);
		}

		template <class MatchType>
		struct predicate
		{
		public:

			using match_type = MatchType;

		protected:

			match_type m_;

			BOOST_LEAF_CONSTEXPR explicit predicate( match_type m ) noexcept:
				m_(m)
			{
			}

		public:

			BOOST_LEAF_CONSTEXPR match_type matched() const noexcept
			{
				return m_;
			}
		};

	}

#if __cplusplus >= 201703L
	template <class E, auto V1, auto... V>
#else
	template <class E, typename leaf_detail::match_traits<E>::enum_type V1, typename leaf_detail::match_traits<E>::enum_type... V>
#endif
	struct match: leaf_detail::predicate<typename leaf_detail::match_traits<E>::match_type>
	{
		using base = leaf_detail::predicate<typename leaf_detail::match_traits<E>::match_type>;

		BOOST_LEAF_CONSTEXPR explicit match( typename base::match_type m ) noexcept:
			base(m)
		{
		}

		BOOST_LEAF_CONSTEXPR explicit operator bool() const noexcept
		{
			return leaf_detail::check_value_pack(leaf_detail::match_traits<E>::get_value(this->m_), V1, V...);
		}
	};

#if __cplusplus >= 201703L
	template <class T, class E, T E::* P, auto V1, auto... V>
	struct member_eq<P, V1, V...>: leaf_detail::predicate<E const &>
	{
		using base = leaf_detail::predicate<E const &>;

		BOOST_LEAF_CONSTEXPR explicit member_eq( typename base::match_type m ) noexcept:
			base(m)
		{
		}

		BOOST_LEAF_CONSTEXPR explicit operator bool() const noexcept
		{
			return leaf_detail::check_value_pack(this->m_.*P, V1, V...);
		}
	};

	template <class T, class E, T (E::* P)(), auto V1, auto... V>
	struct member_eq<P, V1, V...>: leaf_detail::predicate<E const &>
	{
		using base = leaf_detail::predicate<E const &>;

		BOOST_LEAF_CONSTEXPR explicit member_eq( typename base::match_type m ) noexcept:
			base(m)
		{
		}

		BOOST_LEAF_CONSTEXPR explicit operator bool() const noexcept
		{
			return leaf_detail::check_value_pack((this->m_.*P)(), V1, V...);
		}
	};
#endif
} }

// Boost Exception Integration below

namespace boost { class exception; }
namespace boost { namespace exception_detail { template <class ErrorInfo> struct get_info; } }

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <class Tag, class T>
		struct match_traits_value<boost::error_info<Tag, T>, true>
		{
			using error_type = boost::error_info<Tag, T>;
			using enum_type = T;
			using match_type = T;

			BOOST_LEAF_CONSTEXPR static T get_value( match_type x ) noexcept
			{
				return x;
			}
		};

		template <class, class T> struct dependent_type { using type = T; };
		template <class Dep, class T> using dependent_type_t = typename dependent_type<Dep, T>::type;

		template <class Tag, class T>
		template <class Tup>
		BOOST_LEAF_CONSTEXPR inline T * handler_argument_traits<boost::error_info<Tag, T>>::check( Tup &, error_info const & ei ) noexcept
		{
			using boost_exception = dependent_type_t<T, boost::exception>;
			if( ei.exception_caught() )
				if( boost_exception * be = dynamic_cast<boost_exception *>(ei.exception()) )
					return exception_detail::get_info<boost::error_info<Tag, T>>::get(*be);
			return 0;
		}

		template <class Tag, class T>
		template <class Tup>
		BOOST_LEAF_CONSTEXPR inline boost::error_info<Tag, T> handler_argument_traits<boost::error_info<Tag, T>>::get( Tup const & tup, error_info const & ei ) noexcept
		{
			T * x = check(tup, ei);
			BOOST_LEAF_ASSERT(x != 0);
			return boost::error_info<Tag, T>(*x);
		}
	}

} }

#endif
