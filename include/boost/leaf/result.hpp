#ifndef BOOST_LEAF_2CD8E6B8CA8D11E8BD3B80D66CE5B91B
#define BOOST_LEAF_2CD8E6B8CA8D11E8BD3B80D66CE5B91B

// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/dynamic_store.hpp>
#include <boost/leaf/throw.hpp>
#include <memory>

#define LEAF_AUTO(v,r) auto _r_##v = r; if( !_r_##v ) return _r_##v.error(); auto & v = *_r_##v
#define LEAF_CHECK(r) {auto _r = r; if( !_r ) return _r.error();}

namespace boost { namespace leaf {

	class bad_result: public std::exception { };

	////////////////////////////////////////

	template <class T>
	class result
	{
		using dynamic_store_ptr = std::shared_ptr<leaf_detail::dynamic_store>;

		template <class U>
		friend class result;

		union
		{
			T value_;
			error_id err_;
			dynamic_store_ptr cap_;
		};

		mutable leaf_detail::result_variant which_;

		void destroy() const noexcept
		{
			switch( which_ )
			{
			case leaf_detail::result_variant::value:
				value_.~T();
				break;
			case leaf_detail::result_variant::err:
				err_.~error_id();
				break;
			default:
				assert(which_==leaf_detail::result_variant::cap);
				cap_.~dynamic_store_ptr();
			}
			which_= (leaf_detail::result_variant)-1;
		}

		template <class U>
		void copy_from( result<U> const & x )
		{
			switch( x.which_ )
			{
			case leaf_detail::result_variant::value:
				(void) new(&value_) T(x.value_);
				break;
			case leaf_detail::result_variant::err:
				(void) new(&err_) error_id(x.err_);
				break;
			default:
				assert(x.which_==leaf_detail::result_variant::cap);
				(void) new(&cap_) dynamic_store_ptr(x.cap_);
			};
			which_ = x.which_;
		}

		template <class U>
		void move_from( result<U> && x ) noexcept
		{
			switch( x.which_ )
			{
			case leaf_detail::result_variant:: value:
				(void) new(&value_) T(std::move(x.value_));
				which_ = x.which_;
				break;
			case leaf_detail::result_variant::err:
				(void) new(&err_) error_id(std::move(x.err_));
				which_ = x.which_;
				break;
			default:
				assert(x.which_==leaf_detail::result_variant::cap);
				if( dynamic_store_ptr cap = std::move(x.cap_) )
				{
					x.destroy();
					(void) new(&x.err_) error_id(cap->error());
					x.which_ = leaf_detail::result_variant::err;
					(void) new(&cap_) dynamic_store_ptr(std::move(cap));
				}
				else
					(void) new(&cap_) dynamic_store_ptr(std::move(x.cap_));
				which_ = leaf_detail::result_variant::cap;
			};
		}

	public:

		typedef T value_type;

		~result() noexcept
		{
			destroy();
		}

		result( result && x ) noexcept
		{
			move_from(std::move(x));
		}

		result( result const & x )
		{
			copy_from(x);
		}

		template <class U>
		result( result<U> && x ) noexcept
		{
			move_from(std::move(x));
		}

		template <class U>
		result( result<U> const & x )
		{
			copy_from(x);
		}

		result():
			value_(T()),
			which_(leaf_detail::result_variant::value)
		{
		}

		result( T && v ) noexcept:
			value_(std::move(v)),
			which_(leaf_detail::result_variant::value)
		{
		}

		result( T const & v ):
			value_(v),
			which_(leaf_detail::result_variant::value)
		{
		}

		result( error_id const & id ) noexcept:
			err_(id),
			which_(leaf_detail::result_variant::err)
		{
		}

		result( std::shared_ptr<leaf_detail::dynamic_store> && ) noexcept;

		result & operator=( result && x ) noexcept
		{
			destroy();
			move_from(std::move(x));
			return *this;
		}

		result & operator=( result const & x )
		{
			destroy();
			copy_from(x);
			return *this;
		}

		template <class U>
		result & operator=( result<U> && x ) noexcept
		{
			destroy();
			move_from(std::move(x));
			return *this;
		}

		template <class U>
		result & operator=( result<U> const & x )
		{
			destroy();
			copy_from(x);
			return *this;
		}

		explicit operator bool() const noexcept
		{
			return which_==leaf_detail::result_variant::value;
		}

		T const & value() const
		{
			if( which_==leaf_detail::result_variant::value )
				return value_;
			else
				LEAF_THROW(bad_result());
		}

		T & value()
		{
			if( which_==leaf_detail::result_variant::value )
				return value_;
			else
				LEAF_THROW(bad_result());
		}

		T const & operator*() const
		{
			return value();
		}

		T & operator*()
		{
			return value();
		}

		template <class... E>
		error_id error( E && ... e ) const noexcept
		{
			switch( which_ )
			{
			case leaf_detail::result_variant::value:
				return leaf::new_error(std::forward<E>(e)...);
			case leaf_detail::result_variant::cap:
				{
					dynamic_store_ptr cap = cap_;
					destroy();
					(void) new(&err_) error_id(cap->unload());
					which_ = leaf_detail::result_variant::err;
				}
			default:
				assert(which_==leaf_detail::result_variant::err);
				return err_.propagate(std::forward<E>(e)...);
			}
		}
	};

	////////////////////////////////////////

	template <>
	class result<void>:
		result<bool>
	{
		typedef result<bool> base;

		template <class U>
		friend class result;

		result( result<bool> && rb ):
			base(std::move(rb))
		{
		}

	public:

		typedef void value_type;

		~result() noexcept
		{
		}

		result() = default;

		result( error_id const & id ) noexcept:
			base(id)
		{
		}

		result( std::shared_ptr<leaf_detail::dynamic_store> && ) noexcept;

		void value() const
		{
			(void) base::value();
		}

		void operator*() const
		{
			return value();
		}

		using base::operator bool;
		using base::error;
	};

	template <class T>
	bool succeeded( result<T> const & r )
	{
		return bool(r);
	}

	template <class T>
	error_id get_error_id( result<T> const & r )
	{
		return r.error();
	}

} }

#endif
