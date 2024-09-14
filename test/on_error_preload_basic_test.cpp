// Copyright 2018-2024 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef BOOST_LEAF_TEST_SINGLE_HEADER
#   include "leaf.hpp"
#else
#   include <boost/leaf/on_error.hpp>
#   include <boost/leaf/diagnostics.hpp>
#   include <boost/leaf/result.hpp>
#endif

#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int>
struct info
{
    int value;
};

template <class G>
leaf::error_id f( G && g )
{
    return std::forward<G>(g)();
}

template <class G>
void test( G && g )
{
    int r = leaf::try_handle_all(
        [&]() -> leaf::result<int>
        {
            return f(std::move(g));
        },
        []( info<42> const & i42, leaf::diagnostic_info const & di )
        {
            BOOST_TEST_EQ(i42.value, 42);
            return 1;
        },
        []
        {
            return 2;
        } );
    BOOST_TEST_EQ(r, 1);
}

int main()
{
    test(
        []
        {
            auto load = leaf::on_error( info<42>{42}, info<-42>{-42} );
            return leaf::new_error();
        });
    test(
        []
        {
            info<42> inf1{42};
            info<-42> const inf2{-42};
            auto load = leaf::on_error( inf1, inf2 );
            return leaf::new_error();
        });
    test(
        []
        {
            info<42> inf1{42};
            auto load = leaf::on_error( inf1, info<-42>{-42} );
            return leaf::new_error();
        });
    return boost::report_errors();
}
