//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_BF995DE0B2DF11E8B0E10DFDAD35F1A2
#define UUID_BF995DE0B2DF11E8B0E10DFDAD35F1A2

#include <boost/leaf/detail/diagnostic_print.hpp>
#include <boost/leaf/detail/optional.hpp>
#include <memory>

namespace
boost
	{
	namespace
	leaf
		{
		namespace
		leaf_detail
			{
			class tl_slot_base;
			struct
			tl_slot_state
				{
				tl_slot_base * put_list;
				static
				tl_slot_state &
				tl_instance() noexcept
					{
					static thread_local tl_slot_state state = { };
					return state;
					}
				};
			class
			tl_slot_base
				{
				int open_count_;
				tl_slot_base * next_put_;
				public:
				virtual std::shared_ptr<void const> capture_if_has_value() = 0;
				virtual void reset() noexcept = 0;
				virtual bool diagnostic_print( std::ostream & ) const = 0;
				protected:
				tl_slot_base() noexcept:
					open_count_(0),
					next_put_(0)
					{
					}
				~tl_slot_base() noexcept
					{
					}
				void
				notify_put() noexcept
					{
					if( !next_put_ )
						{
						auto & s = tl_slot_state::tl_instance();
						next_put_ = s.put_list ? s.put_list : this;
						s.put_list = this;
						}
					}
				public:
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
				static
				void
				reset_all() noexcept
					{
					tl_slot_state & s = tl_slot_state::tl_instance();
					if( tl_slot_base * p = s.put_list )
						for(;;)
							{
							p->reset();
							tl_slot_base * n = p->next_put_;
							assert(n!=0);
							p->next_put_ = 0;
							if( p==n )
								break;
							p = n;
							}
					s.put_list = 0;
					}
				template <class F>
				static
				void
				enumerate_put( F f ) noexcept
					{
					tl_slot_state & s = tl_slot_state::tl_instance();
					if( tl_slot_base * p = s.put_list )
						for(;;)
							{
							tl_slot_base * n = p->next_put_;
							assert(n!=0);
							f(*p);
							if( p==n )
								break;
							p = n;
							}
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
				class
				captured_value
					{
					captured_value( captured_value const & ) = delete;
					captured_value & operator=( captured_value const & ) = delete;
					T v_;
					public:
					explicit
					captured_value( T && v ) noexcept:
						v_(std::move(v))
						{
						}
					~captured_value() noexcept
						{
						tl_slot<T>::tl_instance().put_(std::move(v_));
						}
					};
				std::shared_ptr<void const>
				capture_if_has_value()
					{
					if( has_value() )
						return std::make_shared<captured_value>(extract_value());
					else
						return std::shared_ptr<void const>();
					}
				tl_slot() noexcept
					{
					}
				~tl_slot() noexcept
					{
					}
				void
				put_( T && x ) noexcept
					{
					base::put(std::move(x));
					notify_put();
					}
				bool
				diagnostic_print( std::ostream & os ) const
					{
					if( has_value() )
						{
						diagnostic<T>::print(os,value());
						return true;
						}
					else
						return false;
					}
				public:
				using base::has_value;
				using base::value;
				using base::extract_value;
				bool
				put( T const & x )
					{
					if( is_open() )
						{
						base::put(x);
						notify_put();
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
						put_(std::move(x));
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
				static
				tl_slot<T> &
				tl_instance() noexcept
					{
					static thread_local tl_slot<T> x;
					return x;
					}
				};
			}
		}
	}

#endif
