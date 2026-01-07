// Copyright 2018-2026 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef BOOST_LEAF_TEST_SINGLE_HEADER
#   include "leaf.hpp"
#else
#   include <boost/leaf/serialization/type_name.hpp>
#endif

#include <cstring>
#include <cstdint>
#include <unordered_set>

#if __cplusplus >= 201703L
#   include <string_view>
#endif

namespace leaf = boost::leaf;
namespace serialization = boost::leaf::serialization;

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

namespace boost { namespace leaf {
    struct in_namespace_boost_leaf { };
} }

class class_ { };
struct struct_ { };
enum enum_ { };
template <int> class class_template1 { };
template <int> struct struct_template1 { };
template <class> class class_template2 { };
template <class> struct struct_template2 { };

bool test(serialization::type_name const & tn, char const * correct)
{
    return
        std::strlen(correct) == tn.length &&
        std::memcmp(correct, tn.name_not_zero_terminated_at_length, tn.length) == 0;
}

int main()
{
    using serialization::get_type_name;

    BOOST_TEST(test(get_type_name<leaf::in_namespace_boost_leaf>(), "boost::leaf::in_namespace_boost_leaf"));

    BOOST_TEST(test(get_type_name<int>(), "int"));

    BOOST_TEST(test(get_type_name<leaf_test::class_>(), "leaf_test::class_"));
    BOOST_TEST(test(get_type_name<leaf_test::struct_>(), "leaf_test::struct_"));
    BOOST_TEST(test(get_type_name<leaf_test::enum_>(), "leaf_test::enum_"));
    BOOST_TEST(test(get_type_name<leaf_test::class_template1<42>>(), "leaf_test::class_template1<42>"));
    BOOST_TEST(test(get_type_name<leaf_test::struct_template1<42>>(), "leaf_test::struct_template1<42>"));
    BOOST_TEST(test(get_type_name<leaf_test::class_template2<int>>(), "leaf_test::class_template2<int>"));
    BOOST_TEST(test(get_type_name<leaf_test::struct_template2<int>>(), "leaf_test::struct_template2<int>"));

    BOOST_TEST(test(get_type_name<class_>(), "class_"));
    BOOST_TEST(test(get_type_name<struct_>(), "struct_"));
    BOOST_TEST(test(get_type_name<enum_>(), "enum_"));
    BOOST_TEST(test(get_type_name<class_template1<42>>(), "class_template1<42>"));
    BOOST_TEST(test(get_type_name<struct_template1<42>>(), "struct_template1<42>"));
    BOOST_TEST(test(get_type_name<class_template2<int>>(), "class_template2<int>"));
    BOOST_TEST(test(get_type_name<struct_template2<int>>(), "struct_template2<int>"));

    BOOST_TEST(get_type_name<int>() == get_type_name<int>());
    BOOST_TEST(get_type_name<class_>() == get_type_name<class_>());
    BOOST_TEST(!(get_type_name<int>() == get_type_name<float>()));
    BOOST_TEST(!(get_type_name<class_>() == get_type_name<struct_>()));

    BOOST_TEST(!(get_type_name<int>() != get_type_name<int>()));
    BOOST_TEST(get_type_name<int>() != get_type_name<float>());
    BOOST_TEST(get_type_name<class_>() != get_type_name<struct_>());

    BOOST_TEST(get_type_name<class_>() < get_type_name<struct_>()); // "class_" < "struct_"
    BOOST_TEST(!(get_type_name<struct_>() < get_type_name<class_>()));
    BOOST_TEST(!(get_type_name<int>() < get_type_name<int>())); // not less than itself
    BOOST_TEST(get_type_name<int>() < get_type_name<long>()); // "int" < "long"

#if __cplusplus >= 201703L
    {
        auto sv = to_string_view(get_type_name<int>());
        BOOST_TEST(sv == "int");
    }
    {
        auto sv = to_string_view(get_type_name<class_>());
        BOOST_TEST(sv == "class_");
    }
    {
        auto sv = to_string_view(get_type_name<leaf_test::struct_>());
        BOOST_TEST(sv == "leaf_test::struct_");
    }
#endif

    BOOST_TEST(std::hash<serialization::type_name>()(get_type_name<int>()) == get_type_name<int>().hash);
    BOOST_TEST(std::hash<serialization::type_name>()(get_type_name<int>()) == std::hash<serialization::type_name>()(get_type_name<int>()));
    {
        std::unordered_set<serialization::type_name> s;
        s.insert(get_type_name<int>());
        s.insert(get_type_name<float>());
        s.insert(get_type_name<int>()); // duplicate
        BOOST_TEST(s.size() == 2);
        BOOST_TEST(s.count(get_type_name<int>()) == 1);
        BOOST_TEST(s.count(get_type_name<float>()) == 1);
        BOOST_TEST(s.count(get_type_name<double>()) == 0);
    }

    return boost::report_errors();
}
