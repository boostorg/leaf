#ifndef BOOST_LEAF_DIAGNOSTICS_HPP_INCLUDED
#define BOOST_LEAF_DIAGNOSTICS_HPP_INCLUDED

// Copyright 2018-2024 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>
#include <boost/leaf/context.hpp>
#include <boost/leaf/handle_errors.hpp>

namespace boost { namespace leaf {

#if BOOST_LEAF_CFG_DIAGNOSTICS

class diagnostic_info: public error_info
{
    void const * tup_;
    void (*print_context_content_)( std::ostream &, void const * tup, int err_id_to_print );

protected:

    diagnostic_info( diagnostic_info const & ) noexcept = default;

    template <class Tup>
    BOOST_LEAF_CONSTEXPR diagnostic_info( error_info const & ei, Tup const & tup ) noexcept:
        error_info(ei),
        tup_(&tup),
        print_context_content_(&leaf_detail::tuple_for_each<std::tuple_size<Tup>::value, Tup>::print)
    {
    }

    template <class CharT, class Traits>
    friend std::ostream & operator<<( std::basic_ostream<CharT, Traits> & os, diagnostic_info const & x )
    {
        os << static_cast<error_info const &>(x);
        x.print_context_content_(os, x.tup_, x.error().value());
        return os;
    }
};

namespace leaf_detail
{
    struct diagnostic_info_: diagnostic_info
    {
        template <class Tup>
        BOOST_LEAF_CONSTEXPR diagnostic_info_( error_info const & ei, Tup const & tup ) noexcept:
            diagnostic_info(ei, tup)
        {
        }
    };

    template <>
    struct handler_argument_traits<diagnostic_info const &>: handler_argument_always_available<void>
    {
        template <class Tup>
        BOOST_LEAF_CONSTEXPR static diagnostic_info_ get( Tup const & tup, error_info const & ei ) noexcept
        {
            return diagnostic_info_(ei, tup);
        }
    };
}

#else

class diagnostic_info: public error_info
{
protected:

    diagnostic_info( diagnostic_info const & ) noexcept = default;

    BOOST_LEAF_CONSTEXPR diagnostic_info( error_info const & ei ) noexcept:
        error_info(ei)
    {
    }

    template <class CharT, class Traits>
    friend std::ostream & operator<<( std::basic_ostream<CharT, Traits> & os, diagnostic_info const & x )
    {
        return os << "diagnostic_info not available due to BOOST_LEAF_CFG_DIAGNOSTICS=0. Basic error_info follows.\n" << static_cast<error_info const &>(x);
    }
};

namespace leaf_detail
{
    struct diagnostic_info_: diagnostic_info
    {
        BOOST_LEAF_CONSTEXPR diagnostic_info_( error_info const & ei ) noexcept:
            diagnostic_info(ei)
        {
        }
    };

    template <>
    struct handler_argument_traits<diagnostic_info const &>: handler_argument_always_available<void>
    {
        template <class Tup>
        BOOST_LEAF_CONSTEXPR static diagnostic_info_ get( Tup const &, error_info const & ei ) noexcept
        {
            return diagnostic_info_(ei);
        }
    };
}

#endif

////////////////////////////////////////

#if BOOST_LEAF_CFG_DIAGNOSTICS

#if BOOST_LEAF_CFG_CAPTURE

class diagnostic_details: public diagnostic_info
{
    leaf_detail::dynamic_allocator const * const da_;

protected:

    diagnostic_details( diagnostic_details const & ) noexcept = default;

    template <class Tup>
    BOOST_LEAF_CONSTEXPR diagnostic_details( error_info const & ei, Tup const & tup, leaf_detail::dynamic_allocator const * da ) noexcept:
        diagnostic_info(ei, tup),
        da_(da)
    {
    }

    template <class CharT, class Traits>
    friend std::ostream & operator<<( std::basic_ostream<CharT, Traits> & os, diagnostic_details const & x )
    {
        os << static_cast<diagnostic_info const &>(x);
        if( x.da_ )
            x.da_->print(os, x.error().value());
        return os;
    }
};

namespace leaf_detail
{
    template <class T>
    struct get_dispatch
    {
        static BOOST_LEAF_CONSTEXPR T const * get(T const * x) noexcept
        {
            return x;
        }
        static BOOST_LEAF_CONSTEXPR T const * get(void const *) noexcept
        {
            return nullptr;
        }
    };

    template <class T, int I = 0, class... Tp>
    BOOST_LEAF_CONSTEXPR inline typename std::enable_if<I == sizeof...(Tp) - 1, T>::type const *
    find_in_tuple(std::tuple<Tp...> const & t) noexcept
    {
        return get_dispatch<T>::get(&std::get<I>(t));
    }

    template<class T, int I = 0, class... Tp>
    BOOST_LEAF_CONSTEXPR inline typename std::enable_if<I < sizeof...(Tp) - 1, T>::type const *
    find_in_tuple(std::tuple<Tp...> const & t) noexcept
    {
        if( T const * x = get_dispatch<T>::get(&std::get<I>(t)) )
            return x;
        else
            return find_in_tuple<T, I+1, Tp...>(t);
    }

    struct diagnostic_details_: diagnostic_details
    {
        template <class Tup>
        BOOST_LEAF_CONSTEXPR diagnostic_details_( error_info const & ei, Tup const & tup, dynamic_allocator const * da ) noexcept:
            diagnostic_details(ei, tup, da)
        {
        }
    };

    template <>
    struct handler_argument_traits<diagnostic_details const &>: handler_argument_always_available<dynamic_allocator>
    {
        template <class Tup>
        BOOST_LEAF_CONSTEXPR static diagnostic_details_ get( Tup const & tup, error_info const & ei ) noexcept
        {
            slot<dynamic_allocator> const * da = find_in_tuple<slot<dynamic_allocator>>(tup);
            return diagnostic_details_(ei, tup, da ? da->has_value() : nullptr );
        }
    };
}

#else

class diagnostic_details: public diagnostic_info
{
protected:

    diagnostic_details( diagnostic_details const & ) noexcept = default;

    template <class Tup>
    BOOST_LEAF_CONSTEXPR diagnostic_details( error_info const & ei, Tup const & tup ) noexcept:
        diagnostic_info(ei, tup)
    {
    }

    template <class CharT, class Traits>
    friend std::ostream & operator<<( std::basic_ostream<CharT, Traits> & os, diagnostic_details const & x )
    {
        return os << "diagnostic_details not available due to BOOST_LEAF_CFG_CAPTURE=0. Basic diagnostic_info follows.\n" << static_cast<diagnostic_info const &>(x);
    }
};

namespace leaf_detail
{
    struct diagnostic_details_: diagnostic_details
    {
        template <class Tup>
        BOOST_LEAF_CONSTEXPR diagnostic_details_( error_info const & ei, Tup const & tup ) noexcept:
            diagnostic_details(ei, tup)
        {
        }
    };

    template <>
    struct handler_argument_traits<diagnostic_details const &>: handler_argument_always_available<void>
    {
        template <class Tup>
        BOOST_LEAF_CONSTEXPR static diagnostic_details_ get( Tup const & tup, error_info const & ei ) noexcept
        {
            return diagnostic_details_(ei, tup);
        }
    };
}

#endif

#else

class diagnostic_details: public diagnostic_info
{
protected:

    diagnostic_details( diagnostic_details const & ) noexcept = default;

    BOOST_LEAF_CONSTEXPR diagnostic_details( error_info const & ei ) noexcept:
        diagnostic_info(ei)
    {
    }

    template <class CharT, class Traits>
    friend std::ostream & operator<<( std::basic_ostream<CharT, Traits> & os, diagnostic_details const & x )
    {
        return os << "diagnostic_details not available due to BOOST_LEAF_CFG_DIAGNOSTICS=0. Basic error_info follows.\n" << static_cast<error_info const &>(x);
    }
};

namespace leaf_detail
{
    struct diagnostic_details_: diagnostic_details
    {
        BOOST_LEAF_CONSTEXPR diagnostic_details_( error_info const & ei ) noexcept:
            diagnostic_details(ei)
        {
        }
    };

    template <>
    struct handler_argument_traits<diagnostic_details const &>: handler_argument_always_available<void>
    {
        template <class Tup>
        BOOST_LEAF_CONSTEXPR static diagnostic_details_ get( Tup const &, error_info const & ei ) noexcept
        {
            return diagnostic_details_(ei);
        }
    };
}

#endif

using verbose_diagnostic_info = diagnostic_details;

} }

#endif
