//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_75F38740D98D11E881DDB244C82C3C47
#define UUID_75F38740D98D11E881DDB244C82C3C47

#include <boost/leaf/common.hpp>
#include <exception>

#define LEAF_THROW ::boost::leaf::next_error_value().propagate(::boost::leaf::meta::e_source_location{__FILE__,__LINE__,__FUNCTION__}),throw::boost::leaf::exception

namespace boost { namespace leaf {

	////////////////////////////////////////

	namespace leaf_detail
	{
		inline void enforce_std_exception( std::exception const & ) { }
	} //leaf_detail

	template <class Ex>
	class exception:
		public Ex,
		public error
	{
	public:

		exception( exception const & ) = default;
		exception( exception && ) = default;

		template <class... E>
		exception( E && ... e ) noexcept:
			error(std::forward<E>(e)...)
		{
			leaf_detail::enforce_std_exception(*this);
		}

		template <class... E>
		exception( Ex && ex, E && ... e ) noexcept:
			Ex(std::move(ex)),
			error(std::forward<E>(e)...)
		{
			leaf_detail::enforce_std_exception(*this);
		}
	};

} }

#endif
