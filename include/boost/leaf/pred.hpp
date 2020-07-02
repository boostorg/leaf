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

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <class MatchType>
		class pred
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

	template <class ErrorCodeEnum>
	BOOST_LEAF_CONSTEXPR inline bool category( std::error_code const & ec ) noexcept
	{
		static_assert(std::is_error_code_enum<ErrorCodeEnum>::value, "leaf::category requires an error code enum");
		return &ec.category() == &std::error_code(ErrorCodeEnum{}).category();
	}

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
			static_assert(sizeof(Enum)==0, "leaf::condition<E, Enum> should be used with leaf::match_value<>, not with leaf::match<>");
		};
	}

	template <class E, BOOST_LEAF_MATCH_ARGS(match_enum_type<E>, V1, V)>
	struct match: leaf_detail::pred<E const &>
	{
		using error_type = E;

		BOOST_LEAF_CONSTEXPR explicit match(E const * e) noexcept:
			leaf_detail::pred<E const &>(*e)
		{
		}

		BOOST_LEAF_CONSTEXPR static bool evaluate(E const * e)
		{
			return e && leaf_detail::cmp_value_pack(*e, V1, V...);
		}
	};

#define BOOST_LEAF_ESC(...) __VA_ARGS__
	template <class Enum, BOOST_LEAF_MATCH_ARGS(BOOST_LEAF_ESC(match_enum_type<condition<Enum, Enum>>), V1, V)>
	struct match<condition<Enum, Enum>, V1, V...>: leaf_detail::pred<std::error_code const &>
	{
		using error_type = std::error_code;

		BOOST_LEAF_CONSTEXPR explicit match(std::error_code const * e) noexcept:
			leaf_detail::pred<std::error_code const &>(*e)
		{
		}

		BOOST_LEAF_CONSTEXPR static bool evaluate(std::error_code const * e)
		{
			return e && leaf_detail::cmp_value_pack(*e, V1, V...);
		}
	};
#undef BOOST_LEAF_ESC

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
	struct match_value: leaf_detail::pred<E const &>
	{
		using error_type = E;

		BOOST_LEAF_CONSTEXPR explicit match_value(E const * e) noexcept:
			leaf_detail::pred<E const &>(*e)
		{
		}

		BOOST_LEAF_CONSTEXPR static bool evaluate(E const * e)
		{
			return e && leaf_detail::cmp_value_pack(e->value, V1, V...);
		}
	};


#define BOOST_LEAF_ESC(...) __VA_ARGS__
	template <class E, class Enum, BOOST_LEAF_MATCH_ARGS(BOOST_LEAF_ESC(match_value_enum_type<condition<E, Enum>>), V1, V)>
	struct match_value<condition<E, Enum>, V1, V...>: leaf_detail::pred<E const &>
	{
		using error_type = E;

		BOOST_LEAF_CONSTEXPR explicit match_value(E const * e) noexcept:
			leaf_detail::pred<E const &>(*e)
		{
		}

		BOOST_LEAF_CONSTEXPR static bool evaluate(E const * e)
		{
			return e && leaf_detail::cmp_value_pack(e->value, V1, V...);
		}
	};
#undef BOOST_LEAF_ESC

	////////////////////////////////////////

#if __cplusplus >= 201703L

	template <class T, class E, T E::* P, auto V1, auto... V>
	struct match_member<P, V1, V...>: leaf_detail::pred<E const &>
	{
		using error_type = E;

		BOOST_LEAF_CONSTEXPR explicit match_member(E const * e) noexcept:
			leaf_detail::pred<E const &>(*e)
		{
		}

		BOOST_LEAF_CONSTEXPR static bool evaluate(E const * e)
		{
			return e && leaf_detail::cmp_value_pack(e->*P, V1, V...);
		}
	};

#endif

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class Ex>
		BOOST_LEAF_CONSTEXPR inline bool check_exception_pack( std::exception const * ex, Ex const * ) noexcept
		{
			return dynamic_cast<Ex const *>(ex)!=0;
		}

		template <class Ex, class... ExRest>
		BOOST_LEAF_CONSTEXPR inline bool check_exception_pack( std::exception const * ex, Ex const *, ExRest const * ... ex_rest ) noexcept
		{
			return dynamic_cast<Ex const *>(ex)!=0 || check_exception_pack(ex, ex_rest...);
		}

		BOOST_LEAF_CONSTEXPR inline bool check_exception_pack( std::exception const * )
		{
			return true;
		}
	}

	template <class... Ex>
	struct catch_: leaf_detail::pred<std::exception const &>
	{
		using error_type = void;

		BOOST_LEAF_CONSTEXPR explicit catch_(std::exception const * ex) noexcept:
			leaf_detail::pred<std::exception const &>(*ex)
		{
		}

		BOOST_LEAF_CONSTEXPR static bool evaluate(std::exception const * ex)
		{
			return ex && leaf_detail::check_exception_pack(ex, static_cast<Ex const *>(0)...);
		}
	};

	template <class Ex>
	struct catch_<Ex>: leaf_detail::pred<Ex const &>
	{
		using error_type = void;

		BOOST_LEAF_CONSTEXPR explicit catch_(std::exception const * ex) noexcept:
			leaf_detail::pred<Ex const &>(*dynamic_cast<Ex const *>(ex))
		{
		}

		BOOST_LEAF_CONSTEXPR static bool evaluate(std::exception const * ex)
		{
			return dynamic_cast<Ex const *>(ex) != 0;
		}
	};
} }

// Boost Exception Integration

namespace boost { class exception; }
namespace boost { namespace exception_detail { template <class ErrorInfo> struct get_info; } }

namespace boost { namespace leaf {

	template <class Tag, class T, T V1, T... V>
	struct match<boost::error_info<Tag, T>, V1, V...>: leaf_detail::pred<T>
	{
		using error_type = boost::error_info<Tag, T>;

		BOOST_LEAF_CONSTEXPR explicit match(T const * x) noexcept:
			leaf_detail::pred<T>(*x)
		{
		}

		BOOST_LEAF_CONSTEXPR static bool evaluate(T const * x)
		{
			return x && leaf_detail::cmp_value_pack(*x, V1, V...);
		}
	};

	namespace leaf_detail
	{
		template <class Ex>
		BOOST_LEAF_CONSTEXPR Ex * get_exception( error_info const & );

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
