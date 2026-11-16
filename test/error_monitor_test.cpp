// Copyright 2018-2026 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef BOOST_LEAF_TEST_SINGLE_HEADER
#   include "leaf.hpp"
#else
#   include <boost/leaf/on_error.hpp>
#   include <boost/leaf/exception.hpp>
#   include <boost/leaf/handle_errors.hpp>
#   include <boost/leaf/result.hpp>
#endif

#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int>
struct info
{
    int value;
};

int compute_value( int * out, int (*callback)() )
{
    if( int ec = callback() )
        return ec;
    *out = 42;
    return 0;
}

int succeed()
{
    return 0;
}

int fail_without_leaf()
{
    return 1;
}

leaf::error_id callback_error;

int fail_with_leaf()
{
    callback_error = leaf::new_error(info<2>{2});
    return 2;
}

leaf::error_id checked;

leaf::result<int> augmenter( int (*callback)() )
{
    leaf::error_monitor m;
    int val;
    if( int ec = compute_value(&val, callback) )
    {
        checked = m.check();
        leaf::error_id id = m.assigned_error_id();
        BOOST_TEST(id);
        BOOST_TEST(m.assigned_error_id() == id);
        BOOST_TEST(m.check() == id);
        return id.load(info<1>{ec});
    }
    BOOST_TEST(!m.check());
    return val;
}

#ifndef BOOST_LEAF_NO_EXCEPTIONS

struct external_exception: std::exception
{
};

leaf::error_id unwind_checked;
leaf::error_id unwind_rechecked;

struct augment_on_unwind
{
    leaf::error_monitor m;

    ~augment_on_unwind()
    {
        unwind_checked = m.check();
        unwind_rechecked = m.check();
        unwind_checked.load(info<3>{3});
    }
};

#endif

int main()
{
    // Success: no error detected
    {
        int r = leaf::try_handle_all(
            []
            {
                return augmenter(succeed);
            },
            []( info<1> const & )
            {
                return -1;
            },
            []
            {
                return -2;
            } );
        BOOST_TEST_EQ(r, 42);
    }
    // Failure not reported via LEAF, with a stale error: new error_id assigned
    {
        leaf::error_id stale;
        int r = leaf::try_handle_all(
            [&]
            {
                stale = leaf::new_error(info<2>{99});
                return augmenter(fail_without_leaf);
            },
            []( info<1> const &, info<2> const & )
            {
                return -1;
            },
            [&]( info<1> const & a, leaf::error_info const & ei )
            {
                BOOST_TEST(ei.error() != stale);
                return a.value;
            },
            []
            {
                return -2;
            } );
        BOOST_TEST_EQ(r, 1);
        BOOST_TEST(stale);
        BOOST_TEST(!checked);
    }
    // Failure reported via LEAF: same error_id augmented
    {
        int r = leaf::try_handle_all(
            []
            {
                return augmenter(fail_with_leaf);
            },
            []( info<1> const & a, info<2> const & b )
            {
                return a.value * 10 + b.value;
            },
            []
            {
                return -2;
            } );
        BOOST_TEST_EQ(r, 22);
        BOOST_TEST(callback_error);
        BOOST_TEST(checked == callback_error);
    }

#ifndef BOOST_LEAF_NO_EXCEPTIONS
    // Destructor without exception in flight: no error detected
    {
        {
            augment_on_unwind a;
        }
        BOOST_TEST(!unwind_checked);
        BOOST_TEST(!unwind_rechecked);
    }
    // Unwinding due to external exception: new error_id assigned
    {
        int r = leaf::try_catch(
            []() -> int
            {
                augment_on_unwind a;
                throw external_exception();
            },
            []( external_exception const &, info<3> const & x, leaf::error_info const & ei )
            {
                BOOST_TEST(ei.error() == unwind_checked);
                return x.value;
            },
            []
            {
                return -1;
            } );
        BOOST_TEST_EQ(r, 3);
        BOOST_TEST(unwind_checked);
        BOOST_TEST(unwind_rechecked == unwind_checked);
    }
    // Unwinding due to LEAF exception: same error_id augmented
    {
        int r = leaf::try_catch(
            []() -> int
            {
                augment_on_unwind a;
                leaf::throw_exception(info<4>{4});
            },
            []( info<3> const & a, info<4> const & b, leaf::error_info const & ei )
            {
                BOOST_TEST(ei.error() == unwind_checked);
                return a.value * 10 + b.value;
            },
            []
            {
                return -1;
            } );
        BOOST_TEST_EQ(r, 34);
        BOOST_TEST(unwind_checked);
        BOOST_TEST(unwind_rechecked == unwind_checked);
    }
#endif

    return boost::report_errors();
}
