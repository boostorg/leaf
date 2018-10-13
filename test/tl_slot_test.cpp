//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/tl_slot.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <future>
#include <vector>
#include <iterator>
#include <algorithm>

namespace leaf = boost::leaf;

struct error { };

template <class T>
int &
count()
	{
	static thread_local int c=0;
	return c;
	}

template <class Tag>
class
my_info
	{
	my_info & operator=( my_info const & ) = delete;
	public:
	int value;
	explicit
	my_info( int value ):
		value(value)
		{
		BOOST_TEST(++count<my_info>()>0);
		}
	my_info( my_info const & x ):
		value(x.value)
		{
		BOOST_TEST(++count<my_info>()>0);
		}
	my_info( my_info && x ):
		value(x.value)
		{
		x.value=-1;
		BOOST_TEST(++count<my_info>()>0);
		}
	~my_info()
		{
		BOOST_TEST(--count<my_info>()>=0);
		}
	};

class
throws_on_copy
	{
	throws_on_copy & operator=( throws_on_copy const & )=delete;
	public:
	int value;
	throws_on_copy()
		{
		BOOST_TEST(++count<throws_on_copy>()>0);
		}
	throws_on_copy( throws_on_copy const & )
		{
		throw error();
		}
	throws_on_copy( throws_on_copy && )
		{
		BOOST_TEST(++count<throws_on_copy>()>0);
		}
	~throws_on_copy()
		{
		BOOST_TEST(--count<throws_on_copy>()>=0);
		}
	};

struct A;
struct B;
struct C;

void
run_tests()
	{
	using leaf::leaf_detail::tl_slot;
		{
			{
			throws_on_copy a;
			BOOST_TEST(count<throws_on_copy>()==1);
			auto & x = tl_slot<throws_on_copy>::tl_instance();
			BOOST_TEST(!x.is_open());
			BOOST_TEST(!x.put(a));
			BOOST_TEST(count<throws_on_copy>()==1);
			BOOST_TEST(!x.has_value());
			BOOST_TEST(x.open()==1);
			BOOST_TEST(x.put(throws_on_copy()));
			BOOST_TEST(count<throws_on_copy>()==2);
			BOOST_TEST(x.has_value());
			try
				{
				(void) x.put(a);
				BOOST_TEST(false);
				}
			catch( error & )
				{
				}
			BOOST_TEST(count<throws_on_copy>()==1);
			BOOST_TEST(!x.has_value());
			BOOST_TEST(x.is_open());
			BOOST_TEST(x.close()==0);
			}
		BOOST_TEST(count<throws_on_copy>()==0);
			{
			auto & x = tl_slot<my_info<A>>::tl_instance();
			BOOST_TEST(&x==&tl_slot<my_info<A>>::tl_instance());
			BOOST_TEST(!x.is_open());
			BOOST_TEST(!x.has_value());
			BOOST_TEST(!x.put(my_info<A>(42)));
			BOOST_TEST(!x.has_value());
			BOOST_TEST(x.open()==1);
			BOOST_TEST(x.is_open());
			BOOST_TEST(!x.has_value());
			BOOST_TEST(x.open()==2);
			BOOST_TEST(x.is_open());
			BOOST_TEST(!x.has_value());
			BOOST_TEST(count<my_info<A>>()==0);
			BOOST_TEST(x.put(my_info<A>(42)));
			BOOST_TEST(x.close()==1);
			BOOST_TEST(x.is_open());
			BOOST_TEST(x.has_value());
			BOOST_TEST(count<my_info<A>>()==1);
			BOOST_TEST(x.close()==0);
			BOOST_TEST(!x.is_open());
			BOOST_TEST(!x.has_value());
			BOOST_TEST(count<my_info<A>>()==0);
			BOOST_TEST(x.open()==1);
			BOOST_TEST(x.is_open());
			BOOST_TEST(!x.has_value());
			BOOST_TEST(count<my_info<A>>()==0);
			BOOST_TEST(x.put(my_info<A>(43)));
			BOOST_TEST(x.is_open());
			BOOST_TEST(x.has_value());
			BOOST_TEST(x.open()==2);
			BOOST_TEST(x.is_open());
			BOOST_TEST(x.close()==1);
			BOOST_TEST(x.close()==0);
			BOOST_TEST(!x.has_value());
			}
			{
			auto & x = tl_slot<my_info<B>>::tl_instance();
			BOOST_TEST(&x==&tl_slot<my_info<B>>::tl_instance());
			BOOST_TEST(!x.is_open());
			BOOST_TEST(!x.has_value());
				{
				my_info<B> tmp(42);
				BOOST_TEST(!x.put(tmp));
				}
			BOOST_TEST(!x.has_value());
			BOOST_TEST(x.open()==1);
			BOOST_TEST(x.is_open());
			BOOST_TEST(!x.has_value());
			BOOST_TEST(x.open()==2);
			BOOST_TEST(x.is_open());
			BOOST_TEST(!x.has_value());
			BOOST_TEST(count<my_info<B>>()==0);
				{
				my_info<B> tmp(42);
				BOOST_TEST(x.put(tmp));
				}
			BOOST_TEST(x.close()==1);
			BOOST_TEST(x.is_open());
			BOOST_TEST(x.has_value());
			BOOST_TEST(count<my_info<B>>()==1);
			BOOST_TEST(x.close()==0);
			BOOST_TEST(!x.is_open());
			BOOST_TEST(!x.has_value());
			BOOST_TEST(count<my_info<B>>()==0);
			BOOST_TEST(x.open()==1);
			BOOST_TEST(x.is_open());
			BOOST_TEST(!x.has_value());
			BOOST_TEST(count<my_info<B>>()==0);
				{
				my_info<B> tmp(43);
				BOOST_TEST(x.put(tmp));
				}
			BOOST_TEST(x.is_open());
			BOOST_TEST(x.has_value());
			BOOST_TEST(x.open()==2);
			BOOST_TEST(x.is_open());
			BOOST_TEST(x.close()==1);
			BOOST_TEST(x.close()==0);
			BOOST_TEST(!x.has_value());
			}
			{
			auto & x = tl_slot<my_info<C>>::tl_instance();
			BOOST_TEST(x.open()==1);
			BOOST_TEST(x.is_open());
			BOOST_TEST(!x.has_value());
			BOOST_TEST(x.put(my_info<C>(42)));
			BOOST_TEST(x.has_value());
			x.reset();
			BOOST_TEST(x.is_open());
			BOOST_TEST(!x.has_value());
			BOOST_TEST(x.close()==0);
			}
			{
			auto & x = tl_slot<my_info<A>>::tl_instance();
			BOOST_TEST(x.open()==1);
			BOOST_TEST(x.is_open());
			BOOST_TEST(!x.has_value());
			BOOST_TEST(x.put(my_info<A>(42)));
			BOOST_TEST(x.has_value());
			my_info<A> inf = x.extract_value();
			BOOST_TEST(inf.value==42);
			BOOST_TEST(x.is_open());
			BOOST_TEST(!x.has_value());
			BOOST_TEST(x.close()==0);
			}
		}
	BOOST_TEST(count<my_info<A>>()==0);
	BOOST_TEST(count<my_info<B>>()==0);
	BOOST_TEST(count<my_info<C>>()==0);
	}

int
main()
	{
	std::vector<std::future<void>> fut;
	std::generate_n( std::inserter(fut,fut.end()), 100, [ ]
		{
		return std::async( std::launch::async, &run_tests );
		} );
	for( auto & i : fut )
		i.wait();
	for( auto & i : fut )
		i.get();
	return boost::report_errors();
	}
