//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_BC24FB98B2DE11E884419CF5AD35F1A2
#define UUID_BC24FB98B2DE11E884419CF5AD35F1A2

#include <boost/leaf/expect.hpp>
#include <boost/leaf/capture.hpp>
#include <utility>
#include <exception>

namespace
boost
	{
	namespace
	leaf
		{
		class
		exception_wrapper
			{
			capture cap_;
			std::exception_ptr original_exception_;
			public:
			explicit
			exception_wrapper( std::exception_ptr && original_exception ):
				original_exception_(std::move(original_exception))
				{
				}
			[[noreturn]]
			void
			rethrow_original_exception()
				{
				cap_.release();
				std::rethrow_exception(original_exception_);
				}
			};
		namespace
		leaf_detail
			{
			template <class F>
			class
			transport_impl
				{
				transport_impl & operator=( transport_impl const & ) = delete;
				void (* const op_)();
				F f_;
				public:
				transport_impl( transport_impl const & ) = default;
				transport_impl( transport_impl && ) = default;
				transport_impl( void (*op)(), F f ):
					op_(op),
					f_(f)
					{
					assert(op_!=0);
					}
				template <class... T>
				decltype(std::declval<F>()())
				operator()( T && ... x )
					{
					op_();
					try
						{
						return f_(std::forward<T>(x)...);
						}
					catch(...)
						{
						throw exception_wrapper(std::current_exception());
						}
					}
				};
			}
		template <class... ErrorInfo,class F>
		leaf_detail::transport_impl<F>
		transport( F f )
			{
			using namespace leaf_detail;
			return transport_impl<F>(&open_slots<ErrorInfo...>,f);
			}
		template <class Future>
		decltype(std::declval<Future>().get())
		get( Future & f )
			{
			try
				{
				return f.get();
				}
			catch( exception_wrapper & ex )
				{
				ex.rethrow_original_exception();
				}
			}
		}
	}

#endif
