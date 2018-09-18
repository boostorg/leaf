//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/expect.hpp>
#include <boost/leaf/put.hpp>
#include <boost/detail/lightweight_test.hpp>

namespace leaf = boost::leaf;

struct error { };

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
		BOOST_TEST(tl_slot<my_info<2>>::tl_instance().is_open()==0);
		BOOST_TEST(tl_slot<my_info<3>>::tl_instance().is_open()==0);
		leaf::expect<my_info<2>,my_info<3>> info;
		BOOST_TEST(tl_slot<my_info<2>>::tl_instance().is_open()==1);
		BOOST_TEST(tl_slot<my_info<3>>::tl_instance().is_open()==1);
		try
			{
			auto put = leaf::preload( my_info<2>(42), my_info<3>(43) );
			throw error();
			}
		catch( error const & )
			{
			BOOST_TEST(tl_slot<my_info<2>>::tl_instance().has_value());
			BOOST_TEST(tl_slot<my_info<3>>::tl_instance().has_value());
			bool called = false;
			unwrap( info.match<my_info<3>,my_info<2>>( [&called]( int x3, int x2 )
				{
				called = true;
				BOOST_TEST(x2==42);
				BOOST_TEST(x3==43);
				} ) );
			BOOST_TEST(called);
			}
		}
	BOOST_TEST(!tl_slot<my_info<1>>::tl_instance().is_open());
	BOOST_TEST(!tl_slot<my_info<2>>::tl_instance().is_open());
	BOOST_TEST(!tl_slot<my_info<3>>::tl_instance().is_open());
	BOOST_TEST(!tl_slot<my_info<1>>::tl_instance().has_value());
	BOOST_TEST(!tl_slot<my_info<2>>::tl_instance().has_value());
	BOOST_TEST(!tl_slot<my_info<3>>::tl_instance().has_value());
	BOOST_TEST(total_count()==0);
	///////////////////////////////////////////////
		{
		leaf::expect<my_info<2>,my_info<3>> info;
		BOOST_TEST(!tl_slot<my_info<1>>::tl_instance().is_open());
		BOOST_TEST(tl_slot<my_info<2>>::tl_instance().is_open()==1);
		BOOST_TEST(tl_slot<my_info<3>>::tl_instance().is_open()==1);
		try
			{
			leaf::expect<my_info<1>,my_info<2>> info;
			BOOST_TEST(tl_slot<my_info<1>>::tl_instance().is_open()==1);
			BOOST_TEST(tl_slot<my_info<2>>::tl_instance().is_open()==2);
			BOOST_TEST(tl_slot<my_info<3>>::tl_instance().is_open()==1);
			try
				{
				auto put = leaf::preload(my_info<1>(41));
				throw error();
				}
			catch(...)
				{
					{
					bool called = false;
					unwrap( info.match<my_info<1>>( [&called]( int x1 )
						{
						called = true;
						BOOST_TEST(x1==41);
						} ) );
					BOOST_TEST(called);
					}
				try
					{
					unwrap( info.match<my_info<1>,my_info<2>>( [ ]( int, int ) { } ) );
					BOOST_TEST(false);
					}
				catch( leaf::mismatch_error & )
					{
					}
				leaf::put(my_info<2>(42));
					{
					bool called = false;
					unwrap( info.match<my_info<1>,my_info<2>>( [&called]( int x1, int x2 )
						{
						called = true;
						BOOST_TEST(x1==41);
						BOOST_TEST(x2==42);
						} ) );
					BOOST_TEST(called);
					}
				info.rethrow_with_current_info();
				BOOST_TEST(false);
				}
			}
		catch( error const & )
			{
			BOOST_TEST(!tl_slot<my_info<1>>::tl_instance().has_value());
			BOOST_TEST(tl_slot<my_info<2>>::tl_instance().has_value());
			BOOST_TEST(!tl_slot<my_info<3>>::tl_instance().has_value());
				{
				bool called = false;
				unwrap( info.match<my_info<2>>( [&called]( int x2 )
					{
					called = true;
					BOOST_TEST(x2==42);
					} ) );
				BOOST_TEST(called);
				}
			}
		}
	BOOST_TEST(!tl_slot<my_info<1>>::tl_instance().is_open());
	BOOST_TEST(!tl_slot<my_info<2>>::tl_instance().is_open());
	BOOST_TEST(!tl_slot<my_info<3>>::tl_instance().is_open());
	BOOST_TEST(!tl_slot<my_info<1>>::tl_instance().has_value());
	BOOST_TEST(!tl_slot<my_info<2>>::tl_instance().has_value());
	BOOST_TEST(!tl_slot<my_info<3>>::tl_instance().has_value());
	BOOST_TEST(total_count()==0);
	///////////////////////////////////////////////
		{
		leaf::expect<my_info<1>> info1;
		try
			{
			leaf::expect<my_info<1>> info2;
			leaf::throw_with_info(error(),my_info<1>(42));
			BOOST_TEST(false);
			}
		catch( error const & )
			{
			BOOST_TEST(tl_slot<my_info<1>>::tl_instance().has_value());
			unwrap(info1.match<my_info<1>>([ ] ( int x )
				{
				BOOST_TEST(x==42);
				} ) );
			}
		}
	BOOST_TEST(!tl_slot<my_info<1>>::tl_instance().has_value());
	///////////////////////////////////////////////
		{
		leaf::expect<my_info<1>> info1;
			{
			leaf::expect<my_info<1>> info2;
			try
				{
				leaf::throw_with_info(error(),my_info<1>(42));
				BOOST_TEST(false);
				}
			catch( error const & )
				{
				BOOST_TEST(tl_slot<my_info<1>>::tl_instance().has_value());
				unwrap(info1.match<my_info<1>>([ ] ( int x )
					{
					BOOST_TEST(x==42);
					} ) );
				}
			}
		BOOST_TEST(!tl_slot<my_info<1>>::tl_instance().has_value());
		}
	///////////////////////////////////////////////
		{
		leaf::expect<my_info<1>> info1;
		try
			{
			leaf::expect<my_info<1>> info2;
			try
				{
				leaf::throw_with_info(error(),my_info<1>(42));
				BOOST_TEST(false);
				}
			catch( error const & )
				{
				BOOST_TEST(tl_slot<my_info<1>>::tl_instance().has_value());
				unwrap(info1.match<my_info<1>>([ ] ( int x )
					{
					BOOST_TEST(x==42);
					} ) );
				throw error();
				}
			}
		catch( error const & )
			{
			BOOST_TEST(tl_slot<my_info<1>>::tl_instance().has_value());
			unwrap(info1.match<my_info<1>>([ ] ( int x )
				{
				BOOST_TEST(x==42);
				} ) );
			}
		BOOST_TEST(tl_slot<my_info<1>>::tl_instance().has_value());
		}
	BOOST_TEST(!tl_slot<my_info<1>>::tl_instance().has_value());
	///////////////////////////////////////////////
		{
		leaf::expect<my_info<1>> info1;
		try
			{
			try
				{
				leaf::throw_with_info(error(),my_info<1>(42));
				BOOST_TEST(false);
				}
			catch( error const & )
				{
				BOOST_TEST(tl_slot<my_info<1>>::tl_instance().has_value());
				leaf::available info;
				unwrap(info.match<my_info<1>>([ ] ( int x )
					{
					BOOST_TEST(x==42);
					} ) );
				throw error();
				}
			}
		catch( error const & )
			{
			}
		BOOST_TEST(!tl_slot<my_info<1>>::tl_instance().has_value());
		}
	///////////////////////////////////////////////
		{
		leaf::expect<my_info<1>> info1;
		try
			{
			try
				{
				leaf::throw_with_info(error(),my_info<1>(42));
				BOOST_TEST(false);
				}
			catch( error const & )
				{
				BOOST_TEST(tl_slot<my_info<1>>::tl_instance().has_value());
				leaf::available info;
				unwrap(info.match<my_info<1>>([ ] ( int x )
					{
					BOOST_TEST(x==42);
					} ) );
				info.rethrow_with_current_info();
				BOOST_TEST(false);
				}
			}
		catch( error const & )
			{
			}
		BOOST_TEST(tl_slot<my_info<1>>::tl_instance().has_value());
		}
	BOOST_TEST(!tl_slot<my_info<1>>::tl_instance().has_value());
	return boost::report_errors();
	}
