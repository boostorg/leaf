//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/expect.hpp>
#include <boost/leaf/result.hpp>
#include <boost/detail/lightweight_test.hpp>

namespace leaf = boost::leaf;

int &
total_count()
	{
	static thread_local int c=0;
	return c;
	}
template <class T>
int &
count()
	{
	static thread_local int c=0;
	return c;
	}
template <int>
struct
my_info
	{
	my_info( my_info const & ) = delete;
	my_info & operator=( my_info const & ) = delete;
	public:
	int value;
	explicit
	my_info( int value ):
		value(value)
		{
		++total_count();
		++count<my_info>();
		}
	my_info( my_info && x ):
		value(x.value)
		{
		++total_count();
		++count<my_info>();
		}
	~my_info()
		{
		--total_count();
		--count<my_info>();
		}
	};
int
main()
	{
	using namespace leaf::leaf_detail;
	///////////////////////////////////////////////
		{
		leaf::expect<my_info<2>,my_info<3>> exp;
		BOOST_TEST(!leaf::leaf_detail::current_error_flag());
		leaf::result<void> r = [ ]
			{
			auto put = leaf::preload( my_info<2>(42), my_info<3>(43) );
			return leaf::error();
			}();
		BOOST_TEST(!r);
		BOOST_TEST(leaf::leaf_detail::current_error_flag());
		BOOST_TEST((!leaf::peek<my_info<1>>(exp,r)));
		BOOST_TEST((leaf::peek<my_info<2>>(exp,r) && *leaf::peek<my_info<2>>(exp,r)==42));
		BOOST_TEST((leaf::peek<my_info<3>>(exp,r) && *leaf::peek<my_info<3>>(exp,r)==43));
		bool called = false;
		handle_error( exp, r, leaf::match<my_info<3>,my_info<2>>( [&called]( int x3, int x2 )
			{
			called = true;
			BOOST_TEST(leaf::leaf_detail::current_error_flag());
			BOOST_TEST(x2==42);
			BOOST_TEST(x3==43);
			} ) );
		BOOST_TEST(called);
		BOOST_TEST(!leaf::leaf_detail::current_error_flag());
		}
	BOOST_TEST(total_count()==0);
	///////////////////////////////////////////////
		{
		leaf::expect<my_info<2>,my_info<3>> exp;
		BOOST_TEST(!leaf::leaf_detail::current_error_flag());
		leaf::result<void> r = [ ]
			{
			leaf::expect<my_info<1>,my_info<2>> exp;
			BOOST_TEST(!leaf::leaf_detail::current_error_flag());
			leaf::result<void> r = [ ]
				{
				auto put = leaf::preload(my_info<2>(42));
				return leaf::error();
				}();
			BOOST_TEST(!r);
			BOOST_TEST(leaf::leaf_detail::current_error_flag());
			BOOST_TEST((!leaf::peek<my_info<1>>(exp,r)));
			BOOST_TEST((leaf::peek<my_info<2>>(exp,r) && *leaf::peek<my_info<2>>(exp,r)==42));
			BOOST_TEST((!leaf::peek<my_info<3>>(exp,r)));
			return r.error();
			}();
		BOOST_TEST(!r);
		BOOST_TEST(leaf::leaf_detail::current_error_flag());
		BOOST_TEST((!leaf::peek<my_info<1>>(exp,r)));
		BOOST_TEST((leaf::peek<my_info<2>>(exp,r) && *leaf::peek<my_info<2>>(exp,r)==42));
		BOOST_TEST((!leaf::peek<my_info<3>>(exp,r)));
		bool called = false;
		handle_error( exp, r, leaf::match<my_info<2>>( [&called]( int x2 )
			{
			called = true;
			BOOST_TEST(leaf::leaf_detail::current_error_flag());
			BOOST_TEST(x2==42);
			} ) );
		BOOST_TEST(called);
		BOOST_TEST(!leaf::leaf_detail::current_error_flag());
		}
	BOOST_TEST(total_count()==0);
	return boost::report_errors();
	}
