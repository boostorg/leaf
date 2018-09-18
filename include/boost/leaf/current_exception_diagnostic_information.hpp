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
		class
		current_exception_diagnostic_information:
			diagnostic_information
			{
			current_exception_diagnostic_information( current_exception_diagnostic_information const & );
			current_exception_diagnostic_information & operator=( current_exception_diagnostic_information const & );
			public:
			current_exception_diagnostic_information() noexcept
				{
				}
			friend
			std::ostream &
			operator<<( std::ostream & os, current_exception_diagnostic_information const & di )
				{
				os << "Current exception diagnostic Information:" << std::endl;
				try
					{
					throw;
					}
				catch( std::exception const & ex )
					{
					os <<
						"std::exception::what(): " << ex.what() << std::endl <<
						"Dynamic typeid: " << typeid(ex).name() << std::endl;
					}
				catch( ... )
					{
					os << "Not a std::exception" << std::endl;
					}
				return os << static_cast<diagnostic_information const &>(di);
				}
			};
		}
	}

#endif
