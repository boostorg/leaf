// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/handle.hpp>

namespace leaf = boost::leaf;

template <class... Handler>
typename leaf::leaf_detail::deduce_static_store<typename leaf::leaf_detail::handler_args_set<Handler...>::type>::type * expd( Handler && ... )
{
	return 0;
}

template <class T, class U>
void test( U * )
{
	static_assert(std::is_same<T,U>::value,"static_store_deduction");
}

template <int> struct info { int value; };

enum class my_error_code
{
	ok,
	error1,
	error2,
	error3
};
namespace boost { namespace leaf {
	template <> struct is_e_type<my_error_code>: std::true_type { };
} }

void not_called_on_purpose()
{
	using leaf::leaf_detail::static_store;

	test< static_store<info<1>> >( expd([ ]( info<1> ){ }) );
	test< static_store<info<1>> >( expd([ ]( info<1> const ){ }) );
	test< static_store<info<1>> >( expd([ ]( info<1> const & ){ }) );
	test< static_store<info<1>> >( expd([ ]( info<1>, leaf::error_info const & ){ }) );

	test< static_store<info<1>,info<2>> >( expd([ ]( info<1> ){ }, [ ]( info<2> ){ }) );
	test< static_store<info<1>,info<2>> >( expd([ ]( info<1> ){ }, [ ]( info<1>, info<2> ){ }) );
	test< static_store<info<1>,info<2>> >( expd([ ]( info<1>, info<2> ){ }, [ ]( info<2> ){ }) );
	test< static_store<info<1>,info<2>> >( expd([ ]( info<1>, info<2> ){ }, [ ]( info<1>, info<2> ){ }) );

	test< static_store<info<1>,info<2>> >( expd([ ]( leaf::error_info const &, info<1> ){ }, [ ]( info<2> ){ }) );
	test< static_store<info<1>,info<2>> >( expd([ ]( leaf::error_info const &, info<1> ){ }, [ ]( info<1>, info<2> ){ }) );
	test< static_store<info<1>,info<2>> >( expd([ ]( leaf::error_info const &, info<1>, info<2> ){ }, [ ]( info<2> ){ }) );
	test< static_store<info<1>,info<2>> >( expd([ ]( leaf::error_info const &, info<1>, info<2> ){ }, [ ]( info<1>, info<2> ){ }) );

	test< static_store<info<1>,info<2>> >( expd([ ]( info<1>, leaf::error_info const & ){ }, [ ]( info<2> ){ }) );
	test< static_store<info<1>,info<2>> >( expd([ ]( info<1>, leaf::error_info const & ){ }, [ ]( info<1>, info<2> ){ }) );
	test< static_store<info<1>,info<2>> >( expd([ ]( info<1>, leaf::error_info const &, info<2> ){ }, [ ]( info<2> ){ }) );
	test< static_store<info<1>,info<2>> >( expd([ ]( info<1>, leaf::error_info const &, info<2> ){ }, [ ]( info<1>, info<2> ){ }) );

	test< static_store<info<1>,info<2>> >( expd([ ]( info<1> ){ }, [ ]( leaf::error_info const &, info<2> ){ }) );
	test< static_store<info<1>,info<2>> >( expd([ ]( info<1> ){ }, [ ]( leaf::error_info const &, info<1>, info<2> ){ }) );
	test< static_store<info<1>,info<2>> >( expd([ ]( info<1>, info<2> ){ }, [ ]( leaf::error_info const &, info<2> ){ }) );
	test< static_store<info<1>,info<2>> >( expd([ ]( info<1>, info<2> ){ }, [ ]( leaf::error_info const &, info<1>, info<2> ){ }) );

	test< static_store<info<1>,info<2>> >( expd([ ]( info<1> ){ }, [ ]( info<2>, leaf::error_info const & ){ }) );
	test< static_store<info<1>,info<2>> >( expd([ ]( info<1> ){ }, [ ]( info<1>, leaf::error_info const &, info<2> ){ }) );
	test< static_store<info<1>,info<2>> >( expd([ ]( info<1>, info<2> ){ }, [ ]( info<2>, leaf::error_info const & ){ }) );
	test< static_store<info<1>,info<2>> >( expd([ ]( info<1>, info<2> ){ }, [ ]( info<1>, leaf::error_info const &, info<2> ){ }) );

	test< static_store<info<1>,info<2>,info<3>> >( expd([ ]( info<1> ){ }, [ ]( info<2> ){ }, [ ]( info<3> ){ }) );
	test< static_store<info<1>,info<2>,info<3>> >( expd([ ]( info<1> ){ }, [ ]( info<1>, info<2> ){ }, [ ]( info<1>, info<3> ){ }) );
	test< static_store<info<1>,info<2>,info<3>> >( expd([ ]( info<1> ){ }, [ ]( info<1>, info<2> ){ }, [ ]( info<1>, info<3> ){ }) );
	test< static_store<info<1>,info<2>,info<3>> >( expd([ ]( info<1>, info<2> ){ }, [ ]( info<2> ){ }, [ ]( info<3> ){ }) );
	test< static_store<info<1>,info<2>,info<3>> >( expd([ ]( info<1>, info<2> ){ }, [ ]( info<2> ){ }, [ ]( info<3> ){ }) );
	test< static_store<info<1>,info<2>,info<3>> >( expd([ ]( info<1> ){ }, [ ]( info<2> ){ }, [ ]( info<3>, info<2> ){ }) );
	test< static_store<info<1>,info<2>,info<3>> >( expd([ ]( info<1> ){ }, [ ]( info<2> ){ }, [ ]( info<3>, info<2> ){ }) );
	test< static_store<info<1>,info<3>,info<2>> >( expd([ ]( info<1>, info<3> ){ }, [ ]( info<2> ){ }, [ ]( info<3> ){ }) );
	test< static_store<info<1>,info<3>,info<2>> >( expd([ ]( info<1>, info<3> ){ }, [ ]( info<2> ){ }, [ ]( info<3> ){ }) );
	test< static_store<info<1>,info<2>,info<3>> >( expd([ ]( info<1> ){ }, [ ]( info<2>, info<3> ){ }, [ ]( info<3> ){ }) );
	test< static_store<info<1>,info<2>,info<3>> >( expd([ ]( info<1> ){ }, [ ]( info<2>, info<3> ){ }, [ ]( info<3> ){ }) );

	test< static_store<my_error_code> >( expd([ ]( leaf::match<my_error_code,my_error_code::error1> ){ }) );
	test< static_store<info<1>> >( expd([ ]( leaf::match<info<1>,42> ){ }) );
	test< static_store<info<1>> >( expd([ ]( leaf::catch_<std::exception>, info<1> ){ }) );

	test< static_store<info<1>,info<2>,info<3>> >( expd([ ]( info<1> const *, info<2> ){ }, [ ]( info<1>, info<3> const * ){ }) );
	test< static_store<info<1>,info<2>,info<3>> >( expd([ ]( info<1> const, info<2> ){ }, [ ]( info<1> const *, info<3> ){ }) );
}

int main()
{
	return 0;
}
