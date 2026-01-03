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

#include <boost/leaf/handle_errors.hpp>
#include <boost/leaf/result.hpp>
#include <boost/leaf/diagnostics.hpp>
#include <boost/leaf/common.hpp>
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

    leaf::parsed type() const noexcept override
    {
        return leaf::parse<nlohmann_writer>();
    }

public:
    explicit nlohmann_writer(nlohmann::ordered_json & j) noexcept:
        json_(j)
    {
    }

    template <class E>
    void write(E const & e)
    {
        using namespace leaf;
        to_json(json_, e);
    }
};

namespace boost { namespace leaf {

template <class E>
void serialize(writer & w, E const & e)
{
    if( nlohmann_writer * nw = w.check_type<nlohmann_writer>(parse<nlohmann_writer>()) )
        nw->write(e);
    else if( ostream_writer * ow = w.check_type<ostream_writer>(parse<ostream_writer>()) )
        ow->write(e);
}

} } // namespace boost::leaf

template <int N>
struct my_error
{
    int code;
    char const * message;

    template <class Json>
    friend void to_json(Json & j, my_error const & e)
    {
        leaf::parsed p = leaf::parse<my_error>();
        Json & v = j[std::string(p.name, p.len)];
        v["code"] = e.code;
        v["message"] = e.message;
    }

    friend std::ostream & operator<<(std::ostream & os, my_error const & e)
    {
        return os << "code=" << e.code << ", message=" << e.message;
    }
};

leaf::result<void> fail()
{
    return leaf::new_error(
        my_error<1>{1, "error one"},
        my_error<2>{2, "error two"},
        leaf::e_errno{ENOENT},
        leaf::e_api_function{"fail"},
        std::make_error_code(std::errc::invalid_argument)
    );
}

int main()
{
    {
        leaf::try_handle_all(
            []
            {
                return fail();
            },
            [](leaf::diagnostic_info const & di, my_error<1> const * e1)
            {
                BOOST_TEST(e1 != nullptr);

                nlohmann::ordered_json j;
                nlohmann_writer w(j);
                di.write_to(w);

                auto const & jc = j;
                std::cout << "diagnostic_info JSON output:\n" << std::setw(2) << jc << std::endl;
                BOOST_TEST(jc["serial number"].get<int>() > 0);

                auto const & e1j = jc["my_error<1>"];
                BOOST_TEST_EQ(e1j["code"].get<int>(), 1);
                BOOST_TEST_EQ(e1j["message"].get<std::string>(), "error one");

                BOOST_TEST(!jc.contains("my_error<2>"));
            }
        );
    }

    {
        leaf::try_handle_all(
            []
            {
                return fail();
            },
            [](leaf::diagnostic_details const & dd, my_error<1> const * e1)
            {
                BOOST_TEST(e1 != nullptr);

                nlohmann::ordered_json j;
                nlohmann_writer w(j);
                dd.write_to(w);

                auto const & jc = j;
                std::cout << "diagnostic_details JSON output:\n" << std::setw(2) << jc << std::endl;
                BOOST_TEST(jc["serial number"].get<int>() > 0);

                auto const & e1j = jc["my_error<1>"];
                BOOST_TEST_EQ(e1j["code"].get<int>(), 1);
                BOOST_TEST_EQ(e1j["message"].get<std::string>(), "error one");

                auto const & e2j = jc["my_error<2>"];
                BOOST_TEST_EQ(e2j["code"].get<int>(), 2);
                BOOST_TEST_EQ(e2j["message"].get<std::string>(), "error two");

                auto const & ej = jc["boost::leaf::e_errno"];
                BOOST_TEST_EQ(ej["value"].get<int>(), ENOENT);
                BOOST_TEST(!ej["message"].get<std::string>().empty());

                BOOST_TEST_EQ(jc["boost::leaf::e_api_function"].get<std::string>(), "fail");

                auto const & ecj = jc["std::error_code"];
                BOOST_TEST_EQ(ecj["value"].get<int>(), static_cast<int>(std::errc::invalid_argument));
                BOOST_TEST(!ecj["category"].get<std::string>().empty());
                BOOST_TEST(!ecj["message"].get<std::string>().empty());
            }
        );
    }

    {
        leaf::try_handle_all(
            []() -> leaf::result<void>
            {
                return leaf::new_error(std::make_exception_ptr(std::runtime_error("test exception")));
            },
            [](leaf::diagnostic_details const & dd)
            {
                nlohmann::ordered_json j;
                nlohmann_writer w(j);
                dd.write_to(w);

                auto const & jc = j;
                std::cout << "std::exception JSON output:\n" << std::setw(2) << jc << std::endl;

                auto const & ep = jc["std::exception_ptr"];
                std::string type = ep["typeid.name"].get<std::string>();
                std::string what = ep["what"].get<std::string>();
#ifdef BOOST_LEAF_NO_EXCEPTIONS
                BOOST_TEST(type == "<<unknown>>" || type == "<<empty>>");
                BOOST_TEST_EQ(what, "N/A");
#else
                BOOST_TEST(type.find("std::runtime_error") != std::string::npos);
                BOOST_TEST_EQ(what, "test exception");
#endif
            }
        );
    }

    {
        leaf::try_handle_all(
            []() -> leaf::result<void>
            {
                return leaf::new_error(std::make_exception_ptr(42));
            },
            [](leaf::diagnostic_details const & dd)
            {
                nlohmann::ordered_json j;
                nlohmann_writer w(j);
                dd.write_to(w);

                auto const & jc = j;
                std::cout << "non-std::exception JSON output:\n" << std::setw(2) << jc << std::endl;

                auto const & ep = jc["std::exception_ptr"];
                std::string type = ep["typeid.name"].get<std::string>();
                std::string what = ep["what"].get<std::string>();
#ifdef BOOST_LEAF_NO_EXCEPTIONS
                BOOST_TEST(type == "<<unknown>>" || type == "<<empty>>");
#else
                BOOST_TEST_EQ(type, "<<unknown>>");
#endif
                BOOST_TEST_EQ(what, "N/A");
            }
        );
    }

    return boost::report_errors();
}

#endif
