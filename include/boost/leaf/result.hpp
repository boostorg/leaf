//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_2CD8E6B8CA8D11E8BD3B80D66CE5B91B
#define UUID_2CD8E6B8CA8D11E8BD3B80D66CE5B91B

#include <boost/leaf/error_capture.hpp>

#define LEAF_AUTO(v,r) auto _r_##v = r; if( !_r_##v ) return _r_##v.error(); auto & v = *_r_##v
#define LEAF_CHECK(r) {auto _r_##v = r; if( !_r_##v ) return _r_##v.error();}

namespace
boost
	{
	namespace
	leaf
		{
		class bad_result: public std::exception { };

		template <class T>
		class result;

		template <class... E>
		class expect;

		template <class P,class... E,class T>
		decltype(P::value) const * peek( expect<E...> const &, result<T> const & ) noexcept;
		////////////////////////////////////////
		template <class T>
		class
		result
			{
			template <class P,class... E,class U>
			friend decltype(P::value) const * leaf::peek( expect<E...> const &, result<U> const & ) noexcept;

			union
				{
				T value_;
				error err_;
				error_capture cap_;
				};
			enum class
			variant
				{
				value,
				err,
				cap
				};
			variant which_;
			void
			destroy() noexcept
				{
				switch( which_ )
					{
					case variant::
					value:
						value_.~T();
						break;
					case variant::
					err:
						err_.~error();
						break;
					default:
						assert(which_==variant::cap);
						cap_.~error_capture();
					}
				which_= (variant)-1;
				}
			void
			copy_from( result const & x )
				{
				switch( x.which_ )
					{
					case variant::
					value:
						(void) new(&value_) T(x.value_);
						break;
					case variant::
					err:
						(void) new(&err_) leaf::error(x.err_);
						break;
					default:
						assert(x.which_==variant::cap);
						(void) new(&cap_) error_capture(x.cap_);
					};
				which_ = x.which_;
				}
			void
			move_from( result && x ) noexcept
				{
				switch( x.which_ )
					{
					case variant::
					value:
						(void) new(&value_) T(std::move(x.value_));
						break;
					case variant::
					err:
						(void) new(&err_) leaf::error(std::move(x.err_));
						break;
					default:
						assert(x.which_==variant::cap);
						(void) new(&cap_) error_capture(std::move(x.cap_));
					};
				which_ = x.which_;
				}

			public:

			~result() noexcept
				{
				destroy();
				}
			result( result const & x )
				{
				copy_from(x);
				}
			result( result && x ) noexcept
				{
				move_from(std::move(x));
				}
			result() noexcept:
				value_(T()),
				which_(variant::value)
				{
				}
			result( T const & v ):
				value_(v),
				which_(variant::value)
				{
				}
			result( T && v ) noexcept:
				value_(std::move(v)),
				which_(variant::value)
				{
				}
			result( leaf::error const & e ) noexcept:
				err_(e),
				which_(variant::err)
				{
				}
			result( leaf::error_capture const & cap ) noexcept:
				cap_(cap),
				which_(variant::cap)
				{
				}
			result( leaf::error_capture && cap ) noexcept:
				cap_(std::move(cap)),
				which_(variant::cap)
				{
				}
			result &
			operator=( result const & x )
				{
				destroy();
				copy_from(x);
				return *this;
				}
			result &
			operator=( result && x ) noexcept
				{
				destroy();
				move_from(std::move(x));
				return *this;
				}
			void
			reset( T const & v )
				{
				destroy();
				(void) new(&value_) T(v);
				which_ = variant::value;
				}
			void
			reset( T && v ) noexcept
				{
				destroy();
				(void) new(&value_) T(std::move(v));
				which_ = variant::value;
				}
			void
			reset( leaf::error const & e ) noexcept
				{
				destroy();
				(void) new(&err_) leaf::error(e);
				which_ = variant::err;
				}
			void
			reset( leaf::error_capture const & cap ) noexcept
				{
				destroy();
				(void) new(&cap_) leaf::error_capture(cap);
				which_ = variant::cap;
				}
			void
			reset( leaf::error_capture && cap ) noexcept
				{
				destroy();
				(void) new(&cap_) leaf::error_capture(std::move(cap));
				which_ = variant::cap;
				}
			explicit
			operator bool() const noexcept
				{
				return which_==variant::value;
				}
			T const &
			value() const
				{
				if( which_==variant::value )
					return value_;
				else
					throw bad_result();
				}
			T &
			value()
				{
				if( which_==variant::value )
					return value_;
				else
					throw bad_result();
				}
			T const &
			operator*() const
				{
				return value();
				}
			T &
			operator*()
				{
				return value();
				}
			template <class... E>
			leaf::error
			error( E && ... e ) noexcept
				{
				switch( which_ )
					{
					case variant::
					value:
						return leaf::error(std::forward<E>(e)...);
					case variant::
					cap:
						reset(cap_.propagate());
					default:
						assert(which_==variant::err);
						return err_.propagate(std::forward<E>(e)...);
					}
				}
			template <class... E>
			friend
			result &&
			capture( expect<E...> & exp, result && r )
				{
				if( r.which_==variant::err )
					{
					auto cap = capture(exp,r.err_);
					r.err_.~error();
					(void) new (&r.cap_) error_capture(std::move(cap));
					r.which_ = variant::cap;					
					}
				return std::move(r);
				}
			template <class... M,class... E>
			friend
			bool
			handle_error( expect<E...> & exp, result & r, M && ... m ) noexcept
				{
				assert(!r);
				if( r.which_==result::variant::err )
					return handle_error(exp,r.err_,m...);
				else
					{
					assert(r.which_==result::variant::cap);
					return handle_error(r.cap_,m...);
					}
				}
			template <class... E>
			friend
			void
			diagnostic_print( std::ostream & os, expect<E...> const & exp, result const & r )
				{
				assert(!r);
				if( r.which_==result::variant::err )
					return diagnostic_print(os,exp,r.err_);
				else
					{
					assert(r.which_==result::variant::cap);
					return diagnostic_print(os,r.cap_);
					}
				}
			};
		////////////////////////////////////////
		template <>
		class
		result<void>:
			result<bool>
			{
			template <class P,class... E,class T>
			friend decltype(P::value) const * leaf::peek( expect<E...> const &, result<T> const & ) noexcept;

			typedef result<bool> base;

			public:

			~result() noexcept
				{
				}
			result() noexcept
				{
				}
			result( leaf::error const & e ) noexcept:
				base(e)
				{
				}
			result( leaf::error_capture const & cap ) noexcept:
				base(cap)
				{
				}
			result( leaf::error_capture && cap ) noexcept:
				base(std::move(cap))
				{
				}
			using base::operator bool;
			using base::error;
			using base::reset;
			void reset( bool const & ) = delete;
			void reset( bool && ) = delete;
			template <class... E>
			friend
			result &&
			capture( expect<E...> & exp, result && r )
				{
				result<bool> && rb = std::move(r);
				(void) capture(exp,std::move(rb));
				return std::move(r);
				}
			template <class... M,class... E>
			friend
			bool
			handle_error( expect<E...> & exp, result & r, M && ... m ) noexcept
				{
				result<bool> & rb = r;
				return handle_error(exp,rb,m...);
				}
			template <class... E>
			friend
			void
			diagnostic_print( std::ostream & os, expect<E...> const & exp, result const & r )
				{
				result<bool> const & rb = r;
				diagnostic_print(os,exp,rb);
				}
			};
		////////////////////////////////////////
		template <class P,class... E,class T>
		decltype(P::value) const *
		peek( expect<E...> const & exp, result<T> const & r ) noexcept
			{
			assert(!r);
			if( r.which_==result<T>::variant::err )
				return peek<P>(exp,r.err_);
			else
				{
				assert(r.which_==result<T>::variant::cap);
				return peek<P>(r.cap_);
				}
			}
		}
	}

#endif
