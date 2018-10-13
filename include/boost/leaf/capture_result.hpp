//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_E95BDA9ACEAA11E894618A98A659E189
#define UUID_E95BDA9ACEAA11E894618A98A659E189

#include <boost/leaf/result.hpp>
#include <boost/leaf/detail/captured_info.hpp>

namespace
boost
	{
	namespace
	leaf
		{
		template <class T>
		class
		captured_result:
			result<T>
			{
			captured_result( captured_result const & ) = delete;
			captured_result & operator=( captured_result const & ) = delete;
			std::unique_ptr<leaf_detail::captured_info> cap_;
			public:
			captured_result( captured_result && ) noexcept = default;
			explicit
			captured_result( result<T> const & r ) noexcept:
				result<T>(r)
				{
				}
			explicit
			captured_result( result<T> && r ) noexcept:
				result<T>(std::move(r))
				{
				}
			explicit
			captured_result( std::unique_ptr<leaf_detail::captured_info> && cap ) noexcept:
				result<T>(leaf_detail::error_tag()),
				cap_(std::move(cap))
				{
				}
			template <class R,class... ExpectErrorInfo>
			friend
			result<T>
			unpack( captured_result<R> && pr, expect<ExpectErrorInfo...> & )
				{
				if( pr )
					return pr;
				else
					{
					pr.cap_->unpack();
					return leaf_detail::error_tag();
					}
				}
			};
		namespace
		leaf_detail
			{
			template <class F,class R,class... ErrorInfo>
			class wrapper_result;

			template <class F,class T,class... ErrorInfo>
			class
			wrapper_result<F,result<T>,ErrorInfo...>
				{
				wrapper_result & operator=( wrapper_result const & ) = delete;
				F f_;
				public:
				wrapper_result( wrapper_result const & ) = default;
				wrapper_result( wrapper_result && ) = default;
				explicit
				wrapper_result( F && f ):
					f_(std::move(f))
					{
					}
				template <class... A>
				captured_result<T>
				operator()( A && ... a )
					{
					expect<ErrorInfo...> exp;
					if( result<T> r = f_(std::forward<A>(a)...) )
						return captured_result<T>(r);
					else
						return captured_result<T>(std::unique_ptr<captured_info>(new captured_info_impl<ErrorInfo...>(extract(exp))));
					}
				};
			}
		template <class... ErrorInfo,class F>
		leaf_detail::wrapper_result<F,decltype(std::declval<F>()()),ErrorInfo...>
		capture_result( F && f )
			{
			return leaf_detail::wrapper_result<F,decltype(std::declval<F>()()),ErrorInfo...>(std::move(f));
			}
		}
	}

#endif
