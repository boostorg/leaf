// Copyright 2018-2023 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>

#if !BOOST_LEAF_CFG_DIAGNOSTICS || !BOOST_LEAF_CFG_STD_STRING

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
#   include <boost/leaf/detail/print.hpp>
#endif

#include <sstream>
#include <iostream>

#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

struct c0
{
    friend std::ostream & operator<<( std::ostream & os, c0 const & )
    {
        return os << "info";
    }
};

struct c1
{
    int value;

    friend std::ostream & operator<<( std::ostream & os, c1 const & x )
    {
        return os << "value " << x.value;
    }
};

struct c2
{
    int value;
};

std::ostream & operator<<( std::ostream & os, c2 const & x )
{
    return os << "value " << x.value;
}

struct c3
{
    int value;
};

struct c4
{
    struct unprintable { };
    unprintable value;;
};

template <int Line, class T>
std::string print(T const & x, char const * prefix, char const * delimiter)
{
    using namespace leaf::leaf_detail;
    std::ostringstream s;
    diagnostic<T>::print(s, prefix, delimiter, x);
    std::string q = s.str();
    std::cout << "[LINE " << Line << "] " << q << '\n';
    return q;
}

struct my_exception: std::exception
{
    char const * what() const noexcept override { return "my_exception what"; }
};

int main()
{
    {
        std::string out = print<__LINE__>(c0{}, "Title", " --> ");
        BOOST_TEST_NE(out.find("Title --> "), out.npos);
        BOOST_TEST_NE(out.find("c0"), out.npos);
        BOOST_TEST_NE(out.find(": info"), out.npos);
    }
    {
        std::string out = print<__LINE__>(c1{42}, "Title", " --> ");
        BOOST_TEST_NE(out.find("Title --> "), out.npos);
        BOOST_TEST_NE(out.find("c1"), out.npos);
        BOOST_TEST_NE(out.find(": value 42"), out.npos);
    }
    {
        std::string out = print<__LINE__>(c2{42}, "Title", " --> ");
        BOOST_TEST_NE(out.find("Title --> "), out.npos);
        BOOST_TEST_NE(out.find("c2"), out.npos);
        BOOST_TEST_NE(out.find(": value 42"), out.npos);
    }
    {
        std::string out = print<__LINE__>(c3{42}, "Title", " --> ");
        BOOST_TEST_NE(out.find("Title --> "), out.npos);
        BOOST_TEST_NE(out.find("c3"), out.npos);
        BOOST_TEST_NE(out.find(": 42"), out.npos);
    }
    {
        std::string out = print<__LINE__>(c4{}, "Title", " --> ");
        BOOST_TEST_NE(out.find("Title --> "), out.npos);
        BOOST_TEST_NE(out.find("c4"), out.npos);
        BOOST_TEST_EQ(out.find(": "), out.npos);
    }
    {
        std::string out = print<__LINE__>(my_exception{}, "Title", " --> ");
        BOOST_TEST_NE(out.find("Title --> "), out.npos);
        BOOST_TEST_NE(out.find("my_exception, std::exception::what(): my_exception what"), out.npos);
    }
    return boost::report_errors();
}

#endif
