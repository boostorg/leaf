#ifndef BOOST_LEAF_E823AAD6151311E9A430DDBB67511AEB
#define BOOST_LEAF_E823AAD6151311E9A430DDBB67511AEB

// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/captured_exception.hpp>
#include <boost/leaf/detail/demangle.hpp>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		inline void print_exception_info( std::ostream & os, std::exception const * ex, captured_exception const * cap )
		{
			if( cap )
			{
				os << "Detected exception_capture of the following error types:" << std::endl;
				cap->print(os);
				os << "Diagnostic Information about the original exception follows" << std::endl;
			}
			if( ex )
			{
				assert(!dynamic_cast<leaf_detail::captured_exception const *>(ex));
				os <<
					"Exception dynamic type: " << leaf_detail::demangle(typeid(*ex).name()) << std::endl <<
					"std::exception::what(): " << ex->what() << std::endl;
			}
			else
				os << "Unknown exception type (not a std::exception)" << std::endl;
		}
	} // namespace leaf_detail

} }

#endif
