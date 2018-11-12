//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_47258FCCB6B411E8A1F35AA00C39171A
#define UUID_47258FCCB6B411E8A1F35AA00C39171A

#include <utility>
#include <new>
#include <cassert>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <class T>
		class optional
		{
			union { T value_; };
			bool has_value_;

		public:

			typedef T value_type;
			constexpr optional() noexcept:
				has_value_(false)
			{
			}

			constexpr optional( optional const & x ):
				has_value_(x.has_value_)
			{
				if( x.has_value_ )
					(void) new (&value_) T( x.value_ );
			}

			constexpr optional( optional && x ) noexcept:
				has_value_(x.has_value_)
			{
				if( x.has_value_ )
				{
					(void) new (&value_) T( std::move(x.value_) );
					x.value_.~T();
					x.has_value_ = false;
				}
			}

			constexpr optional( T const & v ):
				value_(v),
				has_value_(true)
			{
			}

			constexpr optional( T && v ) noexcept:
				value_(std::move(v)),
				has_value_(true)
			{
			}

			optional & operator=( optional const & x )
			{
				reset();
				if( x.has_value() )
					put(x.value());
				return *this;
			}

			optional & operator=( optional && x ) noexcept
			{
				reset();
				if( x.has_value() )
					put(std::move(x).value());
				return *this;
			}

			~optional() noexcept
			{
				reset();
			}

			void reset() noexcept
			{
				if( has_value() )
				{
					value_.~T();
					has_value_=false;
				}
			}

			T & put( T const & v )
			{
				reset();
				(void) new(&value_) T(v);
				has_value_=true;
				return value_;
			}

			T & put( T && v ) noexcept
			{
				reset();
				(void) new(&value_) T(std::move(v));
				has_value_=true;
				return value_;
			}

			bool has_value() const noexcept
			{
				return has_value_;
			}

			T const & value() const & noexcept
			{
				assert(has_value());
				return value_;
			}

			T & value() & noexcept
			{
				assert(has_value());
				return value_;
			}

			T const && value() const && noexcept
			{
				assert(has_value());
				return value_;
			}

			T value() && noexcept
			{
				assert(has_value());
				T tmp(std::move(value_));
				reset();
				return tmp;
			}
		};

	} //leaf_detail

} }

#endif
