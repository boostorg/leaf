#ifndef BOOST_LEAF_SERIALIZATION_JSON_WRITER_HPP_INCLUDED
#define BOOST_LEAF_SERIALIZATION_JSON_WRITER_HPP_INCLUDED

// Copyright 2018-2026 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>
#include <boost/leaf/serialization/writer.hpp>
#include <boost/leaf/detail/exception_base.hpp>
#include <boost/leaf/error.hpp>
#include <boost/leaf/common.hpp>

#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
#   include <system_error>
#endif

#ifndef BOOST_LEAF_NO_EXCEPTIONS
#   include <typeinfo>
#endif

namespace boost { namespace leaf {

namespace serialization
{
    template <class Json>
    void to_json(Json & j, error_id x)
    {
        to_json(j, x.value() / 4);
    }

    template <class Json>
    void to_json(Json & j, e_source_location const & x)
    {
        to_json(j["file"], x.file);
        to_json(j["line"], x.line);
        to_json(j["function"], x.function);
    }

    template <class Json>
    void to_json(Json & j, e_errno const & e)
    {
        to_json(j["value"], e.value);
        to_json(j["message"], std::strerror(e.value));
    }

#if BOOST_LEAF_CFG_STD_SYSTEM_ERROR
    template <class Json>
    void to_json(Json & j, std::error_code const & ec)
    {
        to_json(j["category"], ec.category().name());
        to_json(j["value"], ec.value());
        to_json(j["message"], ec.message());
    }

    template <class Json>
    void to_json(Json & j, std::error_condition const & ec)
    {
        to_json(j["category"], ec.category().name());
        to_json(j["value"], ec.value());
        to_json(j["message"], ec.message());
    }
#endif

    template <class Json>
    void to_json(Json & j, detail::exception_base const & eb)
    {
        char zstr[1024];
        to_json(j["type"], to_zstr(zstr, eb.get_type_name()));
        char const * what = "N/A";
#ifndef BOOST_LEAF_NO_EXCEPTIONS
        if( std::exception const * ex = dynamic_cast<std::exception const *>(&eb) )
            what = ex->what();
#endif
        to_json(j["what"], what ? what : "<<nullptr>>");
    }

#ifndef BOOST_LEAF_NO_EXCEPTIONS
    template <class Json>
    void to_json(Json & j, std::exception const & ex)
    {
        if( detail::exception_base const * eb = dynamic_cast<detail::exception_base const *>(&ex) )
        {
            char zstr[1024];
            to_json(j["type"], to_zstr(zstr, eb->get_type_name()));
        }
        else
            to_json(j["type"], detail::demangler(typeid(ex).name()).get());
        if( char const * w = ex.what() )
            to_json(j["what"], w);
        else
            to_json(j["what"], "<<nullptr>>");
    }
#endif

    template <class Json>
    void to_json(Json & j, std::exception_ptr const & ep)
    {
        if( ep )
        {
#ifndef BOOST_LEAF_NO_EXCEPTIONS
            try
            {
                std::rethrow_exception(ep);
            }
            catch( detail::exception_base const & eb )
            {
                to_json(j, eb);
                return;
            }
            catch( std::exception const & ex )
            {
                to_json(j, ex);
                return;
            }
            catch( ... )
            {
            }
#endif
            to_json(j["type"], "<<unknown>>");
        }
        else
            to_json(j["type"], "<<empty>>");
        to_json(j["what"], "N/A");
    }

    template <class Json, class E>
    auto to_json(Json & j, E const & e) -> decltype(to_json(j, e.value), void())
    {
        to_json(j["value"], e.value);
    }

    ////////////////////////////////////////

    template <class Json>
    class json_writer: public writer
    {
        Json & j_;

    public:

        explicit json_writer(Json & j) noexcept:
            writer(this),
            j_(j)
        {
        }

        template <class E>
        void write(E const & e)
        {
            char zstr[1024];
            to_json(j_[to_zstr(zstr, get_type_name<E>())], e);
        }
    };

} // namespace serialization

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_SERIALIZATION_JSON_WRITER_HPP_INCLUDED
