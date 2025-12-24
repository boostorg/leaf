// Copyright 2018-2025 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef BOOST_LEAF_TEST_SINGLE_HEADER
#   include "leaf.hpp"
#else
#   include <boost/leaf/on_error.hpp>
#   include <boost/leaf/handle_errors.hpp>
#   include <boost/leaf/result.hpp>
#   include <boost/leaf/config/tls.hpp>
#   include <boost/leaf/diagnostics.hpp>
#endif

#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int N>
struct info
{
    int value;
};

leaf::result<void> f()
{
    BOOST_TEST_EQ(leaf::tls::read_ptr<leaf::detail::slot<info<42>>>(), nullptr);
    BOOST_TEST_NE(leaf::tls::read_ptr<leaf::detail::slot<info<43>>>(), nullptr);
    auto load = leaf::on_error( info<42>{42} );
    BOOST_TEST_EQ(leaf::tls::read_ptr<leaf::detail::slot<info<42>>>(), nullptr);
    auto e = leaf::new_error(info<43>{});
    BOOST_TEST(
        (BOOST_LEAF_CFG_CAPTURE == 0 || BOOST_LEAF_CFG_DIAGNOSTICS == 0)
        ==
        (leaf::tls::read_ptr<leaf::detail::slot<info<42>>>() == nullptr));
    return e;
}

int main()
{
    int r = leaf::try_handle_all(
        []() -> leaf::result<int>
        {
            BOOST_LEAF_CHECK(f());
            return 0;
        },
        []( info<43> const & )
        {
            return 1;
        },
        []( leaf::diagnostic_details const & )
        {
            return 2;
        } );
    BOOST_TEST_EQ(r, 1);

    return boost::report_errors();
}
