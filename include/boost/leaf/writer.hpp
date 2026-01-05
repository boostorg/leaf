#ifndef BOOST_LEAF_WRITER_HPP_INCLUDED
#define BOOST_LEAF_WRITER_HPP_INCLUDED

// Copyright 2018-2025 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>
#include <boost/leaf/detail/demangle.hpp>

#include <type_traits>

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
    parsed const type_;

protected:

    template <class Derived>
    explicit writer(Derived * d) noexcept:
        type_(parse<Derived>())
    {
        BOOST_LEAF_ASSERT(d == this), (void) d;
    }

    ~writer() noexcept
    {
    }

public:

    template <class Derived>
    Derived * get() noexcept
    {
        return type_ == parse<typename std::decay<Derived>::type>() ? static_cast<Derived *>(this) : nullptr;
    }
};

////////////////////////////////////////

#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
template <class Json>
void to_json(Json & j, std::error_code const & ec)
{
    j["category"] = ec.category().name();
    j["value"] = ec.value();
    j["message"] = ec.message();
}
#endif

#ifndef BOOST_LEAF_NO_EXCEPTIONS
template <class Json>
void to_json(Json & j, std::exception const & ex)
{
    j["typeid.name"] = detail::demangler(typeid(ex).name()).get();
    if( char const * w = ex.what() )
        j["what"] = w;
    else
        j["what"] = "<<nullptr>>";
}
#endif

template <class Json>
void to_json(Json & j, std::exception_ptr const & ep)
{
    if( ep )
    {
#ifdef BOOST_LEAF_NO_EXCEPTIONS
        j["typeid.name"] = "<<unknown>>";
        j["what"] = "N/A";
#else
        try
        {
            std::rethrow_exception(ep);
        }
        catch( std::exception const & ex )
        {
            j["typeid.name"] = detail::demangler(typeid(ex).name()).get();
            if( char const * w = ex.what() )
                j["what"] = w;
            else
                j["what"] = "<<nullptr>>";
        }
        catch( ... )
        {
            j["typeid.name"] = "<<unknown>>";
            j["what"] = "N/A";
        }
#endif
    }
    else
    {
        j["typeid.name"] = "<<empty>>";
        j["what"] = "N/A";
    }
}

template <class Json, class E>
auto to_json(Json & j, E const & e) -> decltype(to_json(j, e.value), void())
{
    to_json(j["value"], e.value);
}

template <class Json, class E, class = void>
struct has_to_json: std::false_type { };

template <class Json, class E>
struct has_to_json<Json, E, decltype(to_json(std::declval<Json &>(), std::declval<E const &>()), void())>: std::true_type { };

namespace detail
{
    template <class>
    struct dependent_writer
    {
        using type = writer;
    };
}

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_WRITER_HPP_INCLUDED
