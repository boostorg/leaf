#ifndef BOOST_LEAF_DETAIL_WRITER_HPP_INCLUDED
#define BOOST_LEAF_DETAIL_WRITER_HPP_INCLUDED

// Copyright 2018-2026 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>
#include <boost/leaf/detail/type_name.hpp>
#include <boost/leaf/detail/function_traits.hpp>

namespace boost { namespace leaf {

namespace serialization
{
    struct writer_adl {};

    template <class Writer, class E>
    auto write(Writer & w, E const & e) -> decltype(write(w, e.value))
    {
        write(w, e.value);
    }
}

namespace detail
{
    class writer:
        serialization::writer_adl
    {
        writer(writer const &) = delete;
        writer & operator=(writer const &) = delete;

        type_name const type_;
        void * const w_;

        bool dispatch_()
        {
            return false;
        }

        template <class F1, class... Fn>
        bool dispatch_(F1 && f1, Fn && ... fn)
        {
            using writer_type = typename std::decay<fn_arg_type<F1, 0>>::type;
            if (writer_type * w = get<writer_type>())
            {
                std::forward<F1>(f1)(*w);
                return true;
            }
            return dispatch_(std::forward<Fn>(fn)...);
        }

    protected:

        template <class Writer>
        explicit writer(Writer * w) noexcept:
            type_(get_type_name<Writer>()),
            w_(w)
        {
        }

    public:

        template <class Writer>
        Writer * get() noexcept
        {
            return type_ == get_type_name<Writer>() ? static_cast<Writer *>(w_) : nullptr;
        }

        template <class... Fn>
        bool dispatch(Fn && ... fn)
        {
            using writer_types = leaf_detail_mp11::mp_list<typename std::decay<fn_arg_type<Fn, 0>>::type...>;
            static_assert(std::is_same<writer_types, leaf_detail_mp11::mp_unique<writer_types>>::value, "Duplicate writer types in dispatch");
            return dispatch_(std::forward<Fn>(fn)...);
        }
    };

    template <class Writer>
    struct writer_adaptor:
        writer
    {
        explicit writer_adaptor(Writer & w) noexcept:
            writer(&w)
        {
        }
    };
} // namespace detail

} } // namespace boost::leaf

#endif // #ifndef BOOST_LEAF_DETAIL_WRITER_HPP_INCLUDED
