#ifndef BOOST_LEAF_SERIALIZE_HPP_INCLUDED
#define BOOST_LEAF_SERIALIZE_HPP_INCLUDED

// Copyright 2018-2025 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>
#include <boost/leaf/detail/demangle.hpp>

#include <type_traits>
#include <iosfwd>
#include <cstring>

#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
#   include <system_error>
#endif

#ifndef BOOST_LEAF_NO_EXCEPTIONS
#   include <exception>
#   include <typeinfo>
#endif

namespace boost { namespace leaf {

template <class E>
struct show_in_diagnostics: std::true_type
{
};

class writer
{
    virtual parsed type() const noexcept = 0;

protected:
    ~writer() noexcept { }

public:
    template <class W>
    W * check_type(parsed tid) noexcept
    {
        return type() == tid ? static_cast<W *>(this) : nullptr;
    }
};

////////////////////////////////////////

namespace detail
{
    template <class T, class E = void>
    struct is_printable: std::false_type
    {
    };

    template <class T>
    struct is_printable<T, decltype(std::declval<std::ostream&>()<<std::declval<T const &>(), void())>: show_in_diagnostics<T>
    {
    };

    template <class T, class E = void>
    struct has_printable_member_value: std::false_type
    {
    };

    template <class T>
    struct has_printable_member_value<T, decltype(std::declval<std::ostream&>()<<std::declval<T const &>().value, void())>: show_in_diagnostics<T>
    {
    };

    template <class T, class E = void>
    struct has_member_value: std::false_type
    {
    };

    template <class T>
    struct has_member_value<T, decltype((void)std::declval<T const &>().value)>: std::true_type
    {
    };
}

////////////////////////////////////////

class ostream_writer: public writer
{
    std::ostream & os_;
    char const * & prefix_;
    char const * const delimiter_;

    parsed type() const noexcept override
    {
        return parse<ostream_writer>();
    }

    template <class T, class CharT, class Traits>
    static void print_name(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter)
    {
        static_assert(show_in_diagnostics<T>::value, "show_in_diagnostics violation");
        BOOST_LEAF_ASSERT(delimiter);
        char const * p = prefix;
        prefix = nullptr;
        os << (p ? p : delimiter) << parse<T>();
    }

    template <class T, class PrintableInfo, class CharT, class Traits>
    static bool print_impl(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter, char const * mid, PrintableInfo const & x)
    {
        print_name<T>(os, prefix, delimiter);
        if( mid )
            os << mid << x;
        return true;
    }

    template <class T, class PrintableInfo, class CharT, class Traits>
    static bool print_impl(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter, char const * mid, PrintableInfo const * x)
    {
        print_name<T>(os, prefix, delimiter);
        if( mid )
        {
            os << mid;
            if( x )
                os << x;
            else
                os << "<nullptr>";
        }
        return true;
    }

    template <
        class Wrapper,
        bool ShowInDiagnostics = show_in_diagnostics<Wrapper>::value,
        bool WrapperPrintable = detail::is_printable<Wrapper>::value,
        bool ValuePrintable = detail::has_printable_member_value<Wrapper>::value,
        bool IsException = std::is_base_of<std::exception,Wrapper>::value,
        bool IsEnum = std::is_enum<Wrapper>::value>
    struct diagnostic;

public:
    ostream_writer(std::ostream & os, char const * & prefix, char const * delimiter) noexcept:
        os_(os),
        prefix_(prefix),
        delimiter_(delimiter)
    {
    }

    template <class E>
    void write(E const & e)
    {
        diagnostic<E>::print(os_, prefix_, delimiter_, e);
    }
};

template <class Writer, class E>
void serialize(Writer & w, E const & e)
{
    if( ostream_writer * ow = w.template check_type<ostream_writer>(parse<ostream_writer>()) )
        ow->write(e);
}

////////////////////////////////////////

template <class Wrapper, bool WrapperPrintable, bool ValuePrintable, bool IsException, bool IsEnum>
struct ostream_writer::diagnostic<Wrapper, false, WrapperPrintable, ValuePrintable, IsException, IsEnum>
{
    template <class CharT, class Traits>
    static bool print(std::basic_ostream<CharT, Traits> &, char const * &, char const *, Wrapper const &) noexcept
    {
        return false;
    }
};

template <class Wrapper, bool ValuePrintable, bool IsEnum>
struct ostream_writer::diagnostic<Wrapper, true, true, ValuePrintable, false, IsEnum>
{
    template <class CharT, class Traits>
    static bool print(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter, Wrapper const & x)
    {
        return print_impl<Wrapper>(os, prefix, delimiter, ": ", x);
    }
};

template <class Wrapper>
struct ostream_writer::diagnostic<Wrapper, true, false, true, false, false>
{
    template <class CharT, class Traits>
    static bool print(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter, Wrapper const & x)
    {
        return print_impl<Wrapper>(os, prefix, delimiter, ": ", x.value);
    }
};

template <class Exception, bool WrapperPrintable, bool ValuePrintable>
struct ostream_writer::diagnostic<Exception, true, WrapperPrintable, ValuePrintable, true, false>
{
    template <class CharT, class Traits>
    static bool print(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter, Exception const & ex)
    {
        if( print_impl<Exception>(os, prefix, delimiter, ": \"", static_cast<std::exception const &>(ex).what()) )
        {
            os << '"';
            return true;
        }
        return false;
    }
};

template <class Wrapper>
struct ostream_writer::diagnostic<Wrapper, true, false, false, false, false>
{
    template <class CharT, class Traits>
    static bool print(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter, Wrapper const &)
    {
        return print_impl<Wrapper>(os, prefix, delimiter, nullptr, 0);
    }
};

template <class Enum>
struct ostream_writer::diagnostic<Enum, true, false, false, false, true>
{
    template <class CharT, class Traits>
    static bool print(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter, Enum const & enum_)
    {
        return print_impl<Enum>(os, prefix, delimiter, ": ", static_cast<typename std::underlying_type<Enum>::type>(enum_));
    }
};

////////////////////////////////////////

template <class Json, class E>
auto to_json(Json & j, E const & e) -> decltype(to_json(j, e.value), void())
{
    char zstr[256];
    j[parse_to_zstr<E>(zstr)] = e.value;
}

#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
template <class Json>
void to_json(Json & j, std::error_code const & ec)
{
    Json & v = j["std::error_code"];
    v["category"] = ec.category().name();
    v["value"] = ec.value();
    v["message"] = ec.message();
}
#endif

template <class Json>
void to_json(Json & j, std::exception_ptr const & ep)
{
    Json & v = j["std::exception_ptr"];
    if( ep )
    {
#ifdef BOOST_LEAF_NO_EXCEPTIONS
        v["typeid.name"] = "<<unknown>>";
        v["what"] = "N/A";
#else
        try
        {
            std::rethrow_exception(ep);
        }
        catch( std::exception const & ex )
        {
            v["typeid.name"] = detail::demangler(typeid(ex).name()).get();
            if( char const * w = ex.what() )
                v["what"] = w;
            else
                v["what"] = "<<nullptr>>";
        }
        catch( ... )
        {
            v["typeid.name"] = "<<unknown>>";
            v["what"] = "N/A";
        }
#endif
    }
    else
    {
        v["typeid.name"] = "<<empty>>";
        v["what"] = "N/A";
    }
}

template <class Json, class E, class = void>
struct has_to_json: std::false_type { };

template <class Json, class E>
struct has_to_json<Json, E, decltype(to_json(std::declval<Json &>(), std::declval<E const &>()), void())>: std::true_type { };

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_SERIALIZE_HPP_INCLUDED
