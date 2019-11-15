#ifndef LEAF_13D3591AFC6811E9883D0A836044C98A
#define LEAF_13D3591AFC6811E9883D0A836044C98A

// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if defined(LEAF_NO_DIAGNOSTIC_INFO) && !defined(LEAF_DISCARD_UNEXPECTED)
#	define LEAF_DISCARD_UNEXPECTED
#endif

#ifdef LEAF_USE_BOOST_CONFIG
#	include <boost/config.hpp>
#	if !defined(LEAF_NO_EXCEPTIONS) && defined(BOOST_NO_EXCEPTIONS)
#		define LEAF_NO_EXCEPTIONS
#	endif
#	if !defined(LEAF_NO_THREADS) && !defined(BOOST_HAS_THREADS)
#		define LEAF_NO_THREADS
#	endif
#endif

#endif
