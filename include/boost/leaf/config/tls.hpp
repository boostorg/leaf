#ifndef BOOST_LEAF_CONFIG_TLS_HPP_INCLUDED
#define BOOST_LEAF_CONFIG_TLS_HPP_INCLUDED

// Copyright 2018-2024 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <atomic>

#if defined(BOOST_LEAF_TLS_FREERTOS)
#   include <boost/leaf/config/tls_freertos.hpp>
#endif

#ifndef BOOST_LEAF_USE_TLS_ARRAY
#	ifdef BOOST_LEAF_CFG_TLS_INDEX_TYPE
#		warning "BOOST_LEAF_CFG_TLS_INDEX_TYPE" is ignored if BOOST_LEAF_USE_TLS_ARRAY is not defined.
#	endif
#	ifdef BOOST_LEAF_CFG_TLS_ARRAY_SIZE
#		warning "BOOST_LEAF_CFG_TLS_ARRAY_SIZE" is ignored if BOOST_LEAF_USE_TLS_ARRAY is not defined.
#	endif
#	ifdef BOOST_LEAF_CFG_TLS_ARRAY_START_INDEX
#		warning "BOOST_LEAF_CFG_TLS_ARRAY_START_INDEX" is ignored if BOOST_LEAF_USE_TLS_ARRAY is not defined.
#	endif
#endif

#if BOOST_LEAF_CFG_WIN32 == 2
#   include <boost/leaf/config/tls_win32.hpp>
#else
#   if defined BOOST_LEAF_USE_TLS_ARRAY
#       include <boost/leaf/config/tls_array.hpp>
#   elif defined(BOOST_LEAF_NO_THREADS)
#       include <boost/leaf/config/tls_globals.hpp>
#   else
#       include <boost/leaf/config/tls_cpp11.hpp>
#   endif

namespace boost { namespace leaf {

namespace detail
{
    template <class=void>
    struct BOOST_LEAF_SYMBOL_VISIBLE id_factory
    {
        static atomic_unsigned_int counter;
    };

    template <class T>
    atomic_unsigned_int id_factory<T>::counter(1);

    inline unsigned generate_next_error_id() noexcept
    {
        unsigned id = (id_factory<>::counter += 4);
        BOOST_LEAF_ASSERT((id&3) == 1);
        return id;
    }
}

} }

#endif

#endif // BOOST_LEAF_CONFIG_TLS_HPP_INCLUDED
