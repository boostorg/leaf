#ifndef BOOST_LEAF_2CD8E6B8CA8D11E8BD3B80D66CE5B91B
#define BOOST_LEAF_2CD8E6B8CA8D11E8BD3B80D66CE5B91B

// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/exception.hpp>
#include <memory>

namespace boost { namespace leaf {

	class bad_result:
		public std::exception,
		public error_id
	{
		char const * what() const noexcept final override
		{
			return "boost::leaf::bad_result";
		}

	public:

		explicit bad_result( error_id && id ) noexcept:
			error_id(std::move(id))
		{
			assert(value());
		}
	};

	////////////////////////////////////////

	template <class T>
	class result
	{
		template <class U>
		friend class result;

		union
		{
			T value_;
			mutable int err_id_;
			context_ptr ctx_;
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
				assert(which_==leaf_detail::result_variant::ctx);
				ctx_.~context_ptr();
			}
			which_ = leaf_detail::result_variant::err_id;
			err_id_ = 0;
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
				assert(x.which_==leaf_detail::result_variant::ctx);
				(void) new(&ctx_) context_ptr(x.ctx_);
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
				assert(x.which_==leaf_detail::result_variant::ctx);
				assert(x.ctx_);
				(void) new(&ctx_) context_ptr(std::move(x.ctx_));
				x.destroy();
				x.err_id_ = leaf_detail::import_error_code(ctx_->ec).value();
				x.which_ = leaf_detail::result_variant::err_id;
				which_ = leaf_detail::result_variant::ctx;
			};
		}

		int unload_then_get_err_id() const noexcept
		{
			switch( which_ )
			{
			case leaf_detail::result_variant::value:
				return 0;
			case leaf_detail::result_variant::ctx:
				{
					context_ptr ctx = std::move(ctx_);
					destroy();
					err_id_ = leaf_detail::import_error_code(ctx->ec).value();
					assert(err_id_!=0);
					which_ = leaf_detail::result_variant::err_id;
					context_activator active_context(*ctx, on_deactivation::propagate);
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

		result( context_ptr const & ctx ) noexcept:
			ctx_(ctx),
			which_(leaf_detail::result_variant::ctx)
		{
			assert(ctx_->ec);
			assert(leaf_detail::is_error_id(ctx_->ec));
		}

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
				throw bad_result(error());
		}

		T & value()
		{
			if( which_==leaf_detail::result_variant::value )
				return value_;
			else
				throw bad_result(error());
		}

		T const & operator*() const
		{
			return value();
		}

		T & operator*()
		{
			return value();
		}

		T const * operator->() const
		{
			return &value();
		}

		T * operator->()
		{
			return &value();
		}

		error_id error() const noexcept
		{
			return std::error_code(unload_then_get_err_id(), leaf_detail::get_error_category());
		}

		template <class... E>
		error_id load( E && ... e ) noexcept
		{
			if( *this )
				return error_id();
			else
				return error().load(std::forward<E>(e)...);
		}

		template <class... F>
		error_id accumulate( F && ... f ) noexcept
		{
			if( *this )
				return error_id();
			else
				return error().accumulate(std::forward<F>(f)...);
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

		result( context_ptr const & ctx ) noexcept:
			base(ctx)
		{
		}

		void value() const
		{
			(void) base::value();
		}

		void operator*() const
		{
			return value();
		}


		void operator->() const
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
	inline result<T> make_continuation_result( result<T>  const & r, context_ptr const & ctx = context_ptr() ) noexcept
	{
		if( r )
			return r;
		else
		{
			error_id ne = new_error();
			leaf_detail::slot_base::reassign(r.error().value(), ne.value());
			if( ctx )
			{
				ctx->ec = ne;
				return ctx;
			}
			else
				return ne;
		}
	}

} }

#endif
