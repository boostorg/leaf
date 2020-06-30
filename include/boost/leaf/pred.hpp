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

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <class MatchType>
		inline bool cmp_value_pack( MatchType const & x, bool (*pred)(MatchType const &) noexcept ) noexcept
		{
			BOOST_LEAF_ASSERT(pred != 0);
			return pred(x);
		}

		template <class MatchType, class V>
		inline bool cmp_value_pack( MatchType const & x, V v ) noexcept
		{
			return x == v;
		}

		template <class MatchType, class VCar, class... VCdr>
		inline bool cmp_value_pack( MatchType const & x, VCar car, VCdr ... cdr ) noexcept
		{
			return cmp_value_pack(x, car) || cmp_value_pack(x, cdr...);
		}

		template <class MatchType>
		struct pred
		{
		public:

			using match_type = MatchType;

		protected:

			match_type m_;

			BOOST_LEAF_CONSTEXPR explicit pred( match_type m ) noexcept:
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

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class E>
		struct match_traits
		{
			using enum_type = E;
			using error_type = E;
			using match_type = error_type const &;
		};
	}

	template <class E, BOOST_LEAF_MATCH_ARGS(match_traits, V1, V)>
	struct match: leaf_detail::pred<typename leaf_detail::match_traits<E>::match_type>
	{
		using base = leaf_detail::pred<typename leaf_detail::match_traits<E>::match_type>;

		BOOST_LEAF_CONSTEXPR explicit match( typename base::match_type m ) noexcept:
			base(m)
		{
		}

		BOOST_LEAF_CONSTEXPR explicit operator bool() const noexcept
		{
			return leaf_detail::cmp_value_pack(this->m_, V1, V...);
		}
	};

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class E>
		struct match_value_traits
		{
			using enum_type = typename std::remove_reference<decltype(std::declval<E>().value)>::type;
			using error_type = E;
			using match_type = error_type const &;
		};
	}

	template <class E, BOOST_LEAF_MATCH_ARGS(match_value_traits, V1, V)>
	struct match_value: leaf_detail::pred<typename leaf_detail::match_value_traits<E>::match_type>
	{
		using base = leaf_detail::pred<typename leaf_detail::match_value_traits<E>::match_type>;

		BOOST_LEAF_CONSTEXPR explicit match_value( typename base::match_type m ) noexcept:
			base(m)
		{
		}

		BOOST_LEAF_CONSTEXPR explicit operator bool() const noexcept
		{
			return leaf_detail::cmp_value_pack(this->m_.value, V1, V...);
		}
	};

	////////////////////////////////////////

#if __cplusplus >= 201703L

	template <class T, class E, T E::* P, auto V1, auto... V>
	struct match_member<P, V1, V...>: leaf_detail::pred<E const &>
	{
		using base = leaf_detail::pred<E const &>;

		BOOST_LEAF_CONSTEXPR explicit match_member( typename base::match_type m ) noexcept:
			base(m)
		{
		}

		BOOST_LEAF_CONSTEXPR explicit operator bool() const noexcept
		{
			return leaf_detail::cmp_value_pack(this->m_.*P, V1, V...);
		}
	};

#endif

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

	namespace leaf_detail
	{
		template <class Enum>
		struct match_traits<condition<Enum, Enum>>
		{
			using enum_type = Enum;
			using error_type = std::error_code;
			using match_type = error_type const &;
		};

		template <class E, class Enum>
		struct match_traits<condition<E, Enum>>
		{
			static_assert(sizeof(Enum)==0, "leaf::condition<E, Enum> should be used with leaf::match_value<>, not with leaf::match<>");
		};

		template <class E, class Enum>
		struct match_value_traits<condition<E, Enum>>
		{
			using enum_type = Enum;
			using error_type = E;
			using match_type = error_type const &;
		};

		template <class Enum>
		struct match_value_traits<condition<Enum, Enum>>
		{
			static_assert(sizeof(Enum)==0, "leaf::condition<Enum> should be used with leaf::match<>, not with leaf::match_value<>");
		};
	}

	////////////////////////////////////////

	template <class ErrorCodeEnum>
	BOOST_LEAF_CONSTEXPR inline bool category( std::error_code const & ec ) noexcept
	{
		static_assert(std::is_error_code_enum<ErrorCodeEnum>::value, "leaf::category requires an error code enum");
		return &ec.category() == &std::error_code(ErrorCodeEnum{}).category();
	}

} }

// Boost Exception Integration

namespace boost { class exception; }
namespace boost { namespace exception_detail { template <class ErrorInfo> struct get_info; } }

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <class Ex>
		BOOST_LEAF_CONSTEXPR Ex * get_exception( error_info const & );

		template <class Tag, class T>
		struct match_traits<boost::error_info<Tag, T>>
		{
			using enum_type = T;
			using error_type = boost::error_info<Tag, T>;
			using match_type = T;
		};

		template <class, class T>
		struct dependent_type { using type = T; };

		template <class Dep, class T>
		using dependent_type_t = typename dependent_type<Dep, T>::type;

		template <class Tag, class T>
		template <class Tup>
		BOOST_LEAF_CONSTEXPR inline
		T *
		handler_argument_traits<boost::error_info<Tag, T>>::
		check( Tup &, error_info const & ei ) noexcept
		{
			using boost_exception = dependent_type_t<T, boost::exception>;
			if( auto * be = get_exception<boost_exception>(ei) )
				return exception_detail::get_info<boost::error_info<Tag, T>>::get(*be);
			else
				return 0;
		}

		template <class Tag, class T>
		template <class Tup>
		BOOST_LEAF_CONSTEXPR inline
		boost::error_info<Tag, T>
		handler_argument_traits<boost::error_info<Tag, T>>::
		get( Tup const & tup, error_info const & ei ) noexcept
		{
			return boost::error_info<Tag, T>(*check(tup, ei));
		}
	}

} }

#endif
