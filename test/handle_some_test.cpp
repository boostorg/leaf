// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/handle.hpp>
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int> struct info { int value; };

enum class error_code
{
	ok,
	error1,
	error2,
	error3
};
namespace boost { namespace leaf {
	template <> struct is_e_type<error_code>: std::true_type { };
} }

struct error_codes_ { error_code value; };

template <class R>
leaf::result<R> f( error_code ec )
{
	if( ec==error_code::ok )
		return R(42);
	else
		return leaf::new_error(ec,error_codes_{ec},info<1>{1},info<2>{2},info<3>{3});
}

int main()
{

	// void, handle_some (success)
	{
		int c=0;
		leaf::result<void> r = leaf::handle_some(
			[&c]() -> leaf::result<void>
			{
				LEAF_AUTO(answer, f<int>(error_code::ok));
				c = answer;
				return { };
			},
			[&c]( leaf::error_info const & unmatched )
			{
				BOOST_TEST(c==0);
				c = 1;
				return unmatched.error();
			} );
		BOOST_TEST(r);
		BOOST_TEST(c==42);
	}

	// void, handle_some (failure, matched)
	{
		int c=0;
		leaf::result<void> r = leaf::handle_some(
			[&c]() -> leaf::result<void>
			{
				LEAF_AUTO(answer, f<int>(error_code::error1));
				c = answer;
				return { };
			},
			[&c]( error_code ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec==error_code::error1);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				BOOST_TEST(c==0);
				c = 1;
			} );
		BOOST_TEST(c==1);
		BOOST_TEST(r);
	}

	// void, handle_some (failure, matched), match enum (single enum value)
	{
		int c=0;
		leaf::result<void> r = leaf::handle_some(
			[&c]() -> leaf::result<void>
			{
				LEAF_AUTO(answer, f<int>(error_code::error1));
				c = answer;
				return { };
			},
			[&c]( leaf::match<error_code,error_code::error2> )
			{
				BOOST_TEST(c==0);
				c = 1;
			},
			[&c]( leaf::match<error_code,error_code::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value==error_code::error1);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				BOOST_TEST(c==0);
				c = 2;
			} );
		BOOST_TEST(c==2);
		BOOST_TEST(r);
	}

	// void, handle_some (failure, matched), match enum (multiple enum values)
	{
		int c=0;
		leaf::result<void> r = leaf::handle_some(
			[&c]() -> leaf::result<void>
			{
				LEAF_AUTO(answer, f<int>(error_code::error1));
				c = answer;
				return { };
			},
			[&c]( leaf::match<error_code,error_code::error2> )
			{
				BOOST_TEST(c==0);
				c = 1;
			},
			[&c]( leaf::match<error_code,error_code::error2,error_code::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value==error_code::error1);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				BOOST_TEST(c==0);
				c = 2;
			} );
		BOOST_TEST(c==2);
		BOOST_TEST(r);
	}

	// void, handle_some (failure, matched), match value (single value)
	{
		int c=0;
		leaf::result<void> r = leaf::handle_some(
			[&c]() -> leaf::result<void>
			{
				LEAF_AUTO(answer, f<int>(error_code::error1));
				c = answer;
				return { };
			},
			[&c]( leaf::match<error_codes_,error_code::error2> )
			{
				BOOST_TEST(c==0);
				c = 1;
			},
			[&c]( leaf::match<error_codes_,error_code::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value==error_code::error1);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				BOOST_TEST(c==0);
				c = 2;
			} );
		BOOST_TEST(c==2);
		BOOST_TEST(r);
	}

	// void, handle_some (failure, matched), match value (multiple values)
	{
		int c=0;
		leaf::result<void> r = leaf::handle_some(
			[&c]() -> leaf::result<void>
			{
				LEAF_AUTO(answer, f<int>(error_code::error1));
				c = answer;
				return { };
			},
			[&c]( leaf::match<error_codes_,error_code::error2> )
			{
				BOOST_TEST(c==0);
				c = 1;
			},
			[&c]( leaf::match<error_codes_,error_code::error2,error_code::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value==error_code::error1);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				BOOST_TEST(c==0);
				c = 2;
			} );
		BOOST_TEST(c==2);
		BOOST_TEST(r);
	}

	// void, handle_some (failure, initially not matched)
	{
		int c=0;
		leaf::handle_all(
			[&c]
			{
				leaf::result<void> r = leaf::handle_some(
					[&c]() -> leaf::result<void>
					{
						LEAF_AUTO(answer, f<int>(error_code::error1));
						c = answer;
						return { };
					},
					[&c]( info<4> )
					{
						BOOST_TEST(c==0);
						c = 1;
					} );
				BOOST_TEST(!r);
				BOOST_TEST(c==0);
				return r;
			},
			[&c]( error_code ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec==error_code::error1);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				BOOST_TEST(c==0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST(c==0);
				c = 3;
			} );
		BOOST_TEST(c==2);
	}

	// void, handle_some (failure, initially not matched), match enum (single enum value)
	{
		int c=0;
		leaf::handle_all(
			[&c]
			{
				leaf::result<void> r = leaf::handle_some(
					[&c]() -> leaf::result<void>
					{
						LEAF_AUTO(answer, f<int>(error_code::error1));
						c = answer;
						return { };
					},
					[&c]( leaf::match<error_code,error_code::error2> )
					{
						BOOST_TEST(c==0);
						c = 1;
					} );
				BOOST_TEST(!r);
				BOOST_TEST(c==0);
				return r;
			},
			[&c]( leaf::match<error_code,error_code::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value==error_code::error1);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				BOOST_TEST(c==0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST(c==0);
				c = 3;
			} );
		BOOST_TEST(c==2);
	}

	// void, handle_some (failure, initially not matched), match enum (multiple enum values)
	{
		int c=0;
		leaf::handle_all(
			[&c]
			{
				leaf::result<void> r = leaf::handle_some(
					[&c]() -> leaf::result<void>
					{
						LEAF_AUTO(answer, f<int>(error_code::error1));
						c = answer;
						return { };
					},
					[&c]( leaf::match<error_code,error_code::error2> )
					{
						BOOST_TEST(c==0);
						c = 1;
					} );
				BOOST_TEST(!r);
				BOOST_TEST(c==0);
				return r;
			},
			[&c]( leaf::match<error_code,error_code::error2,error_code::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value==error_code::error1);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				BOOST_TEST(c==0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST(c==0);
				c = 3;
			} );
		BOOST_TEST(c==2);
	}

	// void, handle_some (failure, initially not matched), match value (single value)
	{
		int c=0;
		leaf::handle_all(
			[&c]
			{
				leaf::result<void> r = leaf::handle_some(
					[&c]() -> leaf::result<void>
					{
						LEAF_AUTO(answer, f<int>(error_code::error1));
						c = answer;
						return { };
					},
					[&c]( leaf::match<error_codes_,error_code::error2> )
					{
						BOOST_TEST(c==0);
						c = 1;
					} );
				BOOST_TEST(!r);
				BOOST_TEST(c==0);
				return r;
			},
			[&c]( leaf::match<error_codes_,error_code::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value==error_code::error1);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				BOOST_TEST(c==0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST(c==0);
				c = 3;
			} );
		BOOST_TEST(c==2);
	}

	// void, handle_some (failure, initially not matched), match value (multiple values)
	{
		int c=0;
		leaf::handle_all(
			[&c]
			{
				leaf::result<void> r = leaf::handle_some(
					[&c]() -> leaf::result<void>
					{
						LEAF_AUTO(answer, f<int>(error_code::error1));
						c = answer;
						return { };
					},
					[&c]( leaf::match<error_codes_,error_code::error2> )
					{
						BOOST_TEST(c==0);
						c = 1;
					} );
				BOOST_TEST(!r);
				BOOST_TEST(c==0);
				return r;
			},
			[&c]( leaf::match<error_codes_,error_code::error2,error_code::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value==error_code::error1);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				BOOST_TEST(c==0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST(c==0);
				c = 3;
			} );
		BOOST_TEST(c==2);
	}

	// void, handle_some (failure, initially matched)
	{
		int c=0;
		leaf::handle_all(
			[&c]
			{
				leaf::result<void> r = leaf::handle_some(
					[&c]() -> leaf::result<void>
					{
						LEAF_AUTO(answer, f<int>(error_code::error1));
						c = answer;
						return { };
					},
					[&c]( error_code ec, info<1> const & x, info<2> y )
					{
						BOOST_TEST(ec==error_code::error1);
						BOOST_TEST(x.value==1);
						BOOST_TEST(y.value==2);
						BOOST_TEST(c==0);
						c = 1;
					} );
				BOOST_TEST(r);
				BOOST_TEST(c==1);
				return r;
			},
			[&c]( info<4> )
			{
				BOOST_TEST(c==0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST(c==0);
				c = 3;
			} );
		BOOST_TEST(c==1);
	}

	// void, handle_some (failure, initially matched), match enum (single enum value)
	{
		int c=0;
		leaf::handle_all(
			[&c]
			{
				leaf::result<void> r = leaf::handle_some(
					[&c]() -> leaf::result<void>
					{
						LEAF_AUTO(answer, f<int>(error_code::error1));
						c = answer;
						return { };
					},
					[&c]( leaf::match<error_code,error_code::error1> ec, info<1> const & x, info<2> y )
					{
						BOOST_TEST(ec.value==error_code::error1);
						BOOST_TEST(x.value==1);
						BOOST_TEST(y.value==2);
						BOOST_TEST(c==0);
						c = 1;
					} );
				BOOST_TEST(r);
				BOOST_TEST(c==1);
				return r;
			},
			[&c]( leaf::match<error_code,error_code::error2> )
			{
				BOOST_TEST(c==0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST(c==0);
				c = 3;
			} );
		BOOST_TEST(c==1);
	}

	// void, handle_some (failure, initially matched), match enum (multiple enum values)
	{
		int c=0;
		leaf::handle_all(
			[&c]
			{
				leaf::result<void> r = leaf::handle_some(
					[&c]() -> leaf::result<void>
					{
						LEAF_AUTO(answer, f<int>(error_code::error1));
						c = answer;
						return { };
					},
					[&c]( leaf::match<error_code,error_code::error2,error_code::error1> ec, info<1> const & x, info<2> y )
					{
						BOOST_TEST(ec.value==error_code::error1);
						BOOST_TEST(x.value==1);
						BOOST_TEST(y.value==2);
						BOOST_TEST(c==0);
						c = 1;
					} );
				BOOST_TEST(r);
				BOOST_TEST(c==1);
				return r;
			},
			[&c]( leaf::match<error_code,error_code::error2> )
			{
				BOOST_TEST(c==0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST(c==0);
				c = 3;
			} );
		BOOST_TEST(c==1);
	}

	// void, handle_some (failure, initially matched), match value (single value)
	{
		int c=0;
		leaf::handle_all(
			[&c]
			{
				leaf::result<void> r = leaf::handle_some(
					[&c]() -> leaf::result<void>
					{
						LEAF_AUTO(answer, f<int>(error_code::error1));
						c = answer;
						return { };
					},
					[&c]( leaf::match<error_codes_,error_code::error1> ec, info<1> const & x, info<2> y )
					{
						BOOST_TEST(ec.value==error_code::error1);
						BOOST_TEST(x.value==1);
						BOOST_TEST(y.value==2);
						BOOST_TEST(c==0);
						c = 1;
					} );
				BOOST_TEST(r);
				BOOST_TEST(c==1);
				return r;
			},
			[&c]( leaf::match<error_codes_,error_code::error2> )
			{
				BOOST_TEST(c==0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST(c==0);
				c = 3;
			} );
		BOOST_TEST(c==1);
	}

	// void, handle_some (failure, initially matched), match value (multiple values)
	{
		int c=0;
		leaf::handle_all(
			[&c]
			{
				leaf::result<void> r = leaf::handle_some(
					[&c]() -> leaf::result<void>
					{
						LEAF_AUTO(answer, f<int>(error_code::error1));
						c = answer;
						return { };
					},
					[&c]( leaf::match<error_codes_,error_code::error2,error_code::error1> ec, info<1> const & x, info<2> y )
					{
						BOOST_TEST(ec.value==error_code::error1);
						BOOST_TEST(x.value==1);
						BOOST_TEST(y.value==2);
						BOOST_TEST(c==0);
						c = 1;
					} );
				BOOST_TEST(r);
				BOOST_TEST(c==1);
				return r;
			},
			[&c]( leaf::match<error_codes_,error_code::error2> )
			{
				BOOST_TEST(c==0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST(c==0);
				c = 3;
			} );
		BOOST_TEST(c==1);
	}

	//////////////////////////////////////

	// int, handle_some (success)
	{
		leaf::result<int> r = leaf::handle_some(
			[ ]() -> leaf::result<int>
			{
				LEAF_AUTO(answer, f<int>(error_code::ok));
				return answer;
			},
			[ ]( leaf::error_info const & unmatched )
			{
				return unmatched.error();
			} );
		BOOST_TEST(r && *r==42);
	}

	// int, handle_some (failure, matched)
	{
		leaf::result<int> r = leaf::handle_some(
			[ ]() -> leaf::result<int>
			{
				LEAF_AUTO(answer, f<int>(error_code::error1));
				return answer;
			},
			[ ]( error_code ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec==error_code::error1);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				return 1;
			} );
		BOOST_TEST(r && *r==1);
	}

	// int, handle_some (failure, matched), match enum (single enum value)
	{
		leaf::result<int> r = leaf::handle_some(
			[ ]() -> leaf::result<int>
			{
				LEAF_AUTO(answer, f<int>(error_code::error1));
				return answer;
			},
			[ ]( leaf::match<error_code,error_code::error2> )
			{
				return 1;
			},
			[ ]( leaf::match<error_code,error_code::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value==error_code::error1);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				return 2;
			} );
		BOOST_TEST(r && *r==2);
	}

	// int, handle_some (failure, matched), match enum (multiple enum values)
	{
		leaf::result<int> r = leaf::handle_some(
			[ ]() -> leaf::result<int>
			{
				LEAF_AUTO(answer, f<int>(error_code::error1));
				return answer;
			},
			[ ]( leaf::match<error_code,error_code::error2> )
			{
				return 1;
			},
			[ ]( leaf::match<error_code,error_code::error2,error_code::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value==error_code::error1);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				return 2;
			} );
		BOOST_TEST(r && *r==2);
	}

	// int, handle_some (failure, matched), match value (single value)
	{
		leaf::result<int> r = leaf::handle_some(
			[ ]() -> leaf::result<int>
			{
				LEAF_AUTO(answer, f<int>(error_code::error1));
				return answer;
			},
			[ ]( leaf::match<error_codes_,error_code::error2> )
			{
				return 1;
			},
			[ ]( leaf::match<error_codes_,error_code::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value==error_code::error1);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				return 2;
			} );
		BOOST_TEST(r && *r==2);
	}

	// int, handle_some (failure, matched), match value (multiple values)
	{
		leaf::result<int> r = leaf::handle_some(
			[ ]() -> leaf::result<int>
			{
				LEAF_AUTO(answer, f<int>(error_code::error1));
				return answer;
			},
			[ ]( leaf::match<error_codes_,error_code::error2> )
			{
				return 1;
			},
			[ ]( leaf::match<error_codes_,error_code::error2,error_code::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value==error_code::error1);
				BOOST_TEST(x.value==1);
				BOOST_TEST(y.value==2);
				return 2;
			} );
		BOOST_TEST(r && *r==2);
	}

	// int, handle_some (failure, initially not matched)
	{
		int r = leaf::handle_all(
			[ ]
			{
				leaf::result<int> r = leaf::handle_some(
					[ ]() -> leaf::result<int>
					{
						LEAF_AUTO(answer, f<int>(error_code::error1));
						return answer;
					},
					[ ]( info<4> )
					{
						return 1;
					} );
				BOOST_TEST(!r);
				return r;
			},
			[ ]( error_code ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec==error_code::error1);
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

	// int, handle_some (failure, initially not matched), match enum (single enum value)
	{
		int r = leaf::handle_all(
			[ ]
			{
				leaf::result<int> r = leaf::handle_some(
					[ ]() -> leaf::result<int>
					{
						LEAF_AUTO(answer, f<int>(error_code::error1));
						return answer;
					},
					[ ]( leaf::match<error_code,error_code::error2> )
					{
						return 1;
					} );
				BOOST_TEST(!r);
				return r;
			},
			[ ]( leaf::match<error_code,error_code::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value==error_code::error1);
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

	// int, handle_some (failure, initially not matched), match enum (multiple enum values)
	{
		int r = leaf::handle_all(
			[ ]
			{
				leaf::result<int> r = leaf::handle_some(
					[ ]() -> leaf::result<int>
					{
						LEAF_AUTO(answer, f<int>(error_code::error1));
						return answer;
					},
					[ ]( leaf::match<error_code,error_code::error2> )
					{
						return 1;
					} );
				BOOST_TEST(!r);
				return r;
			},
			[ ]( leaf::match<error_code,error_code::error2,error_code::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value==error_code::error1);
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

	// int, handle_some (failure, initially not matched), match value (single value)
	{
		int r = leaf::handle_all(
			[ ]
			{
				leaf::result<int> r = leaf::handle_some(
					[ ]() -> leaf::result<int>
					{
						LEAF_AUTO(answer, f<int>(error_code::error1));
						return answer;
					},
					[ ]( leaf::match<error_codes_,error_code::error2> )
					{
						return 1;
					} );
				BOOST_TEST(!r);
				return r;
			},
			[ ]( leaf::match<error_codes_,error_code::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value==error_code::error1);
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

	// int, handle_some (failure, initially not matched), match value (multiple values)
	{
		int r = leaf::handle_all(
			[ ]
			{
				leaf::result<int> r = leaf::handle_some(
					[ ]() -> leaf::result<int>
					{
						LEAF_AUTO(answer, f<int>(error_code::error1));
						return answer;
					},
					[ ]( leaf::match<error_codes_,error_code::error2> )
					{
						return 1;
					} );
				BOOST_TEST(!r);
				return r;
			},
			[ ]( leaf::match<error_codes_,error_code::error2,error_code::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value==error_code::error1);
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

	// int, handle_some (failure, initially matched)
	{
		int r = leaf::handle_all(
			[ ]
			{
				leaf::result<int> r = leaf::handle_some(
					[ ]() -> leaf::result<int>
					{
						LEAF_AUTO(answer, f<int>(error_code::error1));
						return answer;
					},
					[ ]( error_code ec, info<1> const & x, info<2> y )
					{
						BOOST_TEST(ec==error_code::error1);
						BOOST_TEST(x.value==1);
						BOOST_TEST(y.value==2);
						return 1;
					} );
				BOOST_TEST(r);
				return r;
			},
			[ ]( info<4> )
			{
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST(r==1);
	}

	// int, handle_some (failure, initially matched), match enum (single enum value)
	{
		int r = leaf::handle_all(
			[ ]
			{
				leaf::result<int> r = leaf::handle_some(
					[ ]() -> leaf::result<int>
					{
						LEAF_AUTO(answer, f<int>(error_code::error1));
						return answer;
					},
					[ ]( leaf::match<error_code,error_code::error1> ec, info<1> const & x, info<2> y )
					{
						BOOST_TEST(ec.value==error_code::error1);
						BOOST_TEST(x.value==1);
						BOOST_TEST(y.value==2);
						return 1;
					} );
				BOOST_TEST(r);
				return r;
			},
			[ ]( leaf::match<error_code,error_code::error2> )
			{
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST(r==1);
	}

	// int, handle_some (failure, initially matched), match enum (multiple enum values)
	{
		int r = leaf::handle_all(
			[ ]
			{
				leaf::result<int> r = leaf::handle_some(
					[ ]() -> leaf::result<int>
					{
						LEAF_AUTO(answer, f<int>(error_code::error1));
						return answer;
					},
					[ ]( leaf::match<error_code,error_code::error2,error_code::error1> ec, info<1> const & x, info<2> y )
					{
						BOOST_TEST(ec.value==error_code::error1);
						BOOST_TEST(x.value==1);
						BOOST_TEST(y.value==2);
						return 1;
					} );
				BOOST_TEST(r);
				return r;
			},
			[ ]( leaf::match<error_code,error_code::error2> )
			{
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST(r==1);
	}

	// int, handle_some (failure, initially matched), match value (single value)
	{
		int r = leaf::handle_all(
			[ ]
			{
				leaf::result<int> r = leaf::handle_some(
					[ ]() -> leaf::result<int>
					{
						LEAF_AUTO(answer, f<int>(error_code::error1));
						return answer;
					},
					[ ]( leaf::match<error_codes_,error_code::error1> ec, info<1> const & x, info<2> y )
					{
						BOOST_TEST(ec.value==error_code::error1);
						BOOST_TEST(x.value==1);
						BOOST_TEST(y.value==2);
						return 1;
					} );
				BOOST_TEST(r);
				return r;
			},
			[ ]( leaf::match<error_codes_,error_code::error2> )
			{
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST(r==1);
	}

	// int, handle_some (failure, initially matched), match value (multiple values)
	{
		int r = leaf::handle_all(
			[ ]
			{
				leaf::result<int> r = leaf::handle_some(
					[ ]() -> leaf::result<int>
					{
						LEAF_AUTO(answer, f<int>(error_code::error1));
						return answer;
					},
					[ ]( leaf::match<error_codes_,error_code::error2,error_code::error1> ec, info<1> const & x, info<2> y )
					{
						BOOST_TEST(ec.value==error_code::error1);
						BOOST_TEST(x.value==1);
						BOOST_TEST(y.value==2);
						return 1;
					} );
				BOOST_TEST(r);
				return r;
			},
			[ ]( leaf::match<error_codes_,error_code::error2> )
			{
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST(r==1);
	}

	return boost::report_errors();
}
