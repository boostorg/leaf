//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/function_traits.hpp>
#include <functional>

template <class F>
void check_traits( F )
{
	using namespace boost::leaf::leaf_detail;
	static_assert(function_traits<F>::arity==4,"arity");
	static_assert(std::is_same<typename function_traits<F>::ret,double>::value,"ret");
	static_assert(std::is_same<typename function_traits<F>::template arg<0>::type,int>::value,"arg<0>");
	static_assert(std::is_same<typename function_traits<F>::template arg<0>::type_,int>::value,"arg<0>");
	static_assert(std::is_same<typename function_traits<F>::template arg<1>::type,float>::value,"arg<1>");
	static_assert(std::is_same<typename function_traits<F>::template arg<1>::type_,float>::value,"arg<1>");
	static_assert(std::is_same<typename function_traits<F>::template arg<2>::type,int const &>::value,"arg<2>");
	static_assert(std::is_same<typename function_traits<F>::template arg<2>::type_,int>::value,"arg<2>");
	static_assert(std::is_same<typename function_traits<F>::template arg<3>::type,float &&>::value,"arg<3>");
	static_assert(std::is_same<typename function_traits<F>::template arg<3>::type_,float>::value,"arg<3>");
	static_assert(std::is_same<typename function_traits<F>::mp_args,mp_list<int,float,int const &,float &&>>::value,"mp_args");
}

double f1( int, float, int const &, float && )
{
	return 42;
}

int main()
{
	check_traits(&f1);
	check_traits(std::function<double(int const volatile, float const, int const &, float &&)>(f1));
	check_traits( [ ]( int const volatile, float const, int const &, float && ) -> double
		{
			return 42;
		} );
	return 0;
}
