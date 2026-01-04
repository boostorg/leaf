// Copyright 2018-2025 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>

#if !BOOST_LEAF_CFG_DIAGNOSTICS

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
#   include <boost/leaf/handle_errors.hpp>
#   include <boost/leaf/result.hpp>
#   include <boost/leaf/diagnostics.hpp>
#   include <boost/leaf/common.hpp>
#   include <boost/leaf/on_error.hpp>
#endif
#include "nlohmann/json.hpp"
#include <iomanip>
#include <iostream>
#include <system_error>
#include <stdexcept>

#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

class nlohmann_writer: public leaf::writer
{
    nlohmann::ordered_json & json_;

public:

    nlohmann_writer(nlohmann::ordered_json & j, leaf::error_id id) noexcept:
        writer(this),
        json_(j)
    {
        json_["error_id"] = id.value();
    }

    template <class E>
    void write(E const & e)
    {
        using namespace leaf;
        auto & obj = json_[to_string(parse<E>())];
        to_json(obj, e);
    }
};

namespace boost { namespace leaf {

template <class E>
void serialize(writer & w, E const & e)
{
    if( nlohmann_writer * nw = w.get<nlohmann_writer>() )
        nw->write(e);
    else if( ostream_writer * ow = w.get<ostream_writer>() )
        ow->write(e);
}

} } // namespace boost::leaf

struct my_exception { };

struct my_exception_ptr
{
    std::exception_ptr value;
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
        my_error<1>{1, "error one"},
        my_error<2>{2, "error two"},
        leaf::e_errno{ENOENT},
        leaf::e_api_function{"my_api_function"},
        std::make_error_code(std::errc::invalid_argument) );
}

#ifndef BOOST_LEAF_NO_EXCEPTIONS
void leaf_throw()
{
    BOOST_LEAF_THROW_EXCEPTION(
        my_error<1>{1, "error one"},
        my_error<2>{2, "error two"},
        leaf::e_errno{ENOENT},
        leaf::e_api_function{"my_api_function"},
        std::make_error_code(std::errc::invalid_argument) );
}

void throw_()
{
    auto load = leaf::on_error(
        my_error<1>{1, "error one"},
        my_error<2>{2, "error two"},
        leaf::e_errno{ENOENT},
        leaf::e_api_function{"my_api_function"},
        std::make_error_code(std::errc::invalid_argument) );
    throw my_exception{};
}
#endif

void check_diagnostic_info(nlohmann::ordered_json const & j, bool has_source_location)
{
    BOOST_TEST(j["error_id"].get<int>() > 0);

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
    BOOST_TEST(j["error_id"].get<int>() > 0);

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
        auto const & e2j = j["my_error<2>"];
        BOOST_TEST_EQ(e2j["code"].get<int>(), 2);
        BOOST_TEST_EQ(e2j["message"].get<std::string>(), "error two");

        auto const & ej = j["boost::leaf::e_errno"];
        BOOST_TEST_EQ(ej["value"].get<int>(), ENOENT);
        BOOST_TEST(!ej["message"].get<std::string>().empty());

        BOOST_TEST_EQ(j["boost::leaf::e_api_function"]["value"].get<std::string>(), "my_api_function");

        auto const & ecj = j["std::error_code"];
        BOOST_TEST_EQ(ecj["value"].get<int>(), static_cast<int>(std::errc::invalid_argument));
        BOOST_TEST(!ecj["category"].get<std::string>().empty());
        BOOST_TEST(!ecj["message"].get<std::string>().empty());
    }
    else
    {
        BOOST_TEST(!j.contains("my_error<2>"));
        BOOST_TEST(!j.contains("boost::leaf::e_errno"));
        BOOST_TEST(!j.contains("boost::leaf::e_api_function"));
        BOOST_TEST(!j.contains("std::error_code"));
    }
}

#ifndef BOOST_LEAF_NO_EXCEPTIONS
void check_exception(nlohmann::ordered_json const & j)
{
    auto const & exj = j["std::exception"];
    BOOST_TEST(!exj["typeid.name"].get<std::string>().empty());
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
                nlohmann_writer w(j, di.error());
                di.write_to(w);
            }
        );
        std::cout << "diagnostic_info JSON output:\n" << std::setw(2) << j << std::endl;
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
                nlohmann_writer w(j, dd.error());
                dd.write_to(w);
            }
        );
        std::cout << "diagnostic_details JSON output:\n" << std::setw(2) << j << std::endl;
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
                nlohmann_writer w(j, di.error());
                di.write_to(w);
            }
        );
        std::cout << "leaf_throw diagnostic_info JSON output:\n" << std::setw(2) << j << std::endl;
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
                nlohmann_writer w(j, dd.error());
                dd.write_to(w);
            }
        );
        std::cout << "leaf_throw diagnostic_details JSON output:\n" << std::setw(2) << j << std::endl;
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
                nlohmann_writer w(j, di.error());
                di.write_to(w);
            }
        );
        std::cout << "throw_ diagnostic_info JSON output:\n" << std::setw(2) << j << std::endl;
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
                nlohmann_writer w(j, dd.error());
                dd.write_to(w);
            }
        );
        std::cout << "throw_ diagnostic_details JSON output:\n" << std::setw(2) << j << std::endl;
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
                nlohmann_writer w(j, dd.error());
                dd.write_to(w);
            }
        );
        std::cout << "std::exception_ptr JSON output:\n" << std::setw(2) << j << std::endl;

        auto const & ep = j["my_exception_ptr"]["value"];
        std::string type = ep["typeid.name"].get<std::string>();
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
                nlohmann_writer w(j, dd.error());
                dd.write_to(w);
            }
        );
        std::cout << "non-std::exception_ptr JSON output:\n" << std::setw(2) << j << std::endl;

        auto const & ep = j["my_exception_ptr"]["value"];
        std::string type = ep["typeid.name"].get<std::string>();
        std::string what = ep["what"].get<std::string>();
        BOOST_TEST_EQ(type, "<<unknown>>");
        BOOST_TEST_EQ(what, "N/A");
    }
#endif

    return boost::report_errors();
}

#endif
