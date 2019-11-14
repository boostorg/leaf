#ifndef LEAF_2CD8E6B8CA8D11E8BD3B80D66CE5B91B
#define LEAF_2CD8E6B8CA8D11E8BD3B80D66CE5B91B

// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/exception.hpp>
#include <memory>

namespace boost { namespace leaf {

	template <class T>
	class result;

	class bad_result:
		public std::exception,
		public error_id
	{
		char const * what() const noexcept final override
		{
			return "boost::leaf::bad_result";
		}

	public:

		explicit bad_result( error_id id ) noexcept:
			error_id(id)
		{
			assert(value());
		}
	};

	////////////////////////////////////////

	namespace leaf_detail
	{
		class error_result
		{
			template <class T>
			friend class ::boost::leaf::result;

			error_id const err_id_;
			context_ptr * const ctx_;

			error_result( error_id err_id ) noexcept:
				err_id_(err_id),
				ctx_(0)
			{
			}

			error_result( context_ptr * ctx ) noexcept:
				ctx_(ctx)
			{
				assert(ctx_!=0);
				assert(*ctx_);
			}

		public:

			error_result( error_result && x ) noexcept = default;

			template <class T>
			operator result<T>() && noexcept;

			operator error_id() && noexcept
			{
				if( ctx_ )
					return (*ctx_)->propagate_captured_errors();
				else
					return err_id_;
			}
		};
	}

	////////////////////////////////////////

	template <class T>
	class result
	{
		template <class U>
		friend class result;

		friend class leaf_detail::error_result;

		int err_id_; // 0:value_, 2:ctx_, else neither (error ids are always odd numbers)

		union
		{
			T value_;
			context_ptr ctx_;
		};

		void destroy() const noexcept
		{
			switch(err_id_)
			{
			case 0:
				value_.~T();
				break;
			case 2:
				assert(!ctx_ || ctx_->captured_id_);
				ctx_.~context_ptr();
				break;
			default:
				assert(err_id_&1);
			}
		}
		template <class U>
		void move_from( result<U> && x ) noexcept
		{
			int const x_err_id = x.err_id_;
			switch(x_err_id)
			{
			case 0:
				(void) new(&value_) T(std::move(x.value_));
				break;
			case 2:
				assert(!x.ctx_ || x.ctx_->captured_id_);
				(void) new(&ctx_) context_ptr(std::move(x.ctx_));
				break;
			default:
				assert(x_err_id&1);
			}
			err_id_ = x_err_id;
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

		template <class U>
		result( result<U> && x ) noexcept
		{
			move_from(std::move(x));
		}

		result():
			err_id_(0),
			value_(T())
		{
		}

		result( T && v ) noexcept:
			err_id_(0),
			value_(std::move(v))
		{
		}

		result( T const & v ):
			err_id_(0),
			value_(v)
		{
		}

		result( error_id err ) noexcept
		{
			if( int err_id=err.value() )
			{
				err_id_ = err_id;
				assert(err_id_&1);
			}
			else
			{
				err_id_ = 2;
				(void) new(&ctx_) context_ptr;
			}
		}

		result( context_ptr && ctx ) noexcept:
			err_id_(2),
			ctx_(std::move(ctx))
		{
		}

		result( std::error_code const & ec ) noexcept
		{
			if( int err_id=error_id(ec).value() )
			{
				err_id_ = err_id;
				assert(err_id_&1);
			}
			else
			{
				err_id_ = 2;
				(void) new(&ctx_) context_ptr;
			}
		}

		result & operator=( result && x ) noexcept
		{
			destroy();
			move_from(std::move(x));
			return *this;
		}

		template <class U>
		result & operator=( result<U> && x ) noexcept
		{
			destroy();
			move_from(std::move(x));
			return *this;
		}

		explicit operator bool() const noexcept
		{
			return err_id_==0;
		}

		T const & value() const
		{
			if( err_id_==0 )
				return value_;
			else
				::boost::leaf::throw_exception(bad_result(get_error_id()));
		}

		T & value()
		{
			if( err_id_==0 )
				return value_;
			else
				::boost::leaf::throw_exception(bad_result(get_error_id()));
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

		error_id get_error_id() const noexcept
		{
			return err_id_==2 ? ctx_->captured_id_ : leaf_detail::make_error_id(err_id_);
		}

		leaf_detail::error_result error() noexcept
		{
			assert(!*this);
			if( err_id_==2 )
				return leaf_detail::error_result(&ctx_);
			else
				return leaf_detail::error_result(leaf_detail::make_error_id(err_id_));
		}

		template <class... E>
		error_id load( E && ... e ) noexcept
		{
			return error_id(error()).load(std::forward<E>(e)...);
		}

		template <class... F>
		error_id accumulate( F && ... f ) noexcept
		{
			return error_id(error()).accumulate(std::forward<F>(f)...);
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

		result( result && x ) noexcept:
			base(std::move(x))
		{
		}

		result() noexcept
		{
		}

		result( error_id err ) noexcept:
			base(err)
		{
		}

		result( context_ptr && ctx ) noexcept:
			base(std::move(ctx))
		{
		}

		result( std::error_code const & ec ) noexcept:
			base(ec)
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

		using base::operator=;
		using base::operator bool;
		using base::get_error_id;
		using base::error;
		using base::load;
		using base::accumulate;
	};

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class T>
		inline error_result::operator result<T>() && noexcept
		{
			if( ctx_ )
				return result<T>(std::move(*ctx_));
			else
				return result<T>(err_id_);
		}
	}

	////////////////////////////////////////

	template <class R>
	struct is_result_type;

	template <class T>
	struct is_result_type<result<T>>: std::true_type
	{
	};

	template <class T>
	inline result<T> make_continuation_result( result<T> && r, context_ptr ctx = context_ptr() ) noexcept
	{
		if( r )
			return std::move(r);
		else
		{
			error_id ne = new_error();
			leaf_detail::slot_base::reassign(error_id(r.error()).value(), ne.value());
			if( ctx )
			{
				ctx->captured_id_ = ne;
				return std::move(ctx);
			}
			else
				return ne;
		}
	}

} }

#endif
