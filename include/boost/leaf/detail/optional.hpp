//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_47258FCCB6B411E8A1F35AA00C39171A
#define UUID_47258FCCB6B411E8A1F35AA00C39171A

#include <utility>
#include <new>
#include <assert.h>

namespace
boost
	{
	namespace
	leaf
		{
		namespace
		leaf_detail
			{
			template <class T>
			class
			optional
				{
				optional & operator=( optional const & ) = delete;
				union { T value_; };
				bool has_value_;
				public:
				optional() noexcept:
					has_value_(false)
					{
					}
				optional( optional && x ) noexcept:
					has_value_(false)
					{
					if( x.has_value() )
						put(std::move(x.value()));
					}
				optional( T && v ):
					has_value_(false)
					{
					put(std::move(v));
					}
				~optional() noexcept
					{
					reset();
					}
				virtual
				void
				reset() noexcept
					{
					if( has_value() )
						{
						value_.~T();
						has_value_=false;
						}
					}
				void
				put( T const & v )
					{
					reset();
					(void) new(&value_) T(v);
					has_value_=true;
					}
				void
				put( T && v ) noexcept
					{
					reset();
					(void) new(&value_) T(std::move(v));
					has_value_=true;
					}
				bool
				has_value() const noexcept
					{
					return has_value_;
					}
				T const &
				value() const noexcept
					{
					assert(has_value());
					return value_;
					}
				T &
				value() noexcept
					{
					assert(has_value());
					return value_;
					}
				T
				extract_value() noexcept
					{
					assert(has_value());
					T tmp(std::move(value_));
					reset();
					return tmp;
					}
				};
			}
		}
	}

#endif
