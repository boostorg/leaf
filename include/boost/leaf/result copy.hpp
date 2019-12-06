#ifndef LEAF_2CD8E6B8CA8D11E8BD3B80D66CE5B91B
#define LEAF_2CD8E6B8CA8D11E8BD3B80D66CE5B91B

// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/exception.hpp>
#include <memory>
#include <climits>

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

		explicit bad_result( error_id id ) noexcept:
			error_id(id)
		{
			assert(value());
		}
	};

	////////////////////////////////////////

	namespace leaf_detail
	{
		class result_discriminant
		{
			unsigned state_;

		public:

			enum kind_t
			{
				no_error = 0,
				err_id = 1,
				ctx_ptr = 2,
				val = 3
			};

			LEAF_CONSTEXPR explicit result_discriminant( error_id id ) noexcept:
				state_(id.value())
			{
				assert((state_&3)==1);
			}

			struct kind_no_error { };
			LEAF_CONSTEXPR explicit result_discriminant( kind_no_error ) noexcept:
				state_(no_error)
			{
			}

			struct kind_val { };
			LEAF_CONSTEXPR explicit result_discriminant( kind_val ) noexcept:
				state_(val)
			{
			}

			struct kind_ctx_ptr { };
			LEAF_CONSTEXPR explicit result_discriminant( kind_ctx_ptr ) noexcept:
				state_(ctx_ptr)
			{
			}

			LEAF_CONSTEXPR kind_t kind() const noexcept
			{
				return kind_t(state_&3);
			}

			LEAF_CONSTEXPR error_id get_error_id() const noexcept
			{
				assert(kind()==no_error || kind()==err_id);
				return leaf_detail::make_error_id(state_);
			}
		};
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		struct void_ { };

		template <class T>
		class result_state
		{
			union
			{
				T val_;
				context_ptr ctx_;
				void_ neither_;
			};

			result_discriminant what_;

		public:

			explicit LEAF_CONSTEXPR result_state( result_discriminant what ) noexcept:
				neither_{},
				what_(what)
			{
				assert(what_.kind()==result_discriminant::no_error || what_.kind()==result_discriminant::err_id);
			}

			explicit LEAF_CONSTEXPR result_state( T && v ) noexcept:
				val_(std::move(v)),
				what_(result_discriminant::kind_val{})
			{
			}

			explicit LEAF_CONSTEXPR result_state( T const & v ):
				val_(v),
				what_(result_discriminant::kind_val{})
			{
			}

			explicit result_state( context_ptr && ctx ) noexcept:
				ctx_(std::move(ctx)),
				what_(result_discriminant::kind_ctx_ptr{})
			{
			}

			~result_state() noexcept
			{
				switch(what_.kind())
				{
				case result_discriminant::val:
					val_.~T();
					break;
				case result_discriminant::ctx_ptr:
					assert(!ctx_ || ctx_->captured_id_);
					ctx_.~context_ptr();
					break;
				default:
					neither_.~void_();
				}
			}

			LEAF_CONSTEXPR result_discriminant what() const noexcept
			{
				return what_;
			}

			LEAF_CONSTEXPR T const & value() const
			{
				if( what_.kind() == result_discriminant::val )
					return val_;
				else
					::boost::leaf::throw_exception(bad_result(get_error_id()));
			}

			LEAF_CONSTEXPR T & value()
			{
				if( what_.kind() == result_discriminant::val )
					return val_;
				else
					::boost::leaf::throw_exception(bad_result(get_error_id()));
			}

			T exctract_val() noexcept
			{
				assert(what_.kind() == result_discriminant::val);
				return std::move(val_);
			}

			context_ptr extract_ctx() noexcept
			{
				assert(what_.kind() == result_discriminant::ctx_ptr);
				return std::move(ctx_);
			}

			LEAF_CONSTEXPR error_id get_error_id() const noexcept
			{
				assert(what_.kind()!=result_discriminant::val);
				return what_.kind()==result_discriminant::ctx_ptr ? ctx_->captured_id_ : what_.get_error_id();
			}
		};

		template <>
		class result_state<void>
		{
			union
			{
				context_ptr ctx_;
				void_ neither_;
			};

			result_discriminant what_;

		public:

			explicit LEAF_CONSTEXPR result_state( result_discriminant what ) noexcept:
				neither_{},
				what_(what)
			{
				assert(what_.kind()==result_discriminant::no_error || what_.kind()==result_discriminant::err_id);
			}

			LEAF_CONSTEXPR result_state() noexcept:
				neither_{},
				what_(result_discriminant::kind_val{})
			{
			}


			explicit result_state( context_ptr && ctx ) noexcept:
				ctx_(std::move(ctx)),
				what_(result_discriminant::kind_ctx_ptr{})
			{
			}

			~result_state() noexcept
			{
				switch(what_.kind())
				{
				case result_discriminant::ctx_ptr:
					assert(!ctx_ || ctx_->captured_id_);
					ctx_.~context_ptr();
					break;
				default:
					neither_.~void_();
				}
			}

			LEAF_CONSTEXPR result_discriminant what() const noexcept
			{
				return what_;
			}

			LEAF_CONSTEXPR void value() const
			{
				if( what_.kind() != result_discriminant::val )
					::boost::leaf::throw_exception(bad_result(get_error_id()));
			}

			result_state move_val() noexcept
			{
				assert(what_.kind() == result_discriminant::val);
				return {};
			}

			result_state move_ctx() noexcept
			{
				assert(what_.kind() == result_discriminant::ctx_ptr);
				return result_state(std::move(ctx_));
			}

			LEAF_CONSTEXPR context_ptr const & ctx() noexcept
			{
				assert(what_.kind() == result_discriminant::ctx_ptr);
				return ctx_;
			}

			LEAF_CONSTEXPR error_id get_error_id() const noexcept
			{
				assert(what_.kind()!=result_discriminant::val);
				return what_.kind()==result_discriminant::ctx_ptr ? ctx_->captured_id_ : what_.get_error_id();
			}
		};
	}

	////////////////////////////////////////

	template <class T>
	class result
	{
		template <class U>
		friend class result;

		using void_ = leaf_detail::void_;
		using state = leaf_detail::result_state<T>;

		struct error_result
		{
			error_result( error_result && ) = default;
			error_result( error_result const & ) = delete;
			error_result & operator=( error_result const & ) = delete;

			result & r_;

			LEAF_CONSTEXPR error_result( result & r ) noexcept:
				r_(r)
			{
			}

			template <class U>
			LEAF_CONSTEXPR operator result<U>() noexcept
			{
				using leaf_detail::result_discriminant;
				result_discriminant what = r_.s_.what();
				switch(what.kind())
				{
				case result_discriminant::val:
					return result<U>(result_discriminant::no_error);
				case result_discriminant::ctx_ptr:
					return result<U>(r_.s_.move_ctx());
				default:
					return result<U>(what);
				}
			}

			LEAF_CONSTEXPR operator error_id() noexcept
			{
				using leaf_detail::result_discriminant;
				result_discriminant what = r_.s_.what();
				switch(what.kind())
				{
				case result_discriminant::val:
					return error_id();
				case result_discriminant::ctx_ptr:
					return r_.s_.ctx()->propagate_captured_errors();
				default:
					return what.get_error_id();
				}
			}
		};

		state s_;

		LEAF_CONSTEXPR result( leaf_detail::result_discriminant what ) noexcept:
			s_(what)
		{
		}

		template <class U>
		LEAF_CONSTEXPR state move_from( result<U> && x ) noexcept
		{
			using leaf_detail::result_discriminant;
			result_discriminant x_what = x.s_.what();
			switch(x_what.kind())
			{
			case result_discriminant::val:
				return x.s_.move_val();
			case result_discriminant::ctx_ptr:
				return x.s_.move_ctx();
			default:
				return state(x_what);
			}
		}

	public:

		typedef T value_type;

		LEAF_CONSTEXPR result( result && x ) noexcept:
			s_(move_from(std::move(x)))
		{
		}

		template <class U>
		LEAF_CONSTEXPR result( result<U> && x ) noexcept:
			s_(move_from(std::move(x)))

		{
		}

		LEAF_CONSTEXPR result()
		{
		}

		LEAF_CONSTEXPR result( T && v ) noexcept:
			s_(std::move(v))
		{
		}

		LEAF_CONSTEXPR result( T const & v ):
			s_(v)
		{
		}

		LEAF_CONSTEXPR result( error_id err ) noexcept:
			s_(leaf_detail::result_discriminant(err))
		{
		}

		LEAF_CONSTEXPR result( std::error_code const & ec ) noexcept:
			s_(error_id(ec))
		{
		}

		LEAF_CONSTEXPR result( context_ptr && ctx ) noexcept:
			s_(std::move(ctx))
		{
		}

		LEAF_CONSTEXPR result & operator=( result && x ) noexcept
		{
			s_.~state();
			(void) new(&s_) state(move_from(std::move(x)));
			return *this;
		}

		template <class U>
		LEAF_CONSTEXPR result & operator=( result<U> && x ) noexcept
		{
			s_.~state();
			(void) new(&s_) state(move_from(std::move(x)));
			return *this;
		}

		LEAF_CONSTEXPR explicit operator bool() const noexcept
		{
			using leaf_detail::result_discriminant;
			return s_.what().kind() == result_discriminant::val;
		}

		LEAF_CONSTEXPR T const & value() const
		{
			return s_.value();
		}

		LEAF_CONSTEXPR T & value()
		{
			return s_.value();
		}

		LEAF_CONSTEXPR T const & operator*() const
		{
			return s_.value();
		}

		LEAF_CONSTEXPR T & operator*()
		{
			return s_.value();
		}

		LEAF_CONSTEXPR T const * operator->() const
		{
			return &s_.value();
		}

		LEAF_CONSTEXPR T * operator->()
		{
			return &s_.value();
		}

		LEAF_CONSTEXPR error_result error() noexcept
		{
			return error_result{*this};
		}

		template <class... E>
		LEAF_CONSTEXPR error_id load( E && ... e ) noexcept
		{
			return error_id(error()).load(std::forward<E>(e)...);
		}

		template <class... F>
		LEAF_CONSTEXPR error_id accumulate( F && ... f ) noexcept
		{
			return error_id(error()).accumulate(std::forward<F>(f)...);
		}
	};

	////////////////////////////////////////

	template <class R>
	struct is_result_type;

	template <class T>
	struct is_result_type<result<T>>: std::true_type
	{
	};
} }

#endif
