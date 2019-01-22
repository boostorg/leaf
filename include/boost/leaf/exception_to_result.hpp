#ifndef BOOST_LEAF_017BF91412EB11E9926CDCED8B7F4AFC
#define BOOST_LEAF_017BF91412EB11E9926CDCED8B7F4AFC

// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/throw.hpp>
#include <boost/leaf/result.hpp>
#include <boost/leaf/detail/static_store.hpp>
#include <boost/leaf/detail/demangle.hpp>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		inline error_id catch_exceptions_helper( std::exception const & ex, leaf_detail_mp11::mp_list<> )
		{
			return leaf::new_error(ex);
		}

		template <class Ex1, class... Ex>
		error_id catch_exceptions_helper( std::exception const & ex, leaf_detail_mp11::mp_list<Ex1,Ex...> )
		{
			if( Ex1 const * p = dynamic_cast<Ex1 const *>(&ex) )
				return leaf::new_error(*p);
			else
				return catch_exceptions_helper(ex, leaf_detail_mp11::mp_list<Ex...>{ });
		}

		////////////////////////////////////////

		template <class T>
		struct deduce_exception_to_result_return_type
		{
			using type = result<T>;
		};

		template <class T>
		struct deduce_exception_to_result_return_type<result<T>>
		{
			using type = result<T>;
		};
	}

	template <class... Ex, class F>
	typename leaf_detail::deduce_exception_to_result_return_type<typename leaf_detail::function_traits<F>::return_type>::type exception_to_result( F && f ) noexcept
	{
		try
		{
			return std::forward<F>(f)();
		}
		catch( std::exception const & ex )
		{
			return leaf_detail::catch_exceptions_helper(ex, leaf_detail_mp11::mp_list<Ex...>());
		}
		catch(...)
		{
			return leaf::new_error(std::current_exception());
		}
	};

} }

#endif
