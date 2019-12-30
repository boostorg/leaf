#ifndef LEAF_75F38740D98D11E881DDB244C82C3C47
#define LEAF_75F38740D98D11E881DDB244C82C3C47

// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/error.hpp>
#include <exception>

#define LEAF_EXCEPTION(...) ::boost::leaf::leaf_detail::exception_at(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)
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

			virtual error_id get_error_id() const noexcept = 0;

		protected:

			constexpr exception_base() noexcept { }
			~exception_base() noexcept { }
		};

		template <class Ex>
		class exception:
			public Ex,
			public exception_base,
			public error_id
		{
			error_id get_error_id() const noexcept final override
			{
				return *this;
			}

		public:

			exception( exception const & ) = default;
			exception( exception && ) = default;

			LEAF_CONSTEXPR exception( error_id id, Ex && ex ) noexcept:
				Ex(std::move(ex)),
				error_id(id)
			{
				leaf_detail::enforce_std_exception(*this);
			}
		};

		template <class Ex, class... E>
		LEAF_CONSTEXPR inline exception<Ex> exception_at( char const * file, int line, char const * function, error_id id, Ex && ex, E && ... e ) noexcept
		{
			assert(file&&*file);
			assert(line>0);
			assert(function&&*function);
			return exception<Ex>(id.load(e_source_location{file,line,function},std::forward<E>(e)...), std::forward<Ex>(ex));
		}

		template <class Ex, class... E>
		LEAF_CONSTEXPR inline exception<Ex> exception_at( char const * file, int line, char const * function, Ex && ex, E && ... e ) noexcept
		{
			assert(file&&*file);
			assert(line>0);
			assert(function&&*function);
			return exception<Ex>(new_error(e_source_location{file,line,function},std::forward<E>(e)...), std::forward<Ex>(ex));
		}
	}

	template <class Ex, class... E>
	LEAF_CONSTEXPR inline leaf_detail::exception<Ex> exception( Ex && ex, E && ... e ) noexcept
	{
		return leaf_detail::exception<Ex>(leaf::new_error(std::forward<E>(e)...), std::forward<Ex>(ex));
	}

} }

#endif
