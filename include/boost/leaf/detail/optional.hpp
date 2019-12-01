#ifndef LEAF_47258FCCB6B411E8A1F35AA00C39171A
#define LEAF_47258FCCB6B411E8A1F35AA00C39171A

// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <utility>
#include <new>
#include <cassert>
#include <ostream>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <class T>
		class optional
		{
			int key_;
			union { T value_; };

		public:

			typedef T value_type;

			optional() noexcept:
				key_(0)
			{
			}

			optional( optional const & x ):
				key_(x.key_)
			{
				if( x.key_ )
					(void) new (&value_) T( x.value_ );
			}

			optional( optional && x ) noexcept:
				key_(x.key_)
			{
				if( x.key_ )
				{
					(void) new (&value_) T( std::move(x.value_) );
					x.reset();
				}
			}

			optional( int key, T const & v ):
				key_(key),
				value_(v)
			{
				assert(!empty());
			}

			optional( int key, T && v ) noexcept:
				key_(key),
				value_(std::move(v))
			{
				assert(!empty());
			}

			optional & operator=( optional const & x )
			{
				reset();
				if( int key = x.key() )
				{
					put(key, x.value_);
					key_ = key;
				}
				return *this;
			}

			optional & operator=( optional && x ) noexcept
			{
				reset();
				if( int key = x.key() )
				{
					put(key, std::move(x.value_));
					x.reset();
				}
				return *this;
			}

			~optional() noexcept
			{
				reset();
			}

			bool empty() const noexcept
			{
				return key_==0;
			}

			int key() const noexcept
			{
				return key_;
			}

			void set_key( int key ) noexcept
			{
				assert(!empty());
				key_ = key;
			}

			void reset() noexcept
			{
				if( key_ )
				{
					value_.~T();
					key_=0;
				}
			}

			T & put( int key, T const & v )
			{
				assert(key);
				reset();
				(void) new(&value_) T(v);
				key_=key;
				return value_;
			}

			T & put( int key, T && v ) noexcept
			{
				assert(key);
				reset();
				(void) new(&value_) T(std::move(v));
				key_=key;
				return value_;
			}

			T const * has_value(int key) const noexcept
			{
				assert(key);
				return key_==key ? &value_ : 0;
			}

			T * has_value(int key) noexcept
			{
				assert(key);
				return key_==key ? &value_ : 0;
			}

			T const & value(int key) const & noexcept
			{
				assert(has_value(key)!=0);
				return value_;
			}

			T & value(int key) & noexcept
			{
				assert(has_value(key)!=0);
				return value_;
			}

			T const && value(int key) const && noexcept
			{
				assert(has_value(key)!=0);
				return value_;
			}

			T value(int key) && noexcept
			{
				assert(has_value(key)!=0);
				T tmp(std::move(value_));
				reset();
				return tmp;
			}

			void print( std::ostream &, int key_to_print ) const;
		};

	} // leaf_detail

} }

#endif
