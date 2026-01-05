#ifndef BOOST_LEAF_DETAIL_DIAGNOSTICS_WRITER_HPP_INCLUDED
#define BOOST_LEAF_DETAIL_DIAGNOSTICS_WRITER_HPP_INCLUDED

// Copyright 2018-2025 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/writer.hpp>
#include <boost/leaf/detail/exception_base.hpp>

#include <iosfwd>

namespace boost { namespace leaf {

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

    ////////////////////////////////////////

    class diagnostics_writer: public writer
    {
        diagnostics_writer(diagnostics_writer const &) = delete;
        diagnostics_writer & operator=(diagnostics_writer const &) = delete;

        void (*print_lf_)(std::ostream &);
        std::ostream & os_;
        char const * prefix_;
        char const * const delimiter_;

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
            bool WrapperPrintable = is_printable<Wrapper>::value,
            bool ValuePrintable = has_printable_member_value<Wrapper>::value,
            bool IsException = std::is_base_of<std::exception,Wrapper>::value,
            bool IsEnum = std::is_enum<Wrapper>::value>
        struct diagnostic;

    public:

        template <class CharT, class Traits>
        explicit diagnostics_writer(std::basic_ostream<CharT, Traits> & os, char const * delimiter = BOOST_LEAF_CFG_DIAGNOSTICS_DELIMITER) noexcept:
            writer(this),
            print_lf_([](std::basic_ostream<CharT, Traits> & os) { os << '\n'; }),
            os_(os),
            prefix_(nullptr),
            delimiter_(delimiter)
        {
        }

        template <class CharT, class Traits, class ErrorId, class SourceLocation, class Exception>
        diagnostics_writer(std::basic_ostream<CharT, Traits> & os, ErrorId id, SourceLocation const * loc, Exception const * ex) noexcept:
            writer(this),
            print_lf_([](std::basic_ostream<CharT, Traits> & os) { os << '\n'; }),
            os_(os),
            prefix_(nullptr),
            delimiter_(BOOST_LEAF_CFG_DIAGNOSTICS_DELIMITER)
        {
            os << "Error with serial #" << id;
            if( loc )
                os << " reported at " << *loc;
#ifndef BOOST_LEAF_NO_EXCEPTIONS
            if( ex )
            {
                os << "\nCaught:" BOOST_LEAF_CFG_DIAGNOSTICS_FIRST_DELIMITER;
                if( auto eb = dynamic_cast<exception_base const *>(ex) )
                    os << eb->type_name();
                else
                    os << demangler(typeid(*ex).name()).get();
                os << ": \"" << ex->what() << '"';
            }
            else
#endif
            {
                prefix_ = "\nCaught:" BOOST_LEAF_CFG_DIAGNOSTICS_FIRST_DELIMITER;
            }
            (void) ex;
        }

        ~diagnostics_writer() noexcept
        {
            print_lf_(os_);
        }

        void set_prefix(char const * prefix) noexcept
        {
            prefix_ = prefix;
        }

        template <class E>
        void write(E const & e)
        {
            diagnostic<E>::print(os_, prefix_, delimiter_, e);
        }
    }; // class diagnostics_writer

    ////////////////////////////////////////

    template <class Wrapper, bool WrapperPrintable, bool ValuePrintable, bool IsException, bool IsEnum>
    struct diagnostics_writer::diagnostic<Wrapper, false, WrapperPrintable, ValuePrintable, IsException, IsEnum>
    {
        template <class CharT, class Traits>
        static bool print(std::basic_ostream<CharT, Traits> &, char const * &, char const *, Wrapper const &) noexcept
        {
            return false;
        }
    };

    template <class Wrapper, bool ValuePrintable, bool IsEnum>
    struct diagnostics_writer::diagnostic<Wrapper, true, true, ValuePrintable, false, IsEnum>
    {
        template <class CharT, class Traits>
        static bool print(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter, Wrapper const & x)
        {
            return print_impl<Wrapper>(os, prefix, delimiter, ": ", x);
        }
    };

    template <class Wrapper>
    struct diagnostics_writer::diagnostic<Wrapper, true, false, true, false, false>
    {
        template <class CharT, class Traits>
        static bool print(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter, Wrapper const & x)
        {
            return print_impl<Wrapper>(os, prefix, delimiter, ": ", x.value);
        }
    };

    template <class Exception, bool WrapperPrintable, bool ValuePrintable>
    struct diagnostics_writer::diagnostic<Exception, true, WrapperPrintable, ValuePrintable, true, false>
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
    struct diagnostics_writer::diagnostic<Wrapper, true, false, false, false, false>
    {
        template <class CharT, class Traits>
        static bool print(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter, Wrapper const &)
        {
            return print_impl<Wrapper>(os, prefix, delimiter, nullptr, 0);
        }
    };

    template <class Enum>
    struct diagnostics_writer::diagnostic<Enum, true, false, false, false, true>
    {
        template <class CharT, class Traits>
        static bool print(std::basic_ostream<CharT, Traits> & os, char const * & prefix, char const * delimiter, Enum const & enum_)
        {
            return print_impl<Enum>(os, prefix, delimiter, ": ", static_cast<typename std::underlying_type<Enum>::type>(enum_));
        }
    };

} // namespace detail

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_DETAIL_DIAGNOSTICS_WRITER_HPP_INCLUDED
