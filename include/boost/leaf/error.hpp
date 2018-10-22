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
#include <exception>
#include <ostream>

namespace
boost
	{
	namespace
	leaf
		{
		class error;
		namespace
		leaf_detail
			{
			template <class T>
			T * put1( T && v, error const & e ) noexcept;
			error const & set_current_error( error const & );
			struct current_error_state;
			}
		////////////////////////////////////////
		class
		error
			{
			friend struct leaf_detail::current_error_state;

			unsigned id_;

			struct default_ { };
			explicit
			error( default_ ) noexcept:
				id_(-1)
				{
				}
			static
			unsigned
			new_id() noexcept
				{
				static std::atomic<int> x;
				return ++x;
				}
			public:
			error() noexcept:
				id_(new_id())
				{
				(void) leaf_detail::set_current_error(*this);
				}
			template <class... T>
			explicit
			error( T && ... v ) noexcept:
				id_(new_id())
				{
				(void) leaf_detail::set_current_error(propagate(std::forward<T>(v)...));
				}
			template <class... T>
			error
			propagate( T && ... v ) const noexcept
				{
				{ using _ = void const * [ ]; (void) _ { leaf_detail::put1(std::forward<T>(v),*this)... }; }
				return *this;
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
			};
		////////////////////////////////////////
		namespace
		leaf_detail
			{
			struct
			current_error_state
				{
				error e;
				error const * ep;
				current_error_state() noexcept:
					e(error::default_()),
					ep(0)
					{
					}
				static
				current_error_state &
				tl_instance() noexcept
					{
					static thread_local current_error_state s;
					return s;
					}
				};
			inline
			error const &
			set_current_error( error const & e )
				{
				auto & x = current_error_state::tl_instance();
				return *(x.ep = &(x.e = e));
				}
			inline
			void
			clear_current_error()
				{
				assert(!std::uncaught_exception());
				current_error_state::tl_instance().ep = 0;
				}
			inline
			void
			clear_current_error_( error const & e )
				{
				auto & x = current_error_state::tl_instance();
				if( x.ep && *x.ep==e )
					x.ep = 0;
				}
			inline
			void
			clear_current_error1( error const & e )
				{
				assert(!std::uncaught_exception());
				clear_current_error_(e);
				}
			inline
			void
			clear_current_error2( error const & e )
				{
				assert(!std::uncaught_exception());
				clear_current_error_(e);
				}
			inline
			void
			clear_current_error3( error const & e )
				{
				assert(!std::uncaught_exception());
				clear_current_error_(e);
				}
			inline
			void
			clear_current_error4( error const & e )
				{
				assert(!std::uncaught_exception());
				clear_current_error_(e);
				}
			}
		inline
		error const *
		current_error()
			{
			auto & x = leaf_detail::current_error_state::tl_instance();
			if( error const * e = x.ep )
				return e;
			else if( std::uncaught_exception() )
				{
				error new_error;
				assert(*x.ep==new_error);
				return x.ep;
				}
			else
				return 0;
			}
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
			tl_slot_ptr()
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
				if( prev_ && this->has_value() )
					if( auto ce = current_error() )
						if( *ce==this->value().e )
							(void) prev_->put(this->extract_value());
				tl_slot_ptr<T>() = prev_;
				}
			template <class T>
			T *
			put1( T && v, error const & e ) noexcept
				{
				if( leaf_detail::slot<T> * p = leaf_detail::tl_slot_ptr<T>() )
					return &p->put(leaf_detail::error_info<T>{std::forward<T>(v),e}).v;
				else
					return 0;
				}
			}
		////////////////////////////////////////
		template <class... E>
		error
		propagate( E && ... e ) noexcept
			{
			if( error const * ce = current_error() )
				return ce->propagate(std::forward<E>(e)...);
			else
				return error(std::forward<E>(e)...);
			}
		////////////////////////////////////////
		namespace
		leaf_detail
			{
			template <class F,class... T>
			struct
			match_impl
				{
				F f;
				};
			}
		template <class... T,class F>
		leaf_detail::match_impl<F,T...>
		match( F && f )
			{
			return leaf_detail::match_impl<F,T...> { std::move(f) };
			}
		}
	}

#endif
