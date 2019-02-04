#ifndef BOOST_LEAF_2CD8E6B8CA8D11E8BD3B80D66CE5B91B
#define BOOST_LEAF_2CD8E6B8CA8D11E8BD3B80D66CE5B91B

// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/dynamic_store.hpp>
#include <boost/leaf/exception.hpp>
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
			mutable int err_id_;
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
			case leaf_detail::result_variant::err_id:
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
			case leaf_detail::result_variant::err_id:
				err_id_ = x.err_id_;
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
			case leaf_detail::result_variant::err_id:
				err_id_ = x.err_id_;
				which_ = x.which_;
				break;
			default:
				assert(x.which_==leaf_detail::result_variant::cap);
				if( dynamic_store_ptr cap = std::move(x.cap_) )
				{
					x.destroy();
					x.err_id_ = cap->err_id();
					x.which_ = leaf_detail::result_variant::err_id;
					(void) new(&cap_) dynamic_store_ptr(std::move(cap));
				}
				else
					(void) new(&cap_) dynamic_store_ptr(std::move(x.cap_));
				which_ = leaf_detail::result_variant::cap;
			};
		}

		int get_err_id() const noexcept
		{
			switch( which_ )
			{
			case leaf_detail::result_variant::value:
				return 0;
			case leaf_detail::result_variant::cap:
				{
					dynamic_store_ptr cap = cap_;
					destroy();
					err_id_ = cap->unload();
					which_ = leaf_detail::result_variant::err_id;
				}
			default:
				assert(which_==leaf_detail::result_variant::err_id);
				return err_id_;
			}
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

		result( error_id const & err ) noexcept:
			err_id_(err.value()),
			which_(leaf_detail::result_variant::err_id)
		{
		}

		result( std::error_code const & ec ) noexcept:
			err_id_(error_id(ec).value()),
			which_(leaf_detail::result_variant::err_id)
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
		error_id error() const noexcept
		{
			return leaf_detail::make_error_id(get_err_id());
		}

		template <class... E>
		result & load( E && ... e ) noexcept
		{
			(void) leaf_detail::make_error_id(get_err_id()).load(std::forward<E>(e)...);
			return *this;
		}

		template <class... F>
		result & accumulate( F && ... f ) noexcept
		{
			(void) leaf_detail::make_error_id(get_err_id()).accumulate(std::forward<F>(f)...);
			return *this;
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

		result( error_id const & err ) noexcept:
			base(err)
		{
		}

		result( std::error_code const & ec ) noexcept:
			base(ec)
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

		template <class... E>
		result & load( E && ... e ) noexcept
		{
			(void) base::load(std::forward<E>(e)...);
			return *this;
		}

		template <class... F>
		result & accumulate( F && ... f ) noexcept
		{
			(void) base::accumulate(std::forward<F>(f)...);
			return *this;
		}
	};

	template <class R>
	struct is_result_type;

	template <class T>
	struct is_result_type<result<T>>: std::true_type
	{
	};

	template <class T>
	result<T> make_continuation_result( result<T> && r ) noexcept
	{
		if( r )
			return r;
		else
		{
			error_id ne = new_error();
			leaf_detail::slot_base::reassign(r.error().value(), ne.value());
			return ne;
		}
	}
} }

#endif
