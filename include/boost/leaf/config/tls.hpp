#ifndef BOOST_LEAF_CONFIG_TLS_HPP_INCLUDED
#define BOOST_LEAF_CONFIG_TLS_HPP_INCLUDED

// Copyright 2018-2022 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if defined(BOOST_LEAF_TLS_FREERTOS)
#   include <boost/leaf/config/tls_freertos.hpp>
#endif

#if defined BOOST_LEAF_TLS_ARRAY
#   include <boost/leaf/config/tls_array.hpp>
#elif defined(BOOST_LEAF_NO_THREADS)
#   include <boost/leaf/config/tls_globals.hpp>
#else
#   include <boost/leaf/config/tls_cpp11.hpp>
#endif

#endif
