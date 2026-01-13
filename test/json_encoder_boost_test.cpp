// Copyright 2018-2026 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef BOOST_LEAF_TEST_SINGLE_HEADER
#   include "leaf.hpp"
#else
#   include <boost/leaf/handle_errors.hpp>
#   include <boost/leaf/result.hpp>
#   include <boost/leaf/diagnostics.hpp>
#   include <boost/leaf/common.hpp>
#   include <boost/leaf/on_error.hpp>
#   include <boost/leaf/serialization/json_encoder_boost.hpp>
#endif

#include <boost/json.hpp>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
#   include <system_error>
#endif

#include "lightweight_test.hpp"

#if BOOST_LEAF_CFG_STD_STRING

namespace leaf = boost::leaf;

using output_encoder = leaf::serialization::json_encoder_boost;

namespace boost { namespace leaf {

namespace serialization {

template <class Handle, class T>
void serialize(Handle & h, T const & x, char const * name)
{
    h.dispatch(
        [&](json_encoder_boost & e) { output_at(e, x, name); }
    );
}

}

} }

struct my_exception { };

struct my_exception_ptr
{
    std::exception_ptr value;
};

struct my_error_code
{
    std::error_code value;
};

template <int N>
struct my_error
{
    int code;
    char const * message;

    friend void tag_invoke(boost::json::value_from_tag, boost::json::value & jv, my_error const & e)
    {
        jv.emplace_object();
        jv.as_object()["code"] = e.code;
        jv.as_object()["message"] = e.message;
    }

    friend std::ostream & operator<<(std::ostream & os, my_error const & e)
    {
        return os << "code=" << e.code << ", message=" << e.message;
    }
};

leaf::result<void> fail()
{
    return BOOST_LEAF_NEW_ERROR(
#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
        std::make_error_code(std::errc::invalid_argument),
        std::make_error_condition(std::errc::io_error),
        my_error_code{std::make_error_code(std::errc::invalid_argument)},
#endif
        42,
        my_error<1>{1, "error one"},
        my_error<2>{2, "error two"},
        leaf::e_errno{ENOENT},
        leaf::e_api_function{"my_api_function"} );
}

#ifndef BOOST_LEAF_NO_EXCEPTIONS
void leaf_throw()
{
    BOOST_LEAF_THROW_EXCEPTION(
#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
        std::make_error_code(std::errc::invalid_argument),
        std::make_error_condition(std::errc::io_error),
        my_error_code{std::make_error_code(std::errc::invalid_argument)},
#endif
        42,
        my_error<1>{1, "error one"},
        my_error<2>{2, "error two"},
        leaf::e_errno{ENOENT},
        leaf::e_api_function{"my_api_function"} );
}

void throw_()
{
    auto load = leaf::on_error(
#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
        std::make_error_code(std::errc::invalid_argument),
        std::make_error_condition(std::errc::io_error),
        my_error_code{std::make_error_code(std::errc::invalid_argument)},
#endif
        42,
        my_error<1>{1, "error one"},
        my_error<2>{2, "error two"},
        leaf::e_errno{ENOENT},
        leaf::e_api_function{"my_api_function"} );
    throw my_exception{};
}
#endif

void check_diagnostic_info(boost::json::value const & j, bool has_source_location)
{
    BOOST_TEST(boost::json::value_to<int>(j.at("boost::leaf::error_id")) > 0);

    auto const & e1j = j.at("my_error<1>");
    BOOST_TEST_EQ(boost::json::value_to<int>(e1j.at("code")), 1);
    BOOST_TEST_EQ(boost::json::value_to<std::string>(e1j.at("message")), "error one");

    if( has_source_location )
    {
        auto const & loc = j.at("boost::leaf::e_source_location");
        BOOST_TEST(!boost::json::value_to<std::string>(loc.at("file")).empty());
        BOOST_TEST(boost::json::value_to<int>(loc.at("line")) > 0);
        BOOST_TEST(!boost::json::value_to<std::string>(loc.at("function")).empty());
    }

    BOOST_TEST(j.as_object().find("my_error<2>") == j.as_object().end());
}

void check_diagnostic_details(boost::json::value const & j, bool has_source_location)
{
    BOOST_TEST(boost::json::value_to<int>(j.at("boost::leaf::error_id")) > 0);

    auto const & e1j = j.at("my_error<1>");
    BOOST_TEST_EQ(boost::json::value_to<int>(e1j.at("code")), 1);
    BOOST_TEST_EQ(boost::json::value_to<std::string>(e1j.at("message")), "error one");

    if( has_source_location )
    {
        auto const & loc = j.at("boost::leaf::e_source_location");
        BOOST_TEST(!boost::json::value_to<std::string>(loc.at("file")).empty());
        BOOST_TEST(boost::json::value_to<int>(loc.at("line")) > 0);
        BOOST_TEST(!boost::json::value_to<std::string>(loc.at("function")).empty());
    }

    if( BOOST_LEAF_CFG_CAPTURE )
    {
        BOOST_TEST_EQ(boost::json::value_to<int>(j.at("int")), 42);

        auto const & e2j = j.at("my_error<2>");
        BOOST_TEST_EQ(boost::json::value_to<int>(e2j.at("code")), 2);
        BOOST_TEST_EQ(boost::json::value_to<std::string>(e2j.at("message")), "error two");

        auto const & ej = j.at("boost::leaf::e_errno");
        BOOST_TEST_EQ(boost::json::value_to<int>(ej.at("errno")), ENOENT);
        BOOST_TEST(!boost::json::value_to<std::string>(ej.at("strerror")).empty());

        BOOST_TEST_EQ(boost::json::value_to<std::string>(j.at("boost::leaf::e_api_function")), "my_api_function");

        if( BOOST_LEAF_CFG_STD_SYSTEM_ERROR )
        {
            auto const & ecj = j.at("std::error_code");
            BOOST_TEST_EQ(boost::json::value_to<int>(ecj.at("value")), static_cast<int>(std::errc::invalid_argument));
            BOOST_TEST(!boost::json::value_to<std::string>(ecj.at("category")).empty());
            BOOST_TEST(!boost::json::value_to<std::string>(ecj.at("message")).empty());

            auto const & econdj = j.at("std::error_condition");
            BOOST_TEST_EQ(boost::json::value_to<int>(econdj.at("value")), static_cast<int>(std::errc::io_error));
            BOOST_TEST(!boost::json::value_to<std::string>(econdj.at("category")).empty());
            BOOST_TEST(!boost::json::value_to<std::string>(econdj.at("message")).empty());

            auto const & mecj = j.at("my_error_code");
            BOOST_TEST_EQ(boost::json::value_to<int>(mecj.at("value")), static_cast<int>(std::errc::invalid_argument));
            BOOST_TEST(!boost::json::value_to<std::string>(mecj.at("category")).empty());
            BOOST_TEST(!boost::json::value_to<std::string>(mecj.at("message")).empty());
        }
    }
    else
    {
        BOOST_TEST(j.as_object().find("int") == j.as_object().end());
        BOOST_TEST(j.as_object().find("my_error<2>") == j.as_object().end());
        BOOST_TEST(j.as_object().find("boost::leaf::e_errno") == j.as_object().end());
        BOOST_TEST(j.as_object().find("boost::leaf::e_api_function") == j.as_object().end());
        BOOST_TEST(j.as_object().find("std::error_code") == j.as_object().end());
        BOOST_TEST(j.as_object().find("std::error_condition") == j.as_object().end());
    }
}

#ifndef BOOST_LEAF_NO_EXCEPTIONS
void check_exception(boost::json::value const & j)
{
    auto const & exj = j.at("std::exception");
    BOOST_TEST(!boost::json::value_to<std::string>(exj.at("dynamic_type")).empty());
    BOOST_TEST(!boost::json::value_to<std::string>(exj.at("what")).empty());
}
#endif

int main()
{
    {
        boost::json::value j;
        leaf::try_handle_all(
            []
            {
                return fail();
            },
            [&j](leaf::diagnostic_info const & di, my_error<1> const * e1)
            {
                BOOST_TEST(e1 != nullptr);
                output_encoder e{j};
                di.output_to(e);
            }
        );
        std::cout << __LINE__ << " diagnostic_info JSON output:\n" << boost::json::serialize(j) << std::endl;
        check_diagnostic_info(j, true);
    }

    {
        boost::json::value j;
        leaf::try_handle_all(
            []
            {
                return fail();
            },
            [&j](leaf::diagnostic_details const & dd, my_error<1> const * e1)
            {
                BOOST_TEST(e1 != nullptr);
                output_encoder e{j};
                dd.output_to(e);
            }
        );
        std::cout << __LINE__ << " diagnostic_details JSON output:\n" << boost::json::serialize(j) << std::endl;
        check_diagnostic_details(j, true);
    }

#ifndef BOOST_LEAF_NO_EXCEPTIONS
    {
        boost::json::value j;
        leaf::try_catch(
            []
            {
                leaf_throw();
            },
            [&j](leaf::diagnostic_info const & di, my_error<1> const * e1)
            {
                BOOST_TEST(e1 != nullptr);
                output_encoder e{j};
                di.output_to(e);
            }
        );
        std::cout << __LINE__ << " leaf_throw diagnostic_info JSON output:\n" << boost::json::serialize(j) << std::endl;
        check_diagnostic_info(j, true);
        check_exception(j);
    }

    {
        boost::json::value j;
        leaf::try_catch(
            []
            {
                leaf_throw();
            },
            [&j](leaf::diagnostic_details const & dd, my_error<1> const * e1)
            {
                BOOST_TEST(e1 != nullptr);
                output_encoder e{j};
                dd.output_to(e);
            }
        );
        std::cout << __LINE__ << " leaf_throw diagnostic_details JSON output:\n" << boost::json::serialize(j) << std::endl;
        check_diagnostic_details(j, true);
        check_exception(j);
    }

    {
        boost::json::value j;
        leaf::try_catch(
            []
            {
                throw_();
            },
            [&j](leaf::diagnostic_info const & di, my_error<1> const * e1)
            {
                BOOST_TEST(e1 != nullptr);
                output_encoder e{j};
                di.output_to(e);
            }
        );
        std::cout << __LINE__ << " throw_ diagnostic_info JSON output:\n" << boost::json::serialize(j) << std::endl;
        check_diagnostic_info(j, false);
    }

    {
        boost::json::value j;
        leaf::try_catch(
            []
            {
                throw_();
            },
            [&j](leaf::diagnostic_details const & dd, my_error<1> const * e1)
            {
                BOOST_TEST(e1 != nullptr);
                output_encoder e{j};
                dd.output_to(e);
            }
        );
        std::cout << __LINE__ << " throw_ diagnostic_details JSON output:\n" << boost::json::serialize(j) << std::endl;
        check_diagnostic_details(j, false);
    }

    {
        boost::json::value j;
        leaf::try_handle_all(
            []() -> leaf::result<void>
            {
                return leaf::new_error(my_exception_ptr{std::make_exception_ptr(std::runtime_error("test exception"))});
            },
            [&j](leaf::diagnostic_details const & dd, my_exception_ptr *)
            {
                output_encoder e{j};
                dd.output_to(e);
            }
        );
        std::cout << __LINE__ << " std::exception_ptr JSON output:\n" << boost::json::serialize(j) << std::endl;

        auto const & ep = j.at("my_exception_ptr");
        auto type = boost::json::value_to<std::string>(ep.at("dynamic_type"));
        auto what = boost::json::value_to<std::string>(ep.at("what"));
        BOOST_TEST(type.find("std::runtime_error") != std::string::npos);
        BOOST_TEST_EQ(what, "test exception");
    }

    {
        boost::json::value j;
        leaf::try_handle_all(
            []() -> leaf::result<void>
            {
                return leaf::new_error(my_exception_ptr{std::make_exception_ptr(42)});
            },
            [&j](leaf::diagnostic_details const & dd, my_exception_ptr *)
            {
                output_encoder e{j};
                dd.output_to(e);
            }
        );
        std::cout << __LINE__ << " non-std::exception_ptr JSON output:\n" << boost::json::serialize(j) << std::endl;

        auto const & ep = j.at("my_exception_ptr");
        auto type = boost::json::value_to<std::string>(ep.at("dynamic_type"));
        auto what = boost::json::value_to<std::string>(ep.at("what"));
        BOOST_TEST_EQ(type, "<<unknown>>");
        BOOST_TEST_EQ(what, "N/A");
    }
#endif

    {
        boost::json::value j;
        leaf::result<int> r = 42;
        BOOST_TEST(r);
        output_encoder e{j};
        r.output_to(e);
        std::cout << __LINE__ << " result<int> success JSON output:\n" << boost::json::serialize(j) << std::endl;
        BOOST_TEST_EQ(boost::json::value_to<int>(j.at("int")), 42);
    }

    {
        boost::json::value j;
        leaf::result<int> r = leaf::new_error();
        BOOST_TEST(!r);
        output_encoder e{j};
        r.output_to(e);
        std::cout << __LINE__ << " result<int> error JSON output:\n" << boost::json::serialize(j) << std::endl;
        BOOST_TEST(boost::json::value_to<int>(j.at("boost::leaf::error_id")) > 0);
    }

#if BOOST_LEAF_CFG_CAPTURE
    {
        boost::json::value j;
        leaf::result<int> r = leaf::try_capture_all(
            []() -> leaf::result<int>
            {
                return leaf::new_error(my_error<1>{1, "error one"}, my_error<2>{2, "error two"});
            } );
        BOOST_TEST(!r);
        output_encoder e{j};
        r.output_to(e);
        std::cout << __LINE__ << " result<int> captured error JSON output:\n" << boost::json::serialize(j) << std::endl;
        BOOST_TEST(boost::json::value_to<int>(j.at("boost::leaf::error_id")) > 0);
        auto const & e1j = j.at("my_error<1>");
        BOOST_TEST_EQ(boost::json::value_to<int>(e1j.at("code")), 1);
        BOOST_TEST_EQ(boost::json::value_to<std::string>(e1j.at("message")), "error one");
        auto const & e2j = j.at("my_error<2>");
        BOOST_TEST_EQ(boost::json::value_to<int>(e2j.at("code")), 2);
        BOOST_TEST_EQ(boost::json::value_to<std::string>(e2j.at("message")), "error two");
    }
#endif

    return boost::report_errors();
}

#else

int main()
{
    return 0;
}

#endif
