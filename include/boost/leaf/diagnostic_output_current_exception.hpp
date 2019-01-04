#ifndef BOOST_LEAF_17228D24F83C11E8AAC53F8F652D5A5F
#define BOOST_LEAF_17228D24F83C11E8AAC53F8F652D5A5F

//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/exception_capture.hpp>
#include <boost/leaf/detail/demangle.hpp>

namespace boost { namespace leaf {

	template <class... E>
	void diagnostic_output_current_exception( std::ostream & os )
	{
		os << "Current Exception Diagnostic Information:" << std::endl;

		try
		{
			throw;
		}
		catch( std::exception const & ex )
		{
			os <<
				"Exception dynamic type: " << leaf_detail::demangle(typeid(ex).name()) << std::endl <<
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
		catch( leaf_detail::captured_exception const & ce )
		{
			ce.diagnostic_output(os);
		}
		catch( error const & e )
		{
			e.diagnostic_output(os);
		}
		catch( ... )
		{
			global_diagnostic_output(os);
		}
	}

} }

#endif
