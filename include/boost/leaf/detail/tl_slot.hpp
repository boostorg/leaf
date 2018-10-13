//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_BF995DE0B2DF11E8B0E10DFDAD35F1A2
#define UUID_BF995DE0B2DF11E8B0E10DFDAD35F1A2

#include <boost/leaf/detail/optional.hpp>
#include <boost/leaf/detail/diagnostic_print.hpp>
#include <memory>

namespace
boost
	{
	namespace
	leaf
		{
		struct in_function;
		struct in_file;
		struct at_line;
		template <class Tag> struct ei_source_location { char const * value; };
		template <> struct ei_source_location<at_line> { int value; };
		namespace
		leaf_detail
			{
			inline
			bool &
			current_error_flag() noexcept
				{
				static thread_local bool f=false;
				return f;
				}
			inline
			unsigned &
			current_seq_id() noexcept
				{
				static thread_local unsigned n=0;
				return n;
				}
			class
			tl_slot_base
				{
				int open_count_;
				public:
				virtual void reset() noexcept = 0;
				protected:
				tl_slot_base() noexcept:
					open_count_(0)
					{
					}
				~tl_slot_base() noexcept
					{
					}
				public:
				static
				unsigned
				bump_current_seq_id()
					{
					return ++current_seq_id();
					}
				int
				open() noexcept
					{
					assert(open_count_>=0);
					return ++open_count_;
					}
				int
				close() noexcept
					{
					assert(is_open());
					if( --open_count_==0 )
						reset();
					return open_count_;
					}
				int
				is_open() const noexcept
					{
					assert(open_count_>=0);
					return open_count_;
					}
				};
			template <class T>
			class
			tl_slot:
				optional<T>,
				public tl_slot_base
				{
				tl_slot( tl_slot const & ) = delete;
				tl_slot & operator=( tl_slot const & ) = delete;
				typedef optional<T> base;
				unsigned seq_id_;
				tl_slot() noexcept:
					seq_id_(0)
					{
					}
				~tl_slot() noexcept
					{
					}
				public:
				bool
				has_value() const noexcept
					{
					if( base::has_value() )
						{
						assert(is_open());
						return seq_id_==current_seq_id();
						}
					else
						return false;
					}
				T const &
				value() const noexcept
					{
					assert(has_value());
					return base::value();
					}
				T &
				value() noexcept
					{
					assert(has_value());
					return base::value();
					}
				T
				extract_value() noexcept
					{
					assert(has_value());
					return base::extract_value();
					}
				bool
				put( T const & x )
					{
					if( is_open() )
						{
						base::put(x);
						seq_id_ = current_seq_id();
						return true;
						}
					else
						return false;
					}
				bool
				put( T && x ) noexcept
					{
					if( is_open() )
						{
						base::put(std::move(x));
						seq_id_ = current_seq_id();
						return true;
						}
					else
						return false;
					}
				void
				reset() noexcept
					{
					base::reset();
					}
				bool
				diagnostic_print( std::ostream & os ) const
					{
					if( has_value() )
						return diagnostic<T>::print(os,value());
					else
						return false;
					}
				static
				tl_slot<T> &
				tl_instance() noexcept
					{
					static_assert(sizeof(T::value),"Error info types must define a data member value");
					static thread_local tl_slot<T> x;
					return x;
					}
				};
			template <class T>
			struct
			diagnostic<ei_source_location<T>,false,true>
				{
				static
				bool
				print( std::ostream &, ei_source_location<T> const & )
					{
					return false;
					}
				};
			}
		inline
		bool
		has_current_error() noexcept
			{
			return leaf_detail::current_error_flag() || std::uncaught_exception();
			}
		}
	}

#endif
