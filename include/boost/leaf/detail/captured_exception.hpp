#ifndef BOOST_LEAF_E32F3CCC139011E995085E318C7F4AFC
#define BOOST_LEAF_E32F3CCC139011E995085E318C7F4AFC

// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <exception>
#include <ostream>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		class captured_exception:
			public std::exception
		{
		protected:

			captured_exception() noexcept
			{
			}

			~captured_exception() noexcept
			{
			}

		public:

			[[noreturn]] virtual void unload_and_rethrow_original_exception() = 0;
			virtual void print( std::ostream & ) const = 0;
		};
	}

} }

#endif
