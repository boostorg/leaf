#ifndef BOOST_LEAF_3BAB50A2B87E11E89EEB30600C39171A
#define BOOST_LEAF_3BAB50A2B87E11E89EEB30600C39171A

// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <ostream>
#include <cstring>
#include <cassert>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <int N>
		inline char const * check_prefix( char const * t, char const (&prefix)[N] )
		{
			return std::strncmp(t,prefix,sizeof(prefix)-1)==0 ? t+sizeof(prefix)-1 : t;
		}
	}

	template <class Name>
	char const * type() noexcept
	{
	char const * t =
#ifdef __FUNCSIG__
		__FUNCSIG__;
#else
		__PRETTY_FUNCTION__;
#endif
#if defined(__clang__)
		assert(leaf_detail::check_prefix(t,"const char *boost::leaf::type() ")==t+32);
		return t+32;
#elif defined(__GNUC__)
		assert(leaf_detail::check_prefix(t,"const char* boost::leaf::type() ")==t+32);
		return t+32;
#else
		char const * clang_style = leaf_detail::check_prefix(t,"const char *boost::leaf::type() ");
		if( clang_style!=t )
			return clang_style;
		char const * gcc_style = leaf_detail::check_prefix(t,"const char* boost::leaf::type() ");
		if( gcc_style!=t )
			return gcc_style;
#endif
		return t;
	}

	namespace leaf_detail
	{
		template <class T, class E = void>
		struct is_printable: std::false_type
		{
		};

		template <class T>
		struct is_printable<T, decltype(std::declval<std::ostream&>()<<std::declval<T const &>(), void())>: std::true_type
		{
		};

		////////////////////////////////////////

		template <class T, class E = void>
		struct has_printable_member_value: std::false_type
		{
		};

		template <class T>
		struct has_printable_member_value<T, decltype(std::declval<std::ostream&>()<<std::declval<T const &>().value, void())>: std::true_type
		{
		};

		////////////////////////////////////////

		template <class Wrapper, bool WrapperPrintable=is_printable<Wrapper>::value, bool ValuePrintable=has_printable_member_value<Wrapper>::value>
		struct diagnostic;

		template <class Wrapper, bool ValuePrintable>
		struct diagnostic<Wrapper, true, ValuePrintable>
		{
			static constexpr bool is_invisible = false;
			static void print( std::ostream & os, Wrapper const & x )
			{
				os << x;
			}
		};

		template <class Wrapper>
		struct diagnostic<Wrapper, false, true>
		{
			static constexpr bool is_invisible = false;
			static void print( std::ostream & os, Wrapper const & x )
			{
				os << type<Wrapper>() << ": " << x.value;
			}
		};

		template <class Wrapper>
		struct diagnostic<Wrapper, false, false>
		{
			static constexpr bool is_invisible = false;
			static void print( std::ostream & os, Wrapper const & )
			{
				os << type<Wrapper>() << ": {Non-Printable}";
			}
		};

		template <>
		struct diagnostic<std::exception_ptr, false, false>
		{
			static constexpr bool is_invisible = true;
			static void print( std::ostream &, std::exception_ptr const & )
			{
			}
		};
	} // leaf_detail

} }

#endif
