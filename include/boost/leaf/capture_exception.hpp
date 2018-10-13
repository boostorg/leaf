//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_BC24FB98B2DE11E884419CF5AD35F1A2
#define UUID_BC24FB98B2DE11E884419CF5AD35F1A2

#include <boost/leaf/expect.hpp>
#include <boost/leaf/detail/captured_info.hpp>

namespace
boost
	{
	namespace
	leaf
		{
		namespace
		leaf_detail
			{
			class
			captured_exception
				{
				std::unique_ptr<captured_info> cap_;
				std::exception_ptr ex_;
				public:
				captured_exception( std::unique_ptr<captured_info> && cap, std::exception_ptr && ex ):
					cap_(std::move(cap)),
					ex_(std::move(ex))
					{
					assert(ex_);
					}
				[[noreturn]]
				void
				rethrow_original_exception()
					{
					cap_->unpack();
					std::rethrow_exception(ex_);
					}
				};
			template <class F,class... ErrorInfo>
			class
			wrapper_exception
				{
				wrapper_exception & operator=( wrapper_exception const & ) = delete;
				F f_;
				public:
				wrapper_exception( wrapper_exception const & ) = default;
				wrapper_exception( wrapper_exception && ) = default;
				explicit
				wrapper_exception( F && f ):
					f_(std::move(f))
					{
					}
				template <class... A>
				decltype(std::declval<F>()())
				operator()( A && ... a )
					{
					expect<ErrorInfo...> exp;
					try
						{
						return f_(std::forward<A>(a)...);
						}
					catch(...)
						{
						throw captured_exception(std::unique_ptr<captured_info>(new captured_info_impl<ErrorInfo...>(extract(exp))),std::current_exception());
						}
					}
				};
			}
		template <class... ErrorInfo,class F>
		leaf_detail::wrapper_exception<F,ErrorInfo...>
		capture_exception( F && f )
			{
			return leaf_detail::wrapper_exception<F,ErrorInfo...>(std::move(f));
			}
		template <class Future>
		decltype(std::declval<Future>().get())
		get( Future && f )
			{
			try
				{
				return std::forward<Future>(f).get();
				}
			catch( leaf_detail::captured_exception & ex )
				{
				ex.rethrow_original_exception();
				}
			}
		}
	}

#endif
