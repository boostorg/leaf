#ifndef BOOST_LEAF_6CCC5F56124B11E9B6C4CB8C8C7F4AFC
#define BOOST_LEAF_6CCC5F56124B11E9B6C4CB8C8C7F4AFC

//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <ostream>

namespace boost { namespace leaf {

	class error;

	namespace leaf_detail
	{
		class dynamic_store
		{
		protected:

			dynamic_store() noexcept
			{
			}

			~dynamic_store() noexcept
			{
			}

		public:

			virtual error get_error() const noexcept = 0;
			virtual error unload() noexcept = 0;
			virtual error unload( error const & ) noexcept = 0;
			virtual void print( std::ostream & ) const = 0;
		};
	}

} }

#endif
