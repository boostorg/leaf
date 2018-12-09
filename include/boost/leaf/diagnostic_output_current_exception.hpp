//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_17228D24F83C11E8AAC53F8F652D5A5F
#define UUID_17228D24F83C11E8AAC53F8F652D5A5F

#include <boost/leaf/exception_capture.hpp>

namespace boost { namespace leaf {

	template <class... E>
	void diagnostic_output_current_exception( std::ostream & os, expect<E...> const & exp )
	{
		os << "Current Exception Diagnostic Information:" << std::endl;

		try
		{
			throw;
		}
		catch( std::exception const & ex )
		{
			os <<
				"Exception dynamic type: " << typeid(ex).name() << std::endl <<
				"std::exception::what(): " << ex.what() << std::endl;
		}
		catch( ... )
		{
			os << "Unknown exception type (not a std::exception)" << std::endl;
		}

		try
		{
			throw;
		}
		catch( leaf_detail::captured_exception const & e )
		{
			diagnostic_output_(os,e);
		}
		catch( error const & e )
		{
			diagnostic_output(os,exp,e);
		}
		catch( ... )
		{
			diagnostic_output(os,exp);
		}
	}

} }

#endif
