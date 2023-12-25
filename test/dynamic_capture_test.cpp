// Copyright 2018-2023 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>

#if BOOST_LEAF_CFG_STD_STRING
#   include <sstream>
#   include <iostream>
#endif

#if !BOOST_LEAF_CFG_CAPTURE

#include <iostream>

int main()
{
    std::cout << "Unit test not applicable." << std::endl;
    return 0;
}

#else

#ifdef BOOST_LEAF_TEST_SINGLE_HEADER
#   include "leaf.hpp"
#else
#   include <boost/leaf/result.hpp>
#   include <boost/leaf/handle_errors.hpp>
#endif

#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

namespace
{
    template <int>
    struct err
    {
        static int count;
        static int move_count;

        err()
        {
            ++count;
        }

        err( err const & )
        {
            ++count;
        }

        err( err && )
        {
            ++count;
            ++move_count;
        }

        ~err()
        {
            --count;
        }
    };
    template <int N> int err<N>::count = 0;
    template <int N> int err<N>::move_count = 0;
}

int main()
{
    {
        leaf::try_handle_all(
            []() -> leaf::result<void>
            {
                return leaf::new_error(err<1>{}, err<2>{});
            },
            [&]( err<1>, err<2> &, leaf::dynamic_capture const & cap )
            {
                BOOST_TEST_EQ(err<1>::count, 2);
                BOOST_TEST_EQ(err<2>::count, 1);
                BOOST_TEST_EQ(cap.size(), 0);
                BOOST_TEST(cap.empty());
            },
            []
            {
                BOOST_TEST(false);
            });
        BOOST_TEST_EQ(err<1>::count, 0);
        BOOST_TEST_EQ(err<2>::count, 0);
    }

    {
        leaf::try_handle_all(
            []() -> leaf::result<void>
            {
                return leaf::new_error(err<1>{}, err<2>{});
            },
            [&]( leaf::dynamic_capture const & cap )
            {
                BOOST_TEST_EQ(err<1>::count, 1);
                BOOST_TEST_EQ(err<2>::count, 1);
                BOOST_TEST_EQ(cap.size(), 2);
                BOOST_TEST(!cap.empty());
            } );
        BOOST_TEST_EQ(err<1>::count, 0);
        BOOST_TEST_EQ(err<2>::count, 0);
    }

    {
        leaf::result<void> r = leaf::try_handle_some(
            []() -> leaf::result<void>
            {
                return leaf::new_error(err<1>{}, err<2>{});
            },
            [&]( leaf::dynamic_capture const & cap ) -> leaf::result<void>
            {
                BOOST_TEST_EQ(err<1>::count, 1);
                BOOST_TEST_EQ(err<2>::count, 1);
                BOOST_TEST_EQ(cap.size(), 2);
                BOOST_TEST(!cap.empty());
                return cap;
            } );
        BOOST_TEST(!r);
        BOOST_TEST_EQ(err<1>::count, 1);
        BOOST_TEST_EQ(err<2>::count, 1);
        int r1 = leaf::try_handle_all(
            [&]() -> leaf::result<int>
            {
                BOOST_LEAF_CHECK(r);
                return 0;
            },
            [](err<1>, err<2>)
            {
                return 1;
            },
            []
            {
                return 2;
            } );
        BOOST_TEST_EQ(r1, 1);
    }
    BOOST_TEST_EQ(err<1>::count, 0);
    BOOST_TEST_EQ(err<2>::count, 0);

    {
        leaf::result<int> r = leaf::try_handle_some(
            []() -> leaf::result<int>
            {
                return leaf::new_error(err<1>{}, err<2>{});
            },
            [&]( leaf::dynamic_capture const & cap ) -> leaf::result<int>
            {
                BOOST_TEST_EQ(err<1>::count, 1);
                BOOST_TEST_EQ(err<2>::count, 1);
                BOOST_TEST_EQ(cap.size(), 2);
                BOOST_TEST(!cap.empty());
                return cap;
            } );
        BOOST_TEST(!r);
        BOOST_TEST_EQ(err<1>::count, 1);
        BOOST_TEST_EQ(err<2>::count, 1);
        int r1 = leaf::try_handle_all(
            [&]() -> leaf::result<int>
            {
                BOOST_LEAF_CHECK(r);
                return 0;
            },
            [](err<1>, err<2>)
            {
                return 1;
            },
            []
            {
                return 2;
            } );
        BOOST_TEST_EQ(r1, 1);
    }
    BOOST_TEST_EQ(err<1>::count, 0);
    BOOST_TEST_EQ(err<2>::count, 0);

    // nested/unload
    {
        leaf::result<void> r = leaf::try_handle_some(
            []() -> leaf::result<void>
            {
                return leaf::try_handle_some(
                    []() -> leaf::result<void>
                    {
                        return leaf::new_error(err<1>{}, err<2>{}, err<3>{});
                    },
                    []( err<3>, err<4> )
                    {
                        BOOST_TEST(false);
                    },
                    [&]( leaf::error_info const & ei, leaf::dynamic_capture const & cap )
                    {
                        BOOST_TEST_EQ(err<1>::count, 1);
                        BOOST_TEST_EQ(err<2>::count, 1);
                        BOOST_TEST_EQ(err<3>::count, 1);
                        BOOST_TEST_EQ(cap.size(), 2);
                        BOOST_TEST(!cap.empty());
                        err<1>::move_count = 0;
                        err<2>::move_count = 0;
                        return ei.error();
                    } );
            },
            []( leaf::dynamic_capture const & cap ) -> leaf::result<void>
            {
                BOOST_TEST_EQ(err<1>::count, 1);
                BOOST_TEST_EQ(err<2>::count, 1);
                BOOST_TEST_EQ(err<3>::count, 1);
                BOOST_TEST_EQ(cap.size(), 3);
                BOOST_TEST(!cap.empty());
                return cap;
            } );
        BOOST_TEST_EQ(err<1>::count, 1);
        BOOST_TEST_EQ(err<2>::count, 1);
        BOOST_TEST_EQ(err<3>::count, 1);
        BOOST_TEST_EQ(err<1>::move_count, 0);
        BOOST_TEST_EQ(err<2>::move_count, 0);
        BOOST_TEST(!r);
#if BOOST_LEAF_CFG_STD_STRING
        {
            std::ostringstream st;
            st << r;
            std::string s = st.str();
            std::cout << s << std::endl;
            if( BOOST_LEAF_CFG_DIAGNOSTICS )
            {
                auto n1 = s.find("err<1>");
                auto n3 = s.find("err<3>");
                BOOST_TEST_NE(n1, s.npos);
                BOOST_TEST_NE(n3, s.npos);
                BOOST_TEST_LT(n1, n3);
            }
        }
#endif
        int r1 = leaf::try_handle_all(
            [&]() -> leaf::result<int>
            {
                BOOST_LEAF_CHECK(r);
                return 0;
            },
            [](err<1>, err<2>)
            {
                return 1;
            },
            []
            {
                return 2;
            } );
        BOOST_TEST_EQ(r1, 1);
    }
    BOOST_TEST_EQ(err<1>::count, 0);
    BOOST_TEST_EQ(err<2>::count, 0);
    BOOST_TEST_EQ(err<3>::count, 0);

    // nested/unload, different order
    {
        leaf::result<void> r = leaf::try_handle_some(
            []() -> leaf::result<void>
            {
                return leaf::try_handle_some(
                    []() -> leaf::result<void>
                    {
                        return leaf::new_error(err<1>{}, err<2>{}, err<3>{});
                    },
                    [&]( leaf::error_info const & ei, leaf::dynamic_capture const & cap )
                    {
                        BOOST_TEST_EQ(err<1>::count, 1);
                        BOOST_TEST_EQ(err<2>::count, 1);
                        BOOST_TEST_EQ(err<3>::count, 1);
                        BOOST_TEST_EQ(cap.size(), 2);
                        BOOST_TEST(!cap.empty());
                        err<1>::move_count = 0;
                        err<2>::move_count = 0;
                        return ei.error();
                    },
                    []( err<3>, err<4> )
                    {
                        BOOST_TEST(false);
                    } );
            },
            []( leaf::dynamic_capture const & cap ) -> leaf::result<void>
            {
                BOOST_TEST_EQ(err<1>::count, 1);
                BOOST_TEST_EQ(err<2>::count, 1);
                BOOST_TEST_EQ(err<3>::count, 1);
                BOOST_TEST_EQ(cap.size(), 3);
                BOOST_TEST(!cap.empty());
                return cap;
            } );
        BOOST_TEST_EQ(err<1>::count, 1);
        BOOST_TEST_EQ(err<2>::count, 1);
        BOOST_TEST_EQ(err<3>::count, 1);
        BOOST_TEST_EQ(err<1>::move_count, 0);
        BOOST_TEST_EQ(err<2>::move_count, 0);
        BOOST_TEST(!r);
#if BOOST_LEAF_CFG_STD_STRING
        {
            std::ostringstream st;
            st << r;
            std::string s = st.str();
            std::cout << s << std::endl;
            if( BOOST_LEAF_CFG_DIAGNOSTICS )
            {
                auto n1 = s.find("err<1>");
                auto n3 = s.find("err<3>");
                BOOST_TEST_NE(n1, s.npos);
                BOOST_TEST_NE(n3, s.npos);
                BOOST_TEST_LT(n3, n1);
            }
        }
#endif
        int r1 = leaf::try_handle_all(
            [&]() -> leaf::result<int>
            {
                BOOST_LEAF_CHECK(r);
                return 0;
            },
            [](err<1>, err<2>)
            {
                return 1;
            },
            []
            {
                return 2;
            } );
        BOOST_TEST_EQ(r1, 1);
    }
    BOOST_TEST_EQ(err<1>::count, 0);
    BOOST_TEST_EQ(err<2>::count, 0);
    BOOST_TEST_EQ(err<3>::count, 0);

    return boost::report_errors();
}

#endif
