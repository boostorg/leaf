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
#   include <boost/leaf/serialization/writer.hpp>
#endif

#include <boost/json.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
#   include <system_error>
#endif

#include "lightweight_test.hpp"

#if BOOST_LEAF_CFG_STD_STRING

namespace leaf = boost::leaf;

void pretty_print(std::ostream & os, boost::json::value const & jv, std::string indent = "")
{
    std::string const indent1 = indent + "  ";
    switch( jv.kind() )
    {
    case boost::json::kind::object:
        {
            os << "{\n";
            auto const & obj = jv.get_object();
            for( auto it = obj.begin(); it != obj.end(); ++it )
            {
                os << indent1 << boost::json::serialize(it->key()) << ": ";
                pretty_print(os, it->value(), indent1);
                if( std::next(it) != obj.end() )
                    os << ",";
                os << "\n";
            }
            os << indent << "}";
            break;
        }
    case boost::json::kind::array:
        {
            os << "[\n";
            auto const & arr = jv.get_array();
            for( auto it = arr.begin(); it != arr.end(); ++it )
            {
                os << indent1;
                pretty_print(os, *it, indent1);
                if( std::next(it) != arr.end() )
                    os << ",";
                os << "\n";
            }
            os << indent << "]";
            break;
        }
    case boost::json::kind::string:
        os << boost::json::serialize(jv.get_string());
        break;
    case boost::json::kind::int64:
        os << jv.get_int64();
        break;
    case boost::json::kind::uint64:
        os << jv.get_uint64();
        break;
    case boost::json::kind::double_:
        os << jv.get_double();
        break;
    case boost::json::kind::bool_:
        os << (jv.get_bool() ? "true" : "false");
        break;
    case boost::json::kind::null:
        os << "null";
        break;
    }
}

struct pretty
{
    boost::json::value const & jv;
    friend std::ostream & operator<<(std::ostream & os, pretty const & p)
    {
        pretty_print(os, p.jv);
        return os;
    }
};

namespace
{
    struct write_detector: leaf::serialization::writer
    {
        write_detector(): writer(this) { }
    };

    template <class E, class = void>
    struct has_leaf_write: std::false_type { };

    template <class E>
    struct has_leaf_write<E, decltype(write(std::declval<write_detector&>(), std::declval<E const&>()), void())>: std::true_type { };
}

class boost_json_writer: public leaf::serialization::writer
{
    boost::json::value & j_;

    struct value_from_fallback { };
    struct prefer_write: value_from_fallback { };

    template <class E>
    static auto dispatch(boost::json::value & j, E const & e, prefer_write)
        -> decltype(write(std::declval<boost_json_writer &>(), e), void())
    {
        boost_json_writer w(j);
        write(w, e);
    }

    template <class E>
    static void dispatch(boost::json::value & j, E const & e, value_from_fallback)
    {
        j = boost::json::value_from(e);
    }

public:

    explicit boost_json_writer(boost::json::value & j) noexcept:
        writer(this),
        j_(j)
    {
        if( j_.is_null() )
            j_ = boost::json::object();
    }

    template <class E>
    friend void write_nested(boost_json_writer & w, E const & e, char const * name)
    {
        dispatch(w.j_.as_object()[name], e, prefer_write{});
    }

    template <class E>
    friend auto write(boost_json_writer & w, E const & e)
        -> std::enable_if_t<!has_leaf_write<E>::value, void>
    {
        w.j_ = boost::json::value_from(e);
    }
};

namespace boost { namespace leaf {

namespace serialization {

template <class E>
void serialize(writer & w, E const & e, char const * name)
{
    if( boost_json_writer * bw = w.get<boost_json_writer>() )
        write_nested(*bw, e, name);
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
        jv = { {"code", e.code}, {"message", e.message} };
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
    BOOST_TEST(j.at("boost::leaf::error_id").as_int64() > 0);

    auto const & e1j = j.at("my_error<1>");
    BOOST_TEST_EQ(e1j.at("code").as_int64(), 1);
    BOOST_TEST_EQ(e1j.at("message").as_string(), "error one");

    if( has_source_location )
    {
        auto const & loc = j.at("boost::leaf::e_source_location");
        BOOST_TEST(!loc.at("file").as_string().empty());
        BOOST_TEST(loc.at("line").as_int64() > 0);
        BOOST_TEST(!loc.at("function").as_string().empty());
    }

    BOOST_TEST(j.as_object().find("my_error<2>") == j.as_object().end());
}

void check_diagnostic_details(boost::json::value const & j, bool has_source_location)
{
    BOOST_TEST(j.at("boost::leaf::error_id").as_int64() > 0);

    auto const & e1j = j.at("my_error<1>");
    BOOST_TEST_EQ(e1j.at("code").as_int64(), 1);
    BOOST_TEST_EQ(e1j.at("message").as_string(), "error one");

    if( has_source_location )
    {
        auto const & loc = j.at("boost::leaf::e_source_location");
        BOOST_TEST(!loc.at("file").as_string().empty());
        BOOST_TEST(loc.at("line").as_int64() > 0);
        BOOST_TEST(!loc.at("function").as_string().empty());
    }

    if( BOOST_LEAF_CFG_CAPTURE )
    {
        BOOST_TEST_EQ(j.at("int").as_int64(), 42);

        auto const & e2j = j.at("my_error<2>");
        BOOST_TEST_EQ(e2j.at("code").as_int64(), 2);
        BOOST_TEST_EQ(e2j.at("message").as_string(), "error two");

        auto const & ej = j.at("boost::leaf::e_errno");
        BOOST_TEST_EQ(ej.at("errno").as_int64(), ENOENT);
        BOOST_TEST(!ej.at("strerror").as_string().empty());

        BOOST_TEST_EQ(j.at("boost::leaf::e_api_function").as_string(), "my_api_function");

        if( BOOST_LEAF_CFG_STD_SYSTEM_ERROR )
        {
            auto const & ecj = j.at("std::error_code");
            BOOST_TEST_EQ(ecj.at("value").as_int64(), static_cast<int>(std::errc::invalid_argument));
            BOOST_TEST(!ecj.at("category").as_string().empty());
            BOOST_TEST(!ecj.at("message").as_string().empty());

            auto const & econdj = j.at("std::error_condition");
            BOOST_TEST_EQ(econdj.at("value").as_int64(), static_cast<int>(std::errc::io_error));
            BOOST_TEST(!econdj.at("category").as_string().empty());
            BOOST_TEST(!econdj.at("message").as_string().empty());

            auto const & mecj = j.at("my_error_code");
            BOOST_TEST_EQ(mecj.at("value").as_int64(), static_cast<int>(std::errc::invalid_argument));
            BOOST_TEST(!mecj.at("category").as_string().empty());
            BOOST_TEST(!mecj.at("message").as_string().empty());
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
    BOOST_TEST(!exj.at("dynamic_type").as_string().empty());
    BOOST_TEST(!exj.at("what").as_string().empty());
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
                boost_json_writer w(j);
                di.write_to(w);
            }
        );
        std::cout << __LINE__ << " diagnostic_info JSON output:\n" << pretty{j} << std::endl;
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
                boost_json_writer w(j);
                dd.write_to(w);
            }
        );
        std::cout << __LINE__ << " diagnostic_details JSON output:\n" << pretty{j} << std::endl;
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
                boost_json_writer w(j);
                di.write_to(w);
            }
        );
        std::cout << __LINE__ << " leaf_throw diagnostic_info JSON output:\n" << pretty{j} << std::endl;
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
                boost_json_writer w(j);
                dd.write_to(w);
            }
        );
        std::cout << __LINE__ << " leaf_throw diagnostic_details JSON output:\n" << pretty{j} << std::endl;
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
                boost_json_writer w(j);
                di.write_to(w);
            }
        );
        std::cout << __LINE__ << " throw_ diagnostic_info JSON output:\n" << pretty{j} << std::endl;
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
                boost_json_writer w(j);
                dd.write_to(w);
            }
        );
        std::cout << __LINE__ << " throw_ diagnostic_details JSON output:\n" << pretty{j} << std::endl;
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
                boost_json_writer w(j);
                dd.write_to(w);
            }
        );
        std::cout << __LINE__ << " std::exception_ptr JSON output:\n" << pretty{j} << std::endl;

        auto const & ep = j.at("my_exception_ptr");
        auto const & type = ep.at("dynamic_type").as_string();
        auto const & what = ep.at("what").as_string();
        BOOST_TEST(type.find("std::runtime_error") != boost::json::string::npos);
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
                boost_json_writer w(j);
                dd.write_to(w);
            }
        );
        std::cout << __LINE__ << " non-std::exception_ptr JSON output:\n" << pretty{j} << std::endl;

        auto const & ep = j.at("my_exception_ptr");
        auto const & type = ep.at("dynamic_type").as_string();
        auto const & what = ep.at("what").as_string();
        BOOST_TEST_EQ(type, "<<unknown>>");
        BOOST_TEST_EQ(what, "N/A");
    }
#endif

    {
        boost::json::value j;
        leaf::result<int> r = 42;
        BOOST_TEST(r);
        boost_json_writer w(j);
        r.write_to(w);
        std::cout << __LINE__ << " result<int> success JSON output:\n" << pretty{j} << std::endl;
        BOOST_TEST_EQ(j.at("int").as_int64(), 42);
    }

    {
        boost::json::value j;
        leaf::result<int> r = leaf::new_error();
        BOOST_TEST(!r);
        boost_json_writer w(j);
        r.write_to(w);
        std::cout << __LINE__ << " result<int> error JSON output:\n" << pretty{j} << std::endl;
        BOOST_TEST(j.at("boost::leaf::error_id").as_int64() > 0);
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
        boost_json_writer w(j);
        r.write_to(w);
        std::cout << __LINE__ << " result<int> captured error JSON output:\n" << pretty{j} << std::endl;
        BOOST_TEST(j.at("boost::leaf::error_id").as_int64() > 0);
        auto const & e1j = j.at("my_error<1>");
        BOOST_TEST_EQ(e1j.at("code").as_int64(), 1);
        BOOST_TEST_EQ(e1j.at("message").as_string(), "error one");
        auto const & e2j = j.at("my_error<2>");
        BOOST_TEST_EQ(e2j.at("code").as_int64(), 2);
        BOOST_TEST_EQ(e2j.at("message").as_string(), "error two");
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
