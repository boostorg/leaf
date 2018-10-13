//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_95126C26B8B411E8B5E01E350D39171A
#define UUID_95126C26B8B411E8B5E01E350D39171A

#include <boost/leaf/diagnostic_information.hpp>
#include <typeinfo>
#include <exception>

namespace
boost
	{
	namespace
	leaf
		{
		template <class... A>
		void
		expect<A...>::
		print_current_exception_diagnostic_information( std::ostream & os ) const
			{
			os << "Current exception diagnostic Information:" << std::endl;
			try
				{
				throw;
				}
			catch( std::exception const & ex )
				{
				os <<
					"Exception dynamic type: " << typeid(ex).name() <<
					"std::exception::what(): " << ex.what() << std::endl << std::endl;
				}
			catch( ... )
				{
				os << "Unknown exception type (not a std::exception)" << std::endl;
				}
			print_diagnostic_information(os);
			}
		}
	}

#endif
