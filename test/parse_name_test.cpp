// Copyright 2018-2024 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef BOOST_LEAF_TEST_SINGLE_HEADER
#   include "leaf.hpp"
#else
#   include <boost/leaf/detail/demangle.hpp>
#endif

#if BOOST_LEAF_CFG_STD_STRING

namespace leaf = boost::leaf;

#include "lightweight_test.hpp"

namespace leaf_test
{
    class class_ { };
    struct struct_ { };
    enum enum_ { };
    template <int> class class_template1 { };
    template <int> struct struct_template1 { };
    template <class> class class_template2 { };
    template <class> struct struct_template2 { };
}

class class_ { };
struct struct_ { };
enum enum_ { };
template <int> class class_template1 { };
template <int> struct struct_template1 { };
template <class> class class_template2 { };
template <class> struct struct_template2 { };

namespace
{
    template <class Name>
    void test_name(std::string const & correct)
    {
        leaf::parsed_name pn = leaf::parse_name<Name>();
        BOOST_TEST(pn.parse_success());
        BOOST_TEST_EQ(std::string(pn.name, pn.len), correct);
    }
}

int main()
{
    test_name<leaf_test::class_>("leaf_test::class_");
    test_name<leaf_test::struct_>("leaf_test::struct_");
    test_name<leaf_test::enum_>("leaf_test::enum_");
    test_name<leaf_test::class_template1<42>>("leaf_test::class_template1<42>");
    test_name<leaf_test::struct_template1<42>>("leaf_test::struct_template1<42>");
    test_name<leaf_test::class_template2<int>>("leaf_test::class_template2<int>");
    test_name<leaf_test::struct_template2<int>>("leaf_test::struct_template2<int>");

    test_name<class_>("class_");
    test_name<struct_>("struct_");
    test_name<enum_>("enum_");
    test_name<class_template1<42>>("class_template1<42>");
    test_name<struct_template1<42>>("struct_template1<42>");
    test_name<class_template2<int>>("class_template2<int>");
    test_name<struct_template2<int>>("struct_template2<int>");

    return boost::report_errors();
}

#else

#include <iostream>

int main()
{
    std::cout << "Unit test not applicable." << std::endl;
    return 0;
}

#endif
