//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/diagnostic_print.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <sstream>

namespace leaf = boost::leaf;

class
c1
	{
	friend
	std::ostream &
	operator<<( std::ostream & os, c1 const & )
		{
		return os << "c1";
		}           
	};
class
c2
	{
	};
std::ostream &
operator<<( std::ostream & os, c2 const & )
	{
	return os << "c2";
	}           
class
c3
	{
	};
template <class T>
bool
check( T const & x, char const * sub )
	{
	using namespace leaf::leaf_detail;
	std::ostringstream s;
	diagnostic<T>::template print<true>(s,x);
	std::string q = s.str();
	return q.find(sub)!=q.npos;
	}
int
main()
	{
	BOOST_TEST(check(42,"42"));
		{
		int x=42;
		int & y = x;
		BOOST_TEST(check(x,"42"));
		BOOST_TEST(check(y,"42"));
		}
	BOOST_TEST(check(c1(),"c1"));
		{
		c1 x;
		c1 & y = x;
		BOOST_TEST(check(x,"c1"));
		BOOST_TEST(check(y,"c1"));
		}
	BOOST_TEST(check(c2(),"c2"));
		{
		c2 x;
		c2 & y = x;
		BOOST_TEST(check(x,"c2"));
		BOOST_TEST(check(y,"c2"));
		}
	BOOST_TEST(check(c3(),"N/A"));
		{
		c3 x;
		c3 & y = x;
		BOOST_TEST(check(x,"N/A"));
		BOOST_TEST(check(y,"N/A"));
		}
	return boost::report_errors();
	}
