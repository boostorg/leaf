// Copyright 2018-2026 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef BOOST_LEAF_CFG_CAPTURE
#   undef BOOST_LEAF_CFG_CAPTURE
#endif
#define BOOST_LEAF_CFG_CAPTURE 0
#ifdef BOOST_LEAF_CFG_DIAGNOSTICS
#   undef BOOST_LEAF_CFG_DIAGNOSTICS
#endif
#define BOOST_LEAF_CFG_DIAGNOSTICS 0
#define BOOST_LEAF_USE_TLS_ARRAY
#define BOOST_LEAF_CFG_TLS_ARRAY_SIZE 2
#define BOOST_LEAF_CFG_TLS_ARRAY_START_INDEX 0

#ifdef BOOST_LEAF_TEST_SINGLE_HEADER
#   include "leaf.hpp"
#else
#   include <boost/leaf/handle_errors.hpp>
#   include <boost/leaf/result.hpp>
#endif

#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

static void * tls_storage[BOOST_LEAF_CFG_TLS_ARRAY_SIZE + 1];

namespace boost { namespace leaf {

namespace tls
{
    void * read_void_ptr( int tls_index ) noexcept
    {
        BOOST_TEST_GE(tls_index, (BOOST_LEAF_CFG_TLS_ARRAY_START_INDEX));
        BOOST_TEST_LT(tls_index, (BOOST_LEAF_CFG_TLS_ARRAY_SIZE));
        return tls_storage[tls_index];
    }

    void write_void_ptr( int tls_index, void * p ) noexcept
    {
        BOOST_TEST_GE(tls_index, (BOOST_LEAF_CFG_TLS_ARRAY_START_INDEX));
        BOOST_TEST_LT(tls_index, (BOOST_LEAF_CFG_TLS_ARRAY_SIZE));
        tls_storage[tls_index] = p;
    }
}

} }

struct my_error_info
{
    int value;
};

int main()
{
    leaf::result<int> r = leaf::try_handle_some(
        []() -> leaf::result<int>
        {
            BOOST_TEST(!tls_storage[BOOST_LEAF_CFG_TLS_ARRAY_SIZE]); // detect overflow
            return leaf::new_error(my_error_info{42});
        },
        []( my_error_info const & x ) -> leaf::result<int>
        {
            BOOST_TEST(!tls_storage[BOOST_LEAF_CFG_TLS_ARRAY_SIZE]); // detect overflow
            return x.value;
        } );
    BOOST_TEST(!tls_storage[BOOST_LEAF_CFG_TLS_ARRAY_SIZE]); // detect overflow
    BOOST_TEST(r);
    BOOST_TEST_EQ(r.value(), 42);
    return boost::report_errors();
}
