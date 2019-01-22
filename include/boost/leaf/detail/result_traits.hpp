#ifndef BOOST_LEAF_CE432B981E8F11E9A65FAF8887D1CC0E
#define BOOST_LEAF_CE432B981E8F11E9A65FAF8887D1CC0E

// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <type_traits>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		struct void_type;

		template <class...>
		using rt_void_t = void_type;

		template <class T, class V=void_type, class E=void_type>
		struct result_traits
		{
			using value_type = V;
			using error_type = E;
		};

		template <class T>
		struct result_traits<T, rt_void_t<decltype(std::declval<T>().value())>, rt_void_t<decltype(std::declval<T>().error())>>
		{
			using value_type = decltype(std::declval<T>().value());
			using error_type = decltype(std::declval<T>().error());
		};

		template <class R>
		struct is_result_type
		{
			using T = typename result_traits<R>::value_type;
			using E = typename result_traits<R>::error_type;
			constexpr static bool value =
				!std::is_same<void_type,T>::value &&
				!std::is_same<void_type,E>::value &&
				!std::is_same<void,E>::value;
		};
	} // namespace leaf_detail

} }

#endif
