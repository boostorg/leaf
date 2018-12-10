//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_3BAB50A2B87E11E89EEB30600C39171A
#define UUID_3BAB50A2B87E11E89EEB30600C39171A

#include <ostream>
#include <cstring>

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
		struct is_printable
		{
			static constexpr bool value=false;
		};

		template <class T>
		struct is_printable<T, decltype(std::declval<std::ostream&>()<<std::declval<T const &>(), void())>
		{
			static constexpr bool value=true;
		};

		////////////////////////////////////////

		template <class T, class E = void>
		struct has_printable_member_value
		{
			static constexpr bool value=false;
		};

		template <class T>
		struct has_printable_member_value<T, decltype(std::declval<std::ostream&>()<<std::declval<T const &>().value, void())>
		{
			static constexpr bool value=true;
		};

		////////////////////////////////////////

		template <class Wrapper, bool WrapperPrintable=is_printable<Wrapper>::value, bool ValuePrintable=has_printable_member_value<Wrapper>::value>
		struct diagnostic;

		template <class Wrapper, bool ValuePrintable>
		struct diagnostic<Wrapper, true, ValuePrintable>
		{
			static bool print( std::ostream & os, Wrapper const & x )
			{
				os << x;
				return true;
			}
		};

		template <class Wrapper>
		struct diagnostic<Wrapper, false, true>
		{
			static bool print( std::ostream & os, Wrapper const & x )
			{
				os << type<Wrapper>() << ": " << x.value;
				return true;
			}
		};

		template <class Wrapper>
		struct diagnostic<Wrapper, false, false>
		{
			static bool print( std::ostream & os, Wrapper const & )
			{
				os << type<Wrapper>() << ": N/A";
				return true;
			}
		};
	} //leaf_detail

} }

#endif
