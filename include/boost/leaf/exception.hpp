#ifndef LEAF_75F38740D98D11E881DDB244C82C3C47
#define LEAF_75F38740D98D11E881DDB244C82C3C47

// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/error.hpp>
#include <exception>

#define LEAF_EXCEPTION(...) ::boost::leaf::leaf_detail::exception_at(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)
#define LEAF_MAKE_EXCEPTION(Ex,...) ::boost::leaf::leaf_detail::make_exception_at<Ex>(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)
#define LEAF_THROW(...) ::boost::leaf::throw_exception(LEAF_EXCEPTION(__VA_ARGS__))

#ifdef LEAF_NO_EXCEPTIONS

namespace boost
{
	[[noreturn]] void throw_exception( std::exception const & ); // user defined
}

namespace boost { namespace leaf {

	template <class T>
	[[noreturn]] void throw_exception( T const & e )
	{
		::boost::throw_exception(e);
	}

} }

#else

namespace boost { namespace leaf {

	template <class T>
	[[noreturn]] void throw_exception( T const & e )
	{
		throw e;
	}

} }

#endif

////////////////////////////////////////

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		inline void enforce_std_exception( std::exception const & ) noexcept { }

		class exception_base
		{
		public:

			virtual error_id get_error_id() const = 0;

		protected:

			constexpr exception_base() noexcept { }
			~exception_base() noexcept { }
		};

		template <class Ex>
		class exception:
			public error_id,
			public exception_base,
			public Ex
		{
			error_id get_error_id() const final override
			{
				return *this;
			}

		public:

			exception( exception const & ) = default;
			exception( exception && ) = default;

			template <class... E>
			LEAF_CONSTEXPR exception( Ex && ex, E && ... e ) noexcept:
				error_id(new_error(std::forward<E>(e)...)),
				Ex(std::move(ex))
			{
				leaf_detail::enforce_std_exception(*this);
			}

			template <class... E>
			LEAF_CONSTEXPR exception( E && ... e ) noexcept:
				error_id(new_error(std::forward<E>(e)...))
			{
				leaf_detail::enforce_std_exception(*this);
			}
		};

		template <class Ex, class... E>
		LEAF_CONSTEXPR inline exception<Ex> exception_at( char const * file, int line, char const * function, Ex && ex, E && ... e ) noexcept
		{
			assert(file&&*file);
			assert(line>0);
			assert(function&&*function);
			return exception<Ex>( std::forward<Ex>(ex), e_source_location{file,line,function}, std::forward<E>(e)... );
		}

		template <class Ex, class... E>
		LEAF_CONSTEXPR inline exception<Ex> make_exception_at( char const * file, int line, char const * function, E && ... e ) noexcept
		{
			assert(file&&*file);
			assert(line>0);
			assert(function&&*function);
			return exception<Ex>( e_source_location{file,line,function}, std::forward<E>(e)... );
		}
	}

	template <class Ex, class... E>
	LEAF_CONSTEXPR inline leaf_detail::exception<Ex> exception( Ex && ex, E && ... e ) noexcept
	{
		return leaf_detail::exception<Ex>( std::forward<Ex>(ex), std::forward<E>(e)... );
	}

	template <class Ex, class... E>
	LEAF_CONSTEXPR inline leaf_detail::exception<Ex> make_exception( E && ... e ) noexcept
	{
		return leaf_detail::exception<Ex>( std::forward<E>(e)... );
	}

} }

#endif
