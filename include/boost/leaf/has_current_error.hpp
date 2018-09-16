//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_E20D9594B97411E89C6313280D39171A
#define UUID_E20D9594B97411E89C6313280D39171A

#include <exception>

namespace
boost
	{
	namespace
	leaf
		{
		namespace
		leaf_detail
			{
			inline
			bool
			uncaught_exception_fwd() noexcept
				{
				return std::uncaught_exception();
				}
			bool (* &
			has_current_error() noexcept)()
				{
				static thread_local bool (*f)() = &uncaught_exception_fwd;
				return f;
				}
			}
		bool
		has_current_error() noexcept
			{
			return leaf_detail::has_current_error()();
			}
		void
		set_has_current_error( bool (*f)() ) noexcept
			{
			assert(f!=0);
			leaf_detail::has_current_error() = f;
			}
		}
	}

#endif
