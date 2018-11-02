//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_75F38740D98D11E881DDB244C82C3C47
#define UUID_75F38740D98D11E881DDB244C82C3C47

#include <boost/leaf/common.hpp>
#include <exception>

#define LEAF_THROW ::boost::leaf::peek_next_error().propagate(::boost::leaf::e_source_location{::boost::leaf::e_source_location::loc(__FILE__,__LINE__,__FUNCTION__)}),throw::boost::leaf::make_exception

namespace boost { namespace leaf {

	struct e_source_location
	{
		struct loc
		{
			char const * const file;
			int const line;
			char const * const function;

			loc( char const * file, int line, char const * function ) noexcept:
				file(file),
				line(line),
				function(function)
			{
				assert(file!=0);
				assert(line>0);
				assert(function!=0);
			}
		};

		loc value;

		friend std::ostream & operator<<( std::ostream & os, e_source_location const & x )
		{
			return os << "At " << x.value.file << '(' << x.value.line << ") in function " << x.value.function << std::endl;
		}
	};

	////////////////////////////////////////

	namespace leaf_detail
	{
		inline void enforce_std_exception( std::exception const & ) { }

		template <class Ex>
		class exception:
			public Ex,
			public error
		{
		public:

			exception( Ex && ex, error  const & e ) noexcept:
				Ex(std::move(ex)),
				error(e)
			{
				enforce_std_exception(*this);
			}
		};
	} //leaf_detail

	template <class Ex,class... E>
	leaf_detail::exception<Ex> make_exception( Ex && ex, E && ... e ) noexcept
	{
		return leaf_detail::exception<Ex>( std::forward<Ex>(ex), error(std::forward<E>(e)...) );
	}

	template <class Ex,class... E> [[noreturn]]
	void throw_exception( Ex && ex, E && ... e )
	{
		throw make_exception( std::forward<Ex>(ex), std::forward<E>(e)... );
	}

} }

#endif
