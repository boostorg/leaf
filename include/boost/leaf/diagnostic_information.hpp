//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_E4B56A60B87011E890CB55B30D39171A
#define UUID_E4B56A60B87011E890CB55B30D39171A

#include <boost/leaf/expect.hpp>
#include <boost/leaf/detail/diagnostic_print.hpp>

namespace
boost
	{
	namespace
	leaf
		{
		namespace
		leaf_detail
			{
			template <class T>
			int
			slot_print( std::ostream & os )
				{
				if( tl_slot<T>::tl_instance().diagnostic_print(os) )
					os << std::endl;
				return 42;
				}
			inline
			char const *
			get_info_function()
				{
				auto & info = tl_slot<ei_source_location<in_function>>::tl_instance();
				if( info.has_value() )
					return info.value().value;
				else
					return 0;
				}
			inline
			char const *
			get_info_file()
				{
				auto & info = tl_slot<ei_source_location<in_file>>::tl_instance();
				if( info.has_value() )
					return info.value().value;
				else
					return 0;
				}
			inline
			int
			get_info_line()
				{
				auto & info = tl_slot<ei_source_location<at_line>>::tl_instance();
				if( info.has_value() )
					return info.value().value;
				else
					return -1;
				}
#ifdef _MSC_VER
			template <int I,class Tuple>
			struct
			msvc_workaround_print
				{
				static
				void
				print( std::ostream & os )
					{
					typedef typename std::tuple_element<I-1,Tuple>::type ith_type;
					(void) slot_print<ith_type>(os);
					msvc_workaround_print<I-1,Tuple>::print(os);
					}
				};
			template <class Tuple>
			struct
			msvc_workaround_print<0,Tuple>
				{
				static void print( std::ostream & ) { }
				};
			template <class... T>
			void
			slots_print( std::ostream & os ) noexcept
				{
				msvc_workaround_print<sizeof...(T),std::tuple<T...>>::print(os);
				}
#else
			template <class... T>
			void
			slots_print( std::ostream & os )
				{
				{ using _ = int[ ]; (void) _ { 42, slot_print<T>(os)... }; };
				}
#endif
			}
		template <class... A>
		void
		expect<A...>::
		print_diagnostic_information( std::ostream & os ) const
			{
			using namespace leaf_detail;
			slots_print<A...>(os);
			int line = get_info_line();
			if( char const * file = get_info_file() )
				if( line==-1 )
					os << "In " << file << std::endl;
				else
					os << "At " << file << '(' << line << ')' << std::endl;
			if( char const * function = get_info_function() )
				os << "In function " << function << std::endl;
			}
		}
	}

#endif
