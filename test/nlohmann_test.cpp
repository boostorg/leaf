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
#   include <boost/leaf/serialization/nlohmann_writer.hpp>
#endif

#include "nlohmann/json.hpp"
#include <iomanip>
#include <iostream>
#include <stdexcept>
#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
#   include <system_error>
#endif

#include "lightweight_test.hpp"

#if BOOST_LEAF_CFG_STD_STRING

namespace leaf = boost::leaf;

using output_writer = leaf::serialization::nlohmann_writer<nlohmann::ordered_json>;

namespace boost { namespace leaf {

namespace serialization {

template <class Handle, class E>
void serialize(Handle & h, E const & e, char const * name)
{
    h.dispatch(
        [&](nlohmann_writer<nlohmann::json> & w) { write_nested(w, e, name); },
        [&](nlohmann_writer<nlohmann::ordered_json> & w) { write_nested(w, e, name); }
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

    template <class Json>
    friend void to_json(Json & j, my_error const & e)
    {
        j["code"] = e.code;
        j["message"] = e.message;
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

void check_diagnostic_info(nlohmann::ordered_json const & j, bool has_source_location)
{
    BOOST_TEST(j["boost::leaf::error_id"].get<int>() > 0);

    auto const & e1j = j["my_error<1>"];
    BOOST_TEST_EQ(e1j["code"].get<int>(), 1);
    BOOST_TEST_EQ(e1j["message"].get<std::string>(), "error one");

    if( has_source_location )
    {
        auto const & loc = j["boost::leaf::e_source_location"];
        BOOST_TEST(!loc["file"].get<std::string>().empty());
        BOOST_TEST(loc["line"].get<int>() > 0);
        BOOST_TEST(!loc["function"].get<std::string>().empty());
    }

    BOOST_TEST(!j.contains("my_error<2>"));
}

void check_diagnostic_details(nlohmann::ordered_json const & j, bool has_source_location)
{
    BOOST_TEST(j["boost::leaf::error_id"].get<int>() > 0);

    auto const & e1j = j["my_error<1>"];
    BOOST_TEST_EQ(e1j["code"].get<int>(), 1);
    BOOST_TEST_EQ(e1j["message"].get<std::string>(), "error one");

    if( has_source_location )
    {
        auto const & loc = j["boost::leaf::e_source_location"];
        BOOST_TEST(!loc["file"].get<std::string>().empty());
        BOOST_TEST(loc["line"].get<int>() > 0);
        BOOST_TEST(!loc["function"].get<std::string>().empty());
    }

    if( BOOST_LEAF_CFG_CAPTURE )
    {
        BOOST_TEST_EQ(j["int"].get<int>(), 42);

        auto const & e2j = j["my_error<2>"];
        BOOST_TEST_EQ(e2j["code"].get<int>(), 2);
        BOOST_TEST_EQ(e2j["message"].get<std::string>(), "error two");

        auto const & ej = j["boost::leaf::e_errno"];
        BOOST_TEST_EQ(ej["errno"].get<int>(), ENOENT);
        BOOST_TEST(!ej["strerror"].get<std::string>().empty());

        BOOST_TEST_EQ(j["boost::leaf::e_api_function"].get<std::string>(), "my_api_function");

        if( BOOST_LEAF_CFG_STD_SYSTEM_ERROR )
        {
            auto const & ecj = j["std::error_code"];
            BOOST_TEST_EQ(ecj["value"].get<int>(), static_cast<int>(std::errc::invalid_argument));
            BOOST_TEST(!ecj["category"].get<std::string>().empty());
            BOOST_TEST(!ecj["message"].get<std::string>().empty());

            auto const & econdj = j["std::error_condition"];
            BOOST_TEST_EQ(econdj["value"].get<int>(), static_cast<int>(std::errc::io_error));
            BOOST_TEST(!econdj["category"].get<std::string>().empty());
            BOOST_TEST(!econdj["message"].get<std::string>().empty());

            auto const & mecj = j["my_error_code"];
            BOOST_TEST_EQ(mecj["value"].get<int>(), static_cast<int>(std::errc::invalid_argument));
            BOOST_TEST(!mecj["category"].get<std::string>().empty());
            BOOST_TEST(!mecj["message"].get<std::string>().empty());
        }
    }
    else
    {
        BOOST_TEST(!j.contains("int"));
        BOOST_TEST(!j.contains("my_error<2>"));
        BOOST_TEST(!j.contains("boost::leaf::e_errno"));
        BOOST_TEST(!j.contains("boost::leaf::e_api_function"));
        BOOST_TEST(!j.contains("std::error_code"));
        BOOST_TEST(!j.contains("std::error_condition"));
    }
}

#ifndef BOOST_LEAF_NO_EXCEPTIONS
void check_exception(nlohmann::ordered_json const & j)
{
    auto const & exj = j["std::exception"];
    BOOST_TEST(!exj["dynamic_type"].get<std::string>().empty());
    BOOST_TEST(!exj["what"].get<std::string>().empty());
}
#endif

int main()
{
    {
        nlohmann::ordered_json j;
        leaf::try_handle_all(
            []
            {
                return fail();
            },
            [&j](leaf::diagnostic_info const & di, my_error<1> const * e1)
            {
                BOOST_TEST(e1 != nullptr);
                output_writer w{j};
                di.write_to(w);
            }
        );
        std::cout << __LINE__ << " diagnostic_info JSON output:\n" << std::setw(2) << j << std::endl;
        check_diagnostic_info(j, true);
    }

    {
        nlohmann::ordered_json j;
        leaf::try_handle_all(
            []
            {
                return fail();
            },
            [&j](leaf::diagnostic_details const & dd, my_error<1> const * e1)
            {
                BOOST_TEST(e1 != nullptr);
                output_writer w{j};
                dd.write_to(w);
            }
        );
        std::cout << __LINE__ << " diagnostic_details JSON output:\n" << std::setw(2) << j << std::endl;
        check_diagnostic_details(j, true);
    }

#ifndef BOOST_LEAF_NO_EXCEPTIONS
    {
        nlohmann::ordered_json j;
        leaf::try_catch(
            []
            {
                leaf_throw();
            },
            [&j](leaf::diagnostic_info const & di, my_error<1> const * e1)
            {
                BOOST_TEST(e1 != nullptr);
                output_writer w{j};
                di.write_to(w);
            }
        );
        std::cout << __LINE__ << " leaf_throw diagnostic_info JSON output:\n" << std::setw(2) << j << std::endl;
        check_diagnostic_info(j, true);
        check_exception(j);
    }

    {
        nlohmann::ordered_json j;
        leaf::try_catch(
            []
            {
                leaf_throw();
            },
            [&j](leaf::diagnostic_details const & dd, my_error<1> const * e1)
            {
                BOOST_TEST(e1 != nullptr);
                output_writer w{j};
                dd.write_to(w);
            }
        );
        std::cout << __LINE__ << " leaf_throw diagnostic_details JSON output:\n" << std::setw(2) << j << std::endl;
        check_diagnostic_details(j, true);
        check_exception(j);
    }

    {
        nlohmann::ordered_json j;
        leaf::try_catch(
            []
            {
                throw_();
            },
            [&j](leaf::diagnostic_info const & di, my_error<1> const * e1)
            {
                BOOST_TEST(e1 != nullptr);
                output_writer w{j};
                di.write_to(w);
            }
        );
        std::cout << __LINE__ << " throw_ diagnostic_info JSON output:\n" << std::setw(2) << j << std::endl;
        check_diagnostic_info(j, false);
    }

    {
        nlohmann::ordered_json j;
        leaf::try_catch(
            []
            {
                throw_();
            },
            [&j](leaf::diagnostic_details const & dd, my_error<1> const * e1)
            {
                BOOST_TEST(e1 != nullptr);
                output_writer w{j};
                dd.write_to(w);
            }
        );
        std::cout << __LINE__ << " throw_ diagnostic_details JSON output:\n" << std::setw(2) << j << std::endl;
        check_diagnostic_details(j, false);
    }

    {
        nlohmann::ordered_json j;
        leaf::try_handle_all(
            []() -> leaf::result<void>
            {
                return leaf::new_error(my_exception_ptr{std::make_exception_ptr(std::runtime_error("test exception"))});
            },
            [&j](leaf::diagnostic_details const & dd, my_exception_ptr *)
            {
                output_writer w{j};
                dd.write_to(w);
            }
        );
        std::cout << __LINE__ << " std::exception_ptr JSON output:\n" << std::setw(2) << j << std::endl;

        auto const & ep = j["my_exception_ptr"];
        std::string type = ep["dynamic_type"].get<std::string>();
        std::string what = ep["what"].get<std::string>();
        BOOST_TEST(type.find("std::runtime_error") != std::string::npos);
        BOOST_TEST_EQ(what, "test exception");
    }

    {
        nlohmann::ordered_json j;
        leaf::try_handle_all(
            []() -> leaf::result<void>
            {
                return leaf::new_error(my_exception_ptr{std::make_exception_ptr(42)});
            },
            [&j](leaf::diagnostic_details const & dd, my_exception_ptr *)
            {
                output_writer w{j};
                dd.write_to(w);
            }
        );
        std::cout << __LINE__ << " non-std::exception_ptr JSON output:\n" << std::setw(2) << j << std::endl;

        auto const & ep = j["my_exception_ptr"];
        std::string type = ep["dynamic_type"].get<std::string>();
        std::string what = ep["what"].get<std::string>();
        BOOST_TEST_EQ(type, "<<unknown>>");
        BOOST_TEST_EQ(what, "N/A");
    }
#endif

    {
        nlohmann::ordered_json j;
        leaf::result<int> r = 42;
        BOOST_TEST(r);
        output_writer w{j};
        r.write_to(w);
        std::cout << __LINE__ << " result<int> success JSON output:\n" << std::setw(2) << j << std::endl;
        BOOST_TEST_EQ(j["int"].get<int>(), 42);
    }

    {
        nlohmann::ordered_json j;
        leaf::result<int> r = leaf::new_error();
        BOOST_TEST(!r);
        output_writer w{j};
        r.write_to(w);
        std::cout << __LINE__ << " result<int> error JSON output:\n" << std::setw(2) << j << std::endl;
        BOOST_TEST(j["boost::leaf::error_id"].get<int>() > 0);
    }

#if BOOST_LEAF_CFG_CAPTURE
    {
        nlohmann::ordered_json j;
        leaf::result<int> r = leaf::try_capture_all(
            []() -> leaf::result<int>
            {
                return leaf::new_error(my_error<1>{1, "error one"}, my_error<2>{2, "error two"});
            } );
        BOOST_TEST(!r);
        output_writer w{j};
        r.write_to(w);
        std::cout << __LINE__ << " result<int> captured error JSON output:\n" << std::setw(2) << j << std::endl;
        BOOST_TEST(j["boost::leaf::error_id"].get<int>() > 0);
        auto const & e1j = j["my_error<1>"];
        BOOST_TEST_EQ(e1j["code"].get<int>(), 1);
        BOOST_TEST_EQ(e1j["message"].get<std::string>(), "error one");
        auto const & e2j = j["my_error<2>"];
        BOOST_TEST_EQ(e2j["code"].get<int>(), 2);
        BOOST_TEST_EQ(e2j["message"].get<std::string>(), "error two");
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
