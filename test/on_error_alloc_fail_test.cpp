// Copyright 2018-2025 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef BOOST_LEAF_TEST_SINGLE_HEADER
#   include "leaf.hpp"
#else
#   include <boost/leaf/error.hpp>
#   include <boost/leaf/on_error.hpp>
#   include <boost/leaf/handle_errors.hpp>
#   include <boost/leaf/result.hpp>
#   include <boost/leaf/diagnostics.hpp>
#endif

#if !BOOST_LEAF_CFG_CAPTURE || defined(BOOST_LEAF_NO_EXCEPTIONS)

int main()
{
    return 0;
}

#else

#include <sstream>
#include <string>

namespace leaf = boost::leaf;

struct alloc_fail_info
{
    int value;
};

namespace boost { namespace leaf { namespace detail {

template <>
struct capturing_slot_node_allocator<alloc_fail_info>
{
    template <class... A>
    static dynamic_allocator::capturing_slot_node<alloc_fail_info> * new_( A && ... )
    {
        throw std::bad_alloc();
    }

    static void delete_( dynamic_allocator::capturing_slot_node<alloc_fail_info> * p ) noexcept
    {
        delete p;
    }
};

} } } // namespace boost::leaf::detail

#include "lightweight_test.hpp"

template <int N>
struct other_info
{
    int value;
};

leaf::result<void> f()
{
    auto load = leaf::on_error( other_info<1>{1}, alloc_fail_info{42}, other_info<2>{2} );
    return leaf::new_error();
}

int main()
{
    {
        auto captured = leaf::try_capture_all(
            []() -> leaf::result<void>
            {
                return f();
            } );

        int r = leaf::try_handle_all(
            [&]() -> leaf::result<int>
            {
                (void) captured.value();
                return 0;
            },
            []( std::bad_alloc const &, other_info<1> const & oi1, other_info<2> const * oi2, alloc_fail_info const * afi )
            {
                BOOST_TEST_EQ(oi1.value, 1);
                BOOST_TEST_EQ(oi2, nullptr);
                BOOST_TEST_EQ(afi, nullptr);
                return 1;
            },
            []
            {
                return 2;
            } );
        BOOST_TEST_EQ(r, 1);
    }

#if BOOST_LEAF_CFG_DIAGNOSTICS && BOOST_LEAF_CFG_STD_STRING
    {
        int r = leaf::try_handle_all(
            []() -> leaf::result<int>
            {
                BOOST_LEAF_CHECK(f());
                return 0;
            },
            []( std::bad_alloc const &, leaf::diagnostic_details const & dd )
            {
                std::ostringstream s;
                s << dd;
                std::string str = s.str();
                BOOST_TEST(str.find("other_info<1>") != std::string::npos);
                BOOST_TEST(str.find("other_info<2>") == std::string::npos);
                BOOST_TEST(str.find("alloc_fail_info") == std::string::npos);
                return 1;
            },
            []
            {
                return 2;
            } );
        BOOST_TEST_EQ(r, 1);
    }
#endif

    return boost::report_errors();
}

#endif
