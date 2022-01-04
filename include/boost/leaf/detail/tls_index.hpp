#ifndef BOOST_LEAF_TLS_INDEX_HPP_INCLUDED
#define BOOST_LEAF_TLS_INDEX_HPP_INCLUDED

/// Copyright (c) 2018-2021 Emil Dotchevski and Reverge Studios, Inc.

/// Distributed under the Boost Software License, Version 1.0. (See accompanying
/// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_LEAF_ENABLE_WARNINGS ///
#   if defined(_MSC_VER) ///
#       pragma warning(push,1) ///
#   elif defined(__clang__) ///
#       pragma clang system_header ///
#   elif (__GNUC__*100+__GNUC_MINOR__>301) ///
#       pragma GCC system_header ///
#   endif ///
#endif ///

namespace boost { namespace leaf {

    namespace tls
    {
        template <class=void>
        struct BOOST_LEAF_SYMBOL_VISIBLE index_counter
        {
            static BOOST_LEAF_TLS_INDEX_TYPE c;
        };

        template <class T>
        BOOST_LEAF_TLS_INDEX_TYPE index_counter<T>::c;

        template <class T>
        struct BOOST_LEAF_SYMBOL_VISIBLE index
        {
            inline static BOOST_LEAF_TLS_INDEX_TYPE const idx = index_counter<>::c++;
        };
    }

} }

#if defined(_MSC_VER) && !defined(BOOST_LEAF_ENABLE_WARNINGS) ///
#pragma warning(pop) ///
#endif ///

#endif
