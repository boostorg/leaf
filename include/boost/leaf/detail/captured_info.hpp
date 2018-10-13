//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_C568ACFACEA811E8A23F1893A659E189
#define UUID_C568ACFACEA811E8A23F1893A659E189

#include <boost/leaf/put.hpp>
#include <utility>

namespace
boost
	{
	namespace
	leaf
		{
		namespace
		leaf_detail
			{
			template <int I,class Tuple>
			struct
			tuple_put
				{
				static
				void
				put( Tuple & tup ) noexcept
					{
					auto & opt = std::get<I-1>(tup);
					if( opt.has_value() )
						leaf::put(opt.extract_value());
					tuple_put<I-1,Tuple>::put(tup);
					}
				};
			template <class Tuple>
			struct
			tuple_put<0,Tuple>
				{
				static void put( Tuple & ) noexcept { }
				};
			class
			captured_info
				{
				protected:
				captured_info() noexcept
					{
					}
				public:
				virtual ~captured_info() noexcept = 0;
				virtual void unpack() noexcept = 0;
				};
			inline
			captured_info::
			~captured_info() noexcept
				{
				}
			template <class... ErrorInfo>
			class
			captured_info_impl:
				public captured_info
				{
				std::tuple<optional<ErrorInfo>...> cap_;
				void
				unpack() noexcept
					{
					tuple_put<sizeof...(ErrorInfo),std::tuple<optional<ErrorInfo>...>>::put(cap_);
					}
				public:
				explicit
				captured_info_impl( std::tuple<optional<ErrorInfo>...> && cap ) noexcept:
					cap_(std::move(cap))
					{
					}
				};
			template <class T>
			optional<T>
			slot_extract() noexcept
				{
				auto & x = tl_slot<T>::tl_instance();
				if( x.has_value() )
					return optional<T>(x.extract_value());
				else
					return { };
				}
			template <class... ExpectErrorInfo>
			std::tuple<leaf_detail::optional<ExpectErrorInfo>...>
			extract( expect<ExpectErrorInfo...> & ) noexcept
				{
				return std::make_tuple(slot_extract<ExpectErrorInfo>()...);
				}
			}
		}
	}

#endif
