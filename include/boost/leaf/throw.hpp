#ifndef BOOST_LEAF_75F38740D98D11E881DDB244C82C3C47
#define BOOST_LEAF_75F38740D98D11E881DDB244C82C3C47

// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/error.hpp>
#include <boost/leaf/throw_exception.hpp>
#include <exception>

#define LEAF_EXCEPTION(...) ::boost::leaf::leaf_detail::exception_at(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)
#define LEAF_THROW(...) ::boost::leaf::throw_exception(LEAF_EXCEPTION(__VA_ARGS__))

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		inline void enforce_std_exception( std::exception const & ) noexcept { }

		template <class Ex>
		class exception:
			public Ex,
			public error
		{
		public:

			exception( exception const & ) = default;
			exception( exception && ) = default;

			template <class... E>
			exception( Ex && ex, E && ... e ) noexcept:
				Ex(std::move(ex)),
				error(new_error(std::forward<E>(e)...))
			{
				leaf_detail::enforce_std_exception(*this);
			}
		};

		template <class Ex, class... E>
		exception<Ex> exception_at( char const * file, int line, char const * function, Ex && ex, E && ... e ) noexcept
		{
			assert(file&&*file);
			assert(line>0);
			assert(function&&*function);
			return exception<Ex>( std::forward<Ex>(ex), e_source_location{file,line,function}, std::forward<E>(e)... );
		}
	}

	template <class Ex, class... E>
	leaf_detail::exception<Ex> exception( Ex && ex, E && ... e ) noexcept
	{
		return leaf_detail::exception<Ex>( std::forward<Ex>(ex), std::forward<E>(e)... );
	}

} }

#endif
