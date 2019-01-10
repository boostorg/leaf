#ifndef BOOST_LEAF_017BF91412EB11E9926CDCED8B7F4AFC
#define BOOST_LEAF_017BF91412EB11E9926CDCED8B7F4AFC

//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/throw.hpp>
#include <boost/leaf/result.hpp>
#include <boost/leaf/detail/static_store.hpp>
#include <boost/leaf/detail/demangle.hpp>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <class F>
		leaf::result<typename leaf_detail::function_traits<F>::return_type> catch_exceptions_helper( F && f, leaf_detail_mp11::mp_list<> )
		{
			return std::forward<F>(f)();
		}

		template <class Ex1, class... Ex, class F>
		leaf::result<typename leaf_detail::function_traits<F>::return_type> catch_exceptions_helper( F && f, leaf_detail_mp11::mp_list<Ex1,Ex...> )
		{
			try
			{
				return catch_exceptions_helper(std::forward<F>(f),leaf_detail_mp11::mp_list<Ex...>{ });
			}
			catch( Ex1 const & ex1 )
			{
				return leaf::new_error(ex1);
			}
		}
	}

	template <class... Ex, class F>
	leaf::result<typename leaf_detail::function_traits<F>::return_type> exception_to_result( F && f ) noexcept
	{
		try
		{
			return leaf_detail::catch_exceptions_helper(std::forward<F>(f), leaf_detail_mp11::mp_list<Ex...>());
		}
		catch(...)
		{
			return leaf::new_error(std::current_exception());
		}
	};

} }

#endif
