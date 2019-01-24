// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/try.hpp>
#include <boost/leaf/throw.hpp>
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int> struct info { int value; };

struct error1: std::exception { };
struct error2: std::exception { };
struct error3: std::exception { };

template <class R,class Ex>
R f( Ex && ex )
{
	throw leaf::exception(std::move(ex),info<1>{1},info<2>{2},info<3>{3});
}

template <class R>
R f()
{
	return R(42);
}

int main()
{
	// void, handle_all (success)
	{
		int c=0;
		leaf::try_(
			[&c]
			{
				c = f<int>();
			},
			[&c]
			{
				BOOST_TEST(c==0);
				c = 1;
			} );
		BOOST_TEST(c==42);
	}

	// void, handle_all (failure), match_enum (single enum value)
	{
		int c=0;
		leaf::try_(
			[&c]
			{
				c = f<int>(error1());
			},
			[&c]( leaf::catch_<error2> )
			{
				BOOST_TEST(c==0);
				c = 1;
			},
			[&c]( leaf::catch_<error1>, info<1> const & x, info<2> y )
			{
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				BOOST_TEST(c==0);
				c = 2;
			},
			[&c]
			{
				BOOST_TEST(c==0);
				c = 3;
			} );
		BOOST_TEST(c==2);
	}

	// void, handle_all (failure), match_enum (multiple enum values)
	{
		int c=0;
		leaf::try_(
			[&c]
			{
				c = f<int>(error1());
			},
			[&c]( leaf::catch_<error2> )
			{
				BOOST_TEST(c==0);
				c = 1;
			},
			[&c]( leaf::catch_<error2,error1>, info<1> const & x, info<2> y )
			{
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				BOOST_TEST(c==0);
				c = 2;
			},
			[&c]
			{
				BOOST_TEST(c==0);
				c = 3;
			} );
		BOOST_TEST(c==2);
	}

	// void, handle_all (failure), match_value (single value)
	{
		int c=0;
		leaf::try_(
			[&c]
			{
				c = f<int>(error1());
			},
			[&c]( leaf::catch_<error2> )
			{
				BOOST_TEST(c==0);
				c = 1;
			},
			[&c]( leaf::catch_<error1>, info<1> const & x, info<2> y )
			{
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				BOOST_TEST(c==0);
				c = 2;
			},
			[&c]
			{
				BOOST_TEST(c==0);
				c = 3;
			} );
		BOOST_TEST(c==2);
	}

	// void, handle_all (failure), match_value (multiple values)
	{
		int c=0;
		leaf::try_(
			[&c]
			{
				c = f<int>(error1());
			},
			[&c]( leaf::catch_<error2> )
			{
				BOOST_TEST(c==0);
				c = 1;
			},
			[&c]( leaf::catch_<error2,error1>, info<1> const & x, info<2> y )
			{
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				BOOST_TEST(c==0);
				c = 2;
			},
			[&c]
			{
				BOOST_TEST(c==0);
				c = 3;
			} );
		BOOST_TEST(c==2);
	}

	//////////////////////////////////////

	// void, handle_some (failure, initially not matched), match_enum (single enum value)
	{
		int c=0;
		leaf::try_(
			[&c]
			{
				leaf::try_(
					[&c]
					{
						c = f<int>(error1());
					},
					[&c]( leaf::catch_<error2> )
					{
						BOOST_TEST(c==0);
						c = 1;
					} );
				BOOST_TEST(false);
			},
			[&c]( leaf::catch_<error1>, info<1> const & x, info<2> y )
			{
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				BOOST_TEST(c==0);
				c = 2;
			},
			[&c]
			{
				BOOST_TEST(c==0);
				c = 3;
			} );
		BOOST_TEST(c==2);
	}

	// void, handle_some (failure, initially not matched), match_enum (multiple enum values)
	{
		int c=0;
		leaf::try_(
			[&c]
			{
				leaf::try_(
					[&c]
					{
						c = f<int>(error1());
					},
					[&c]( leaf::catch_<error2> )
					{
						BOOST_TEST(c==0);
						c = 1;
					} );
				BOOST_TEST(false);
			},
			[&c]( leaf::catch_<error2,error1>, info<1> const & x, info<2> y )
			{
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				BOOST_TEST(c==0);
				c = 2;
			},
			[&c]
			{
				BOOST_TEST(c==0);
				c = 3;
			} );
		BOOST_TEST(c==2);
	}

	// void, handle_some (failure, initially matched), match_enum (single enum value)
	{
		int c=0;
		leaf::try_(
			[&c]
			{
				leaf::try_(
					[&c]
					{
						c = f<int>(error1());
					},
					[&c]( leaf::catch_<error1> ec, info<1> const & x, info<2> y )
					{
						BOOST_TEST(x.value==1);
						BOOST_TEST(y.value==2);
						BOOST_TEST(c==0);
						c = 1;
					} );
			},
			[&c]( leaf::catch_<error2> )
			{
				BOOST_TEST(c==0);
				c = 2;
			},
			[&c]
			{
				BOOST_TEST(c==0);
				c = 3;
			} );
		BOOST_TEST(c==1);
	}

	// void, handle_some (failure, initially matched), match_enum (multiple enum values)
	{
		int c=0;
		leaf::try_(
			[&c]
			{
				leaf::try_(
					[&c]
					{
						c = f<int>(error1());
					},
					[&c]( leaf::catch_<error2,error1>, info<1> const & x, info<2> y )
					{
						BOOST_TEST(x.value==1);
						BOOST_TEST(y.value==2);
						BOOST_TEST(c==0);
						c = 1;
					} );
			},
			[&c]( leaf::catch_<error2> )
			{
				BOOST_TEST(c==0);
				c = 2;
			},
			[&c]
			{
				BOOST_TEST(c==0);
				c = 3;
			} );
		BOOST_TEST(c==1);
	}

	//////////////////////////////////////

	// int, handle_all (success)
	{
		int r = leaf::try_(
			[ ]
			{
				return f<int>();
			},
			[ ]
			{
				return 1;
			} );
		BOOST_TEST(r==42);
	}

	// int, handle_all (failure), match_enum (single enum value)
	{
		int r = leaf::try_(
			[ ]
			{
				return f<int>(error1());
			},
			[ ]( leaf::catch_<error2> )
			{
				return 1;
			},
			[ ]( leaf::catch_<error1>, info<1> const & x, info<2> y )
			{
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST(r==2);
	}

	// int, handle_all (failure), match_enum (multiple enum values)
	{
		int r = leaf::try_(
			[ ]
			{
				return f<int>(error1());
			},
			[ ]( leaf::catch_<error2> )
			{
				return 1;
			},
			[ ]( leaf::catch_<error2,error1>, info<1> const & x, info<2> y )
			{
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST(r==2);
	}

	//////////////////////////////////////

	// int, handle_some (failure, matched), match_enum (single enum value)
	{
		int r = leaf::try_(
			[ ]
			{
				return f<int>(error1());
			},
			[ ]( leaf::catch_<error2> )
			{
				return 1;
			},
			[ ]( leaf::catch_<error1>, info<1> const & x, info<2> y )
			{
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				return 2;
			} );
		BOOST_TEST(r==2);
	}

	// int, handle_some (failure, matched), match_enum (multiple enum values)
	{
		int r = leaf::try_(
			[ ]
			{
				return f<int>(error1());
			},
			[ ]( leaf::catch_<error2> )
			{
				return 1;
			},
			[ ]( leaf::catch_<error2,error1>, info<1> const & x, info<2> y )
			{
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				return 2;
			} );
		BOOST_TEST(r==2);
	}

	// int, handle_some (failure, initially not matched), match_enum (single enum value)
	{
		int r = leaf::try_(
			[ ]
			{
				int r = leaf::try_(
					[ ]
					{
						return f<int>(error1());
					},
					[ ]( leaf::catch_<error2> )
					{
						return 1;
					} );
				BOOST_TEST(false);
				return r;
			},
			[ ]( leaf::catch_<error1>, info<1> const & x, info<2> y )
			{
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST(r==2);
	}

	// int, handle_some (failure, initially not matched), match_enum (multiple enum values)
	{
		int r = leaf::try_(
			[ ]
			{
				int r = leaf::try_(
					[ ]
					{
						return f<int>(error1());
					},
					[ ]( leaf::catch_<error2> )
					{
						return 1;
					} );
				BOOST_TEST(false);
				return r;
			},
			[ ]( leaf::catch_<error2,error1>, info<1> const & x, info<2> y )
			{
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST(r==2);
	}

	// int, handle_some (failure, initially matched), match_enum (single enum value)
	{
		int r = leaf::try_(
			[ ]
			{
				int r = leaf::try_(
					[ ]
					{
						return f<int>(error1());
					},
					[ ]( leaf::catch_<error1>, info<1> const & x, info<2> y )
					{
						BOOST_TEST(x.value==1);
						BOOST_TEST(y.value==2);
						return 1;
					} );
				BOOST_TEST(r==1);
				return r;
			},
			[ ]( leaf::catch_<error1> )
			{
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST(r==1);
	}

	// int, handle_some (failure, initially matched), match_enum (multiple enum values)
	{
		int r = leaf::try_(
			[ ]
			{
				int r = leaf::try_(
					[ ]
					{
						return f<int>(error1());
					},
					[ ]( leaf::catch_<error2,error1>, info<1> const & x, info<2> y )
					{
						BOOST_TEST(x.value==1);
						BOOST_TEST(y.value==2);
						return 1;
					} );
				BOOST_TEST(r==1);
				return r;
			},
			[ ]( leaf::catch_<error1> )
			{
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST(r==1);
	}

	//////////////////////////////////////

	return boost::report_errors();
}
