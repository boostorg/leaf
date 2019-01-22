// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/error.hpp>

namespace leaf = boost::leaf;

struct no_member_value { };

leaf::error_id f()
{
	// Note: the line below should trigger a compile error (via static_assert).
	return leaf::new_error( no_member_value{ } );
}
