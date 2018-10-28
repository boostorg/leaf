//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_BA049396D0D411E8B45DF7D4A759E189
#define UUID_BA049396D0D411E8B45DF7D4A759E189

#include <boost/leaf/detail/optional.hpp>
#include <atomic>
#include <cstdio>
#include <climits>
#include <ostream>

namespace
boost
	{
	namespace
	leaf
		{
		class
		error
			{
			unsigned id_;
			explicit
			error( unsigned id ) noexcept:
				id_(id)
				{
				}
			class
			id_factory
				{
				id_factory( id_factory const & ) = delete;
				id_factory & operator=( id_factory const & ) = delete;
				static
				unsigned
				new_error_id() noexcept
					{
					static std::atomic<int> c;
					return ++c;
					}
				bool next_id_valid_;
				unsigned next_id_;
				id_factory() noexcept:
					next_id_valid_(false)
					{
					}
				public:
				static
				id_factory &
				tl_instance() noexcept
					{
					static thread_local id_factory s;
					return s;
					}
				unsigned
				peek() noexcept
					{
					if( !next_id_valid_ )
						{
						next_id_ = new_error_id();
						next_id_valid_ = true;
						}
					return next_id_;
					}
				unsigned
				get() noexcept
					{
					if( next_id_valid_ )
						{
						next_id_valid_ = false;
						return next_id_;
						}
					else
						return new_error_id();
					}
				void
				reset_peek() noexcept
					{
					next_id_valid_ = false;
					}
				};
			public:
			error() noexcept:
				id_(id_factory::tl_instance().get())
				{
				}
			template <class... E>
			explicit
			error( E && ... e ) noexcept:
				id_(id_factory::tl_instance().get())
				{
				propagate(std::forward<E>(e)...);
				}
			friend
			bool
			operator==( error const & e1, error const & e2 ) noexcept
				{
				return e1.id_==e2.id_;
				}
			friend
			bool
			operator!=( error const & e1, error const & e2 ) noexcept
				{
				return e1.id_!=e2.id_;
				}
			friend
			std::ostream &
			operator<<( std::ostream & os, error const & e )
				{
				char buf[sizeof(e.id_)*CHAR_BIT/4+1];
				int nw = std::sprintf(buf,"%X",e.id_);
				assert(nw>=0);
				assert(nw<sizeof(buf));
				os << buf;
				return os;
				}
			static
			error
			peek_next_error() noexcept
				{
				return error(id_factory::tl_instance().peek());
				}
			static
			void
			clear_next_error() noexcept
				{
				id_factory::tl_instance().reset_peek();
				}
			template <class... E>
			error propagate( E && ... ) const noexcept;
			};
		////////////////////////////////////////
		namespace
		leaf_detail
			{
			template <class T>
			struct
			error_info
				{
				T v;
				error e;
				};
			template <class T>
			class
			slot:
				public optional<error_info<T>>
				{
				slot( slot const & ) = delete;
				slot & operator=( slot const & ) = delete;
				typedef optional<error_info<T>> base;
				slot<T> * prev_;
				public:
				typedef decltype(T::value) value_type;
				slot() noexcept;
				~slot() noexcept;
				};
			template <class T>
			slot<T> * &
			tl_slot_ptr() noexcept
				{
				static thread_local slot<T> * s;
				return s;
				}
			template <class T>
			slot<T>::
			slot() noexcept
				{
				slot * & p = tl_slot_ptr<T>();
				prev_ = p;
				p = this;
				}
			template <class T>
			slot<T>::
			~slot() noexcept
				{
				if( prev_ )
					{
					optional<error_info<T>> & p = *prev_;
					optional<error_info<T>> & t = *this;
					p=t;					
					}
				tl_slot_ptr<T>() = prev_;
				}
			template <class T>
			T *
			put_slot( T && v, error const & e ) noexcept
				{
				if( leaf_detail::slot<T> * p = leaf_detail::tl_slot_ptr<T>() )
					return &p->put(leaf_detail::error_info<T>{std::forward<T>(v),e}).v;
				else
					return 0;
				}
			}
		template <class... E>
		error
		error::
		propagate( E && ... e ) const noexcept
			{
			{ using _ = void const * [ ]; (void) _ { 0, leaf_detail::put_slot(std::forward<E>(e),*this)... }; }
			return *this;
			}
		////////////////////////////////////////
		namespace
		leaf_detail
			{
			template <class F>
			class
			deferred
				{
				error const e_;
				F f_;
				public:
				deferred( error const & e, F && f ) noexcept:
					e_(e),
					f_(std::forward<F>(f))
					{
					}
				~deferred() noexcept
					{
					(void) e_.propagate(f_());
					}
				};
			}
		template <class F>
		leaf_detail::deferred<F>
		defer( F && f ) noexcept
			{
			return leaf_detail::deferred<F>(error::peek_next_error(),std::forward<F>(f));
			}
		template <class... E>
		void
		preload( E && ... e ) noexcept
			{
			error::peek_next_error().propagate(std::forward<E>(e)...);
			}
		////////////////////////////////////////
		namespace
		leaf_detail
			{
			template <class F,class... T>
			struct
			match_fn
				{
				F f;
				};
			template <class... T>
			struct
			match_no_fn
				{
				};
			}
		template <class... T,class F>
		leaf_detail::match_fn<F,T...>
		match( F && f ) noexcept
			{
			return leaf_detail::match_fn<F,T...> { std::move(f) };
			}
		template <class... T>
		leaf_detail::match_no_fn<T...>
		match() noexcept
			{
			return leaf_detail::match_no_fn<T...> { };
			}
		}
	}

#endif
