// Copyright (c) 2018-2020 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/config.hpp>
#ifdef BOOST_LEAF_NO_EXCEPTIONS

#include <iostream>

int main()
{
    std::cout << "Unit test not applicable." << std::endl;
    return 0;
}

#else

#include <boost/leaf/handle_errors.hpp>
#include <boost/leaf/exception.hpp>
#include <boost/leaf/pred.hpp>
#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

struct info { int value; };

int main()
{
    {
        int r = leaf::try_catch(
            []() -> int
            {
                throw leaf::exception( std::system_error(EDOM, std::generic_category(), "hello world"), info{42} );
            },
            []( std::system_error const & se, info x )
            {
                return 1;
            },
            []
            {
                return 2;
            } );
        BOOST_TEST_EQ(r, 1);
    }
    {
        int r = leaf::try_catch(
            []() -> int
            {
                auto load = leaf::on_error(info{42});
                throw std::system_error(EDOM, std::generic_category(), "hello world");
            },
            []( std::system_error const & se, info x )
            {
                return 1;
            },
            []
            {
                return 2;
            } );
        BOOST_TEST_EQ(r, 1);
    }
    return boost::report_errors();
}

#endif
