#ifndef BOOST_LEAF_6CCC5F56124B11E9B6C4CB8C8C7F4AFC
#define BOOST_LEAF_6CCC5F56124B11E9B6C4CB8C8C7F4AFC

// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

namespace boost { namespace leaf {

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

			virtual int err_id() const noexcept = 0;
			virtual int unload() noexcept = 0;
			virtual int unload( int error_code ) noexcept = 0;
		};
	}

} }

#endif
