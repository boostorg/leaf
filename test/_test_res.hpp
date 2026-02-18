#ifndef BOOST_LEAF_TEST_RES_HPP_INCLUDED
#define BOOST_LEAF_TEST_RES_HPP_INCLUDED

// Copyright 2018-2024 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "_test_ec.hpp"
#include <utility>

struct test_error
{
    int value;
    bool moved;
    test_error() noexcept: value(0), moved(false) {}
    explicit test_error(int v) noexcept: value(v), moved(false) {}
    test_error(test_error const & o) noexcept: value(o.value), moved(false) {}
    test_error(test_error && o) noexcept: value(o.value), moved(true) {}
    test_error & operator=(test_error const &) = default;
    test_error & operator=(test_error &&) = default;
};

template <class T, class E>
class test_res
{
    enum class variant
    {
        value,
        error
    };
    T value_;
    E error_;
    variant which_;
public:
    test_res( T const & value ) noexcept:
        value_(value),
        error_(),
        which_(variant::value)
    {
    }
    test_res( E const & error ) noexcept:
        value_(),
        error_(error),
        which_(variant::error)
    {
    }
    test_res( E && error ) noexcept:
        value_(),
        error_(std::move(error)),
        which_(variant::error)
    {
    }
    template <class Enum>
    test_res( Enum e, typename std::enable_if<std::is_error_code_enum<Enum>::value, Enum>::type * = nullptr ):
        value_(),
        error_(make_error_code(e)),
        which_(variant::error)
    {
    }
    explicit operator bool() const noexcept
    {
        return which_ == variant::value;
    }
    T const & value() const
    {
        BOOST_LEAF_ASSERT(which_ == variant::value);
        return value_;
    }
#ifndef BOOST_LEAF_NO_CXX11_REF_QUALIFIERS
    E const & error() const &
    {
        BOOST_LEAF_ASSERT(which_ == variant::error);
        return error_;
    }
    E && error() &&
    {
        BOOST_LEAF_ASSERT(which_ == variant::error);
        return std::move(error_);
    }
#else
    E const & error() const
    {
        BOOST_LEAF_ASSERT(which_ == variant::error);
        return error_;
    }
#endif
};

template <class E>
class test_res<void, E>
{
    enum class variant
    {
        value,
        error
    };
    E error_;
    variant which_;
public:
    test_res() noexcept:
        error_(),
        which_(variant::value)
    {
    }
    test_res( E const & error ) noexcept:
        error_(error),
        which_(variant::error)
    {
    }
    test_res( E && error ) noexcept:
        error_(std::move(error)),
        which_(variant::error)
    {
    }
    template <class Enum>
    test_res( Enum e, typename std::enable_if<std::is_error_code_enum<Enum>::value, Enum>::type * = nullptr ):
        error_(make_error_code(e)),
        which_(variant::error)
    {
    }
    explicit operator bool() const noexcept
    {
        return which_ == variant::value;
    }
    void value() const
    {
        BOOST_LEAF_ASSERT(which_ == variant::value);
    }
#ifndef BOOST_LEAF_NO_CXX11_REF_QUALIFIERS
    E const & error() const &
    {
        BOOST_LEAF_ASSERT(which_ == variant::error);
        return error_;
    }
    E && error() &&
    {
        BOOST_LEAF_ASSERT(which_ == variant::error);
        return std::move(error_);
    }
#else
    E const & error() const
    {
        BOOST_LEAF_ASSERT(which_ == variant::error);
        return error_;
    }
#endif
};

namespace boost { namespace leaf {

    template <class T>
    struct is_result_type;

    template <class T, class E>
    struct is_result_type<test_res<T, E>>: std::true_type
    {
    };

} }

#endif
