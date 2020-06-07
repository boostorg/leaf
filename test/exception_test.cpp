// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>
#ifdef LEAF_NO_EXCEPTIONS

#include <iostream>

int main()
{
	std::cout << "Unit test not applicable." << std::endl;
	return 0;
}

#else

#include <boost/leaf/handle_exception.hpp>
#include <boost/leaf/exception.hpp>
#include <boost/leaf/on_error.hpp>
#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

struct info { int value; };
struct extra_info { int value; };

struct my_error: std::exception { };

struct extra_info_exception:
	std::exception
{
	extra_info_exception() noexcept
	{
		leaf::current_error().load(extra_info{42});
	}
};

template <class F>
int test( F && f )
{
	return leaf::try_catch(
		[&]() -> int
		{
			f();
			return 1;
		},
		[]( leaf::catch_<my_error>, leaf::match<info,42>, leaf::e_source_location )
		{
			return 2;
		},
		[]( leaf::catch_<my_error>, leaf::match<info,42>, info x )
		{
			return 3;
		},
		[]( leaf::catch_<my_error>, leaf::e_source_location )
		{
			return 4;
		},
		[]( leaf::catch_<my_error> )
		{
			return 5;
		},
		[]( leaf::match<info,42>, leaf::e_source_location )
		{
			return 6;
		},
		[]( leaf::match<info,42>, info x )
		{
			return 7;
		},
		[]( leaf::e_source_location )
		{
			return 8;
		},
		[]
		{
			return 9;
		} );
}

int main()
{
	{
		int const id = leaf::leaf_detail::current_id();
		BOOST_TEST_EQ( 3, test( []
		{
			auto load = leaf::on_error(info{42});
			throw my_error();
		} ) );
		BOOST_TEST_NE(id, leaf::leaf_detail::current_id());
	}

	{
		BOOST_TEST_EQ( 5, test( []
		{
			int const id = leaf::leaf_detail::current_id();
			try
			{
				leaf::try_catch(
					[]
					{
						throw my_error();
					} );
			}
			catch(...)
			{
				BOOST_TEST_EQ(id, leaf::leaf_detail::current_id());
				throw;
			}
		} ) );
	}

	BOOST_TEST_EQ( 9, test( [] { throw leaf::exception(); } ) );
	BOOST_TEST_EQ( 7, test( [] { throw leaf::exception(info{42}); } ) );

	BOOST_TEST_EQ( 8, test( [] { throw LEAF_EXCEPTION(); } ) );
	BOOST_TEST_EQ( 6, test( [] { throw LEAF_EXCEPTION(info{42}); } ) );

	BOOST_TEST_EQ( 5, test( [] { throw leaf::exception(my_error()); } ) );
	BOOST_TEST_EQ( 3, test( [] { throw leaf::exception(my_error(),info{42}); } ) );

	BOOST_TEST_EQ( 4, test( [] { throw LEAF_EXCEPTION(my_error()); } ) );
	BOOST_TEST_EQ( 2, test( [] { throw LEAF_EXCEPTION(my_error(),info{42}); } ) );

	BOOST_TEST_EQ( 4, test( [] { LEAF_THROW(my_error()); } ) );
	BOOST_TEST_EQ( 2, test( [] { LEAF_THROW(my_error(),info{42}); } ) );

	{
		char const * wh = 0;
		leaf::try_catch(
			[]
			{
				throw std::runtime_error("Test");
			},
			[&]( leaf::catch_<std::exception> ex )
			{
				wh = ex.value().what();
			} );
		BOOST_TEST(wh!=0 || !strcmp(wh,"Test"));
	}

	{
		leaf::try_catch(
			[]
			{
				auto id = leaf::new_error();
				throw leaf::exception(id, extra_info_exception(), info{42});
			},
			[]( leaf::catch_<extra_info_exception>, leaf::match<extra_info, 42>, leaf::match<info, 42> )
			{
			},
			[]
			{
				BOOST_ERROR("Missing info{42}!");
			} );
	}

	{
		leaf::try_catch(
			[]
			{
				auto id = leaf::new_error(extra_info{42});
				throw leaf::exception(id, info{42});
			},
			[]( leaf::match<extra_info, 42>, leaf::match<info, 42> )
			{
			},
			[]
			{
				BOOST_ERROR("Missing info{42}!");
			} );
	}

	{
		leaf::try_catch(
			[]
			{
				auto id = leaf::new_error();
				throw LEAF_EXCEPTION(id, extra_info_exception(), info{42});
			},
			[]( leaf::catch_<extra_info_exception>, leaf::match<extra_info, 42>, leaf::match<info, 42>, leaf::e_source_location )
			{
			},
			[]
			{
				BOOST_ERROR("Missing info{42}!");
			} );
	}

	{
		leaf::try_catch(
			[]
			{
				auto id = leaf::new_error(extra_info{42});
				throw LEAF_EXCEPTION(id, info{42});
			},
			[]( leaf::match<extra_info, 42>, leaf::match<info, 42>, leaf::e_source_location )
			{
			},
			[]
			{
				BOOST_ERROR("Missing info{42}!");
			} );
	}

	return boost::report_errors();
}

#endif
