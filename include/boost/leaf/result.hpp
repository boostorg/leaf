//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_2CD8E6B8CA8D11E8BD3B80D66CE5B91B
#define UUID_2CD8E6B8CA8D11E8BD3B80D66CE5B91B

#include <boost/leaf/error_capture.hpp>
#include <boost/leaf/detail/throw.hpp>

#define LEAF_AUTO(v,r) auto _r_##v = r; if( !_r_##v ) return _r_##v.error(); auto & v = *_r_##v
#define LEAF_CHECK(r) {auto _r = r; if( !_r ) return _r.error();}

namespace boost { namespace leaf {

	class bad_result: public std::exception { };

	template <class T>
	class result;

	template <class... E>
	class expect;

	template <class... E, class T, class... F>
	bool handle_error( expect<E...> &, result<T> &, F && ... ) noexcept;

	template <class P, class... E, class T>
	P const * peek( expect<E...> const &, result<T> const & ) noexcept;

	template <class... E, class T>
	void diagnostic_output( std::ostream &, expect<E...> const &, result<T> const & );

	template <class... E, class T>
	result<T> capture( expect<E...> &, result<T> const & );

	////////////////////////////////////////

	template <class T>
	class result
	{

		template <class... E, class T_, class... F>
		friend bool leaf::handle_error( expect<E...> &, result<T_> &, F && ... ) noexcept;

		template <class P, class... E, class T_>
		friend P const * leaf::peek( expect<E...> const &, result<T_> const & ) noexcept;

		template <class... E, class T_>
		friend void leaf::diagnostic_output( std::ostream &, expect<E...> const &, result<T_> const & );

		template <class... E, class T_>
		friend result<T_> leaf::capture( expect<E...> &, result<T_> const & );

		union
		{
			T value_;
			error err_;
			error_capture cap_;
		};

		enum class variant
		{
			value,
			err,
			cap
		};

		variant which_;

		void destroy() noexcept
		{
			switch( which_ )
			{
			case variant:: value:
				value_.~T();
				break;
			case variant::err:
				err_.~error();
				break;
			default:
				assert(which_==variant::cap);
				cap_.~error_capture();
			}
			which_= (variant)-1;
		}

		void copy_from( result const & x )
		{
			switch( x.which_ )
			{
			case variant::value:
				(void) new(&value_) T(x.value_);
				break;
			case variant::err:
				(void) new(&err_) leaf::error(x.err_);
				break;
			default:
				assert(x.which_==variant::cap);
				(void) new(&cap_) error_capture(x.cap_);
			};
			which_ = x.which_;
		}

		void move_from( result && x ) noexcept
		{
			switch( x.which_ )
			{
			case variant:: value:
				(void) new(&value_) T(std::move(x.value_));
				break;
			case variant::err:
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

		constexpr result() noexcept:
			value_(T()),
			which_(variant::value)
		{
		}

		constexpr result( T const & v ):
			value_(v),
			which_(variant::value)
		{
		}

		constexpr result( T && v ) noexcept:
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

		result & operator=( result const & x )
		{
			destroy();
			copy_from(x);
			return *this;
		}

		result & operator=( result && x ) noexcept
		{
			destroy();
			move_from(std::move(x));
			return *this;
		}

		constexpr explicit operator bool() const noexcept
		{
			return which_==variant::value;
		}

		T const & value() const
		{
			if( which_==variant::value )
				return value_;
			else
				LEAF_THROW<bad_result>();
		}

		T & value()
		{
			if( which_==variant::value )
				return value_;
			else
				LEAF_THROW<bad_result>();
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
		leaf::error error( E && ... e ) noexcept
		{
			switch( which_ )
			{
			case variant::value:
				return leaf::error(std::forward<E>(e)...);
			case variant::cap:
			{
				error_capture cap = cap_;
				destroy();
				(void) new(&err_) leaf::error(cap.unload());
				which_ = variant::err;
			}
			default:
				assert(which_==variant::err);
				return err_.propagate(std::forward<E>(e)...);
			}
		}
	};

	////////////////////////////////////////

	template <>
	class result<void>:
		result<bool>
	{
		template <class... E, class T, class... F>
		friend bool leaf::handle_error( expect<E...> &, result<T> &, F && ... ) noexcept;

		template <class P, class... E, class T>
		friend P const * leaf::peek( expect<E...> const &, result<T> const & ) noexcept;

		template <class... E, class T>
		friend void leaf::diagnostic_output( std::ostream &, expect<E...> const &, result<T> const & );

		template <class... E, class T>
		friend result<T> leaf::capture( expect<E...> &, result<T> const & );

		typedef result<bool> base;

		result( result<bool> && rb ):
			base(std::move(rb))
		{
		}

	public:

		~result() noexcept
		{
		}

		constexpr result() noexcept = default;

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
	};

	////////////////////////////////////////

	template <class... E, class T, class... F>
	bool handle_error( expect<E...> & exp, result<T> & r, F && ... f ) noexcept
	{
		assert(!r);
		if( r.which_==result<T>::variant::err )
			return handle_error(exp,r.err_,f...);
		else
		{
			assert(r.which_==result<T>::variant::cap);
			return handle_error(r.cap_,f...);
		}
	}

	template <class P, class... E, class T>
	P const * peek( expect<E...> const & exp, result<T> const & r ) noexcept
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

	template <class... E, class T>
	void diagnostic_output( std::ostream & os, expect<E...> const & exp, result<T> const & r )
	{
		assert(!r);
		if( r.which_==result<T>::variant::err )
			return diagnostic_output(os,exp,r.err_);
		else
		{
			assert(r.which_==result<T>::variant::cap);
			return diagnostic_output(os,r.cap_);
		}
	}

	template <class... E, class T>
	result<T> capture( expect<E...> & exp, result<T> const & r )
	{
		if( r.which_==result<T>::variant::err )
			return capture(exp,r.err_);
		else
			return r;
	}

} }

#endif
