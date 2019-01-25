// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/handle.hpp>
#include <boost/leaf/result.hpp>
#include "_test_ec.hpp"
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int> struct info { int value; };

enum class not_error_code_enum
{
	ok,
	error1,
	error2,
	error3
};
namespace boost { namespace leaf {
	template <> struct is_error_type<not_error_code_enum>: std::true_type { };
} }

struct e_error_code { not_error_code_enum value; };

template <class R>
leaf::result<R> f( not_error_code_enum ec )
{
	if( ec==not_error_code_enum::ok )
		return R(42);
	else
		return leaf::new_error(ec, e_error_code{ec}, info<1>{1}, info<2>{2}, info<3>{3});
}

template <class R, class Errc>
leaf::result<R> f_errc( Errc ec )
{
	return leaf::new_error(ec, info<1>{1}, info<2>{2}, info<3>{3});
}

int main()
{

	// void, handle_some (success)
	{
		int c=0;
		leaf::result<void> r = leaf::handle_some(
			[&c]() -> leaf::result<void>
			{
				LEAF_AUTO(answer, f<int>(not_error_code_enum::ok));
				c = answer;
				return { };
			},
			[&c]( leaf::error_info const & unmatched )
			{
				BOOST_TEST_EQ(c, 0);
				c = 1;
				return unmatched.error();
			} );
		BOOST_TEST(r);
		BOOST_TEST_EQ(c, 42);
	}

	// void, handle_some (failure, matched)
	{
		int c=0;
		leaf::result<void> r = leaf::handle_some(
			[&c]() -> leaf::result<void>
			{
				LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
				c = answer;
				return { };
			},
			[&c]( not_error_code_enum ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec==not_error_code_enum::error1);
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				BOOST_TEST_EQ(c, 0);
				c = 1;
			} );
		BOOST_TEST_EQ(c, 1);
		BOOST_TEST(r);
	}

	// void, handle_some (failure, matched), match cond_x (single enum value)
	{
		int c=0;
		leaf::result<void> r = leaf::handle_some(
			[&c]() -> leaf::result<void>
			{
				LEAF_AUTO(answer, f_errc<int>(errc_a::a0));
				c = answer;
				return { };
			},
			[&c]( leaf::match<leaf::condition<cond_x>,cond_x::x00> ec, info<1> const & x, info<2> const & y )
			{
				BOOST_TEST_EQ(ec.value(), make_error_code(errc_a::a0));
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				BOOST_TEST_EQ(c, 0);
				c = 1;
			} );
		BOOST_TEST_EQ(c, 1);
		BOOST_TEST(r);
	}

	// void, handle_some (failure, matched), match enum (single enum value)
	{
		int c=0;
		leaf::result<void> r = leaf::handle_some(
			[&c]() -> leaf::result<void>
			{
				LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
				c = answer;
				return { };
			},
			[&c]( leaf::match<not_error_code_enum,not_error_code_enum::error2> )
			{
				BOOST_TEST_EQ(c, 0);
				c = 1;
			},
			[&c]( leaf::match<not_error_code_enum,not_error_code_enum::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value()==not_error_code_enum::error1);
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				BOOST_TEST_EQ(c, 0);
				c = 2;
			} );
		BOOST_TEST_EQ(c, 2);
		BOOST_TEST(r);
	}

	// void, handle_some (failure, matched), match enum (multiple enum values)
	{
		int c=0;
		leaf::result<void> r = leaf::handle_some(
			[&c]() -> leaf::result<void>
			{
				LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
				c = answer;
				return { };
			},
			[&c]( leaf::match<not_error_code_enum,not_error_code_enum::error2> )
			{
				BOOST_TEST_EQ(c, 0);
				c = 1;
			},
			[&c]( leaf::match<not_error_code_enum,not_error_code_enum::error2,not_error_code_enum::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value()==not_error_code_enum::error1);
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				BOOST_TEST_EQ(c, 0);
				c = 2;
			} );
		BOOST_TEST_EQ(c, 2);
		BOOST_TEST(r);
	}

	// void, handle_some (failure, matched), match value (single value)
	{
		int c=0;
		leaf::result<void> r = leaf::handle_some(
			[&c]() -> leaf::result<void>
			{
				LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
				c = answer;
				return { };
			},
			[&c]( leaf::match<e_error_code,not_error_code_enum::error2> )
			{
				BOOST_TEST_EQ(c, 0);
				c = 1;
			},
			[&c]( leaf::match<e_error_code,not_error_code_enum::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value()==not_error_code_enum::error1);
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				BOOST_TEST_EQ(c, 0);
				c = 2;
			} );
		BOOST_TEST_EQ(c, 2);
		BOOST_TEST(r);
	}

	// void, handle_some (failure, matched), match value (multiple values)
	{
		int c=0;
		leaf::result<void> r = leaf::handle_some(
			[&c]() -> leaf::result<void>
			{
				LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
				c = answer;
				return { };
			},
			[&c]( leaf::match<e_error_code,not_error_code_enum::error2> )
			{
				BOOST_TEST_EQ(c, 0);
				c = 1;
			},
			[&c]( leaf::match<e_error_code,not_error_code_enum::error2,not_error_code_enum::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value()==not_error_code_enum::error1);
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				BOOST_TEST_EQ(c, 0);
				c = 2;
			} );
		BOOST_TEST_EQ(c, 2);
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
						LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
						c = answer;
						return { };
					},
					[&c]( info<4> )
					{
						BOOST_TEST_EQ(c, 0);
						c = 1;
					} );
				BOOST_TEST(!r);
				BOOST_TEST_EQ(c, 0);
				return r;
			},
			[&c]( not_error_code_enum ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec==not_error_code_enum::error1);
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				BOOST_TEST_EQ(c, 0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST_EQ(c, 0);
				c = 3;
			} );
		BOOST_TEST_EQ(c, 2);
	}

	// void, handle_some (failure, initially not matched), match cond_x (single enum value)
	{
		int c=0;
		leaf::handle_all(
			[&c]
			{
				leaf::result<void> r = leaf::handle_some(
					[&c]() -> leaf::result<void>
					{
						LEAF_AUTO(answer, f_errc<int>(errc_a::a0));
						c = answer;
						return { };
					},
					[&c]( leaf::match<leaf::condition<cond_x>, cond_x::x11> )
					{
						BOOST_TEST_EQ(c, 0);
						c = 1;
					} );
				BOOST_TEST(!r);
				BOOST_TEST_EQ(c, 0);
				return r;
			},
			[&c]( leaf::match<leaf::condition<cond_x>, cond_x::x00> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST_EQ(ec.value(), make_error_code(errc_a::a0));
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				BOOST_TEST_EQ(c, 0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST_EQ(c, 0);
				c = 3;
			} );
		BOOST_TEST_EQ(c, 2);
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
						LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
						c = answer;
						return { };
					},
					[&c]( leaf::match<not_error_code_enum,not_error_code_enum::error2> )
					{
						BOOST_TEST_EQ(c, 0);
						c = 1;
					} );
				BOOST_TEST(!r);
				BOOST_TEST_EQ(c, 0);
				return r;
			},
			[&c]( leaf::match<not_error_code_enum,not_error_code_enum::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value()==not_error_code_enum::error1);
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				BOOST_TEST_EQ(c, 0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST_EQ(c, 0);
				c = 3;
			} );
		BOOST_TEST_EQ(c, 2);
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
						LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
						c = answer;
						return { };
					},
					[&c]( leaf::match<not_error_code_enum,not_error_code_enum::error2> )
					{
						BOOST_TEST_EQ(c, 0);
						c = 1;
					} );
				BOOST_TEST(!r);
				BOOST_TEST_EQ(c, 0);
				return r;
			},
			[&c]( leaf::match<not_error_code_enum,not_error_code_enum::error2,not_error_code_enum::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value()==not_error_code_enum::error1);
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				BOOST_TEST_EQ(c, 0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST_EQ(c, 0);
				c = 3;
			} );
		BOOST_TEST_EQ(c, 2);
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
						LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
						c = answer;
						return { };
					},
					[&c]( leaf::match<e_error_code,not_error_code_enum::error2> )
					{
						BOOST_TEST_EQ(c, 0);
						c = 1;
					} );
				BOOST_TEST(!r);
				BOOST_TEST_EQ(c, 0);
				return r;
			},
			[&c]( leaf::match<e_error_code,not_error_code_enum::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value()==not_error_code_enum::error1);
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				BOOST_TEST_EQ(c, 0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST_EQ(c, 0);
				c = 3;
			} );
		BOOST_TEST_EQ(c, 2);
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
						LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
						c = answer;
						return { };
					},
					[&c]( leaf::match<e_error_code,not_error_code_enum::error2> )
					{
						BOOST_TEST_EQ(c, 0);
						c = 1;
					} );
				BOOST_TEST(!r);
				BOOST_TEST_EQ(c, 0);
				return r;
			},
			[&c]( leaf::match<e_error_code,not_error_code_enum::error2,not_error_code_enum::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value()==not_error_code_enum::error1);
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				BOOST_TEST_EQ(c, 0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST_EQ(c, 0);
				c = 3;
			} );
		BOOST_TEST_EQ(c, 2);
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
						LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
						c = answer;
						return { };
					},
					[&c]( not_error_code_enum ec, info<1> const & x, info<2> y )
					{
						BOOST_TEST(ec==not_error_code_enum::error1);
						BOOST_TEST_EQ(x.value, 1);
						BOOST_TEST_EQ(y.value, 2);
						BOOST_TEST_EQ(c, 0);
						c = 1;
					} );
				BOOST_TEST(r);
				BOOST_TEST_EQ(c, 1);
				return r;
			},
			[&c]( info<4> )
			{
				BOOST_TEST_EQ(c, 0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST_EQ(c, 0);
				c = 3;
			} );
		BOOST_TEST_EQ(c, 1);
	}

	// void, handle_some (failure, initially matched), match cond_x (single enum value)
	{
		int c=0;
		leaf::handle_all(
			[&c]
			{
				leaf::result<void> r = leaf::handle_some(
					[&c]() -> leaf::result<void>
					{
						LEAF_AUTO(answer, f_errc<int>(errc_a::a0));
						c = answer;
						return { };
					},
					[&c]( leaf::match<leaf::condition<cond_x>, cond_x::x00> ec, info<1> const & x, info<2> y )
					{
						BOOST_TEST_EQ(ec.value(), make_error_code(errc_a::a0));
						BOOST_TEST_EQ(x.value, 1);
						BOOST_TEST_EQ(y.value, 2);
						BOOST_TEST_EQ(c, 0);
						c = 1;
					} );
				BOOST_TEST(r);
				BOOST_TEST_EQ(c, 1);
				return r;
			},
			[&c]( leaf::match<leaf::condition<cond_x>, cond_x::x11> )
			{
				BOOST_TEST_EQ(c, 0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST_EQ(c, 0);
				c = 3;
			} );
		BOOST_TEST_EQ(c, 1);
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
						LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
						c = answer;
						return { };
					},
					[&c]( leaf::match<not_error_code_enum,not_error_code_enum::error1> ec, info<1> const & x, info<2> y )
					{
						BOOST_TEST(ec.value()==not_error_code_enum::error1);
						BOOST_TEST_EQ(x.value, 1);
						BOOST_TEST_EQ(y.value, 2);
						BOOST_TEST_EQ(c, 0);
						c = 1;
					} );
				BOOST_TEST(r);
				BOOST_TEST_EQ(c, 1);
				return r;
			},
			[&c]( leaf::match<not_error_code_enum,not_error_code_enum::error2> )
			{
				BOOST_TEST_EQ(c, 0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST_EQ(c, 0);
				c = 3;
			} );
		BOOST_TEST_EQ(c, 1);
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
						LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
						c = answer;
						return { };
					},
					[&c]( leaf::match<not_error_code_enum,not_error_code_enum::error2,not_error_code_enum::error1> ec, info<1> const & x, info<2> y )
					{
						BOOST_TEST(ec.value()==not_error_code_enum::error1);
						BOOST_TEST_EQ(x.value, 1);
						BOOST_TEST_EQ(y.value, 2);
						BOOST_TEST_EQ(c, 0);
						c = 1;
					} );
				BOOST_TEST(r);
				BOOST_TEST_EQ(c, 1);
				return r;
			},
			[&c]( leaf::match<not_error_code_enum,not_error_code_enum::error2> )
			{
				BOOST_TEST_EQ(c, 0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST_EQ(c, 0);
				c = 3;
			} );
		BOOST_TEST_EQ(c, 1);
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
						LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
						c = answer;
						return { };
					},
					[&c]( leaf::match<e_error_code,not_error_code_enum::error1> ec, info<1> const & x, info<2> y )
					{
						BOOST_TEST(ec.value()==not_error_code_enum::error1);
						BOOST_TEST_EQ(x.value, 1);
						BOOST_TEST_EQ(y.value, 2);
						BOOST_TEST_EQ(c, 0);
						c = 1;
					} );
				BOOST_TEST(r);
				BOOST_TEST_EQ(c, 1);
				return r;
			},
			[&c]( leaf::match<e_error_code,not_error_code_enum::error2> )
			{
				BOOST_TEST_EQ(c, 0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST_EQ(c, 0);
				c = 3;
			} );
		BOOST_TEST_EQ(c, 1);
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
						LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
						c = answer;
						return { };
					},
					[&c]( leaf::match<e_error_code,not_error_code_enum::error2,not_error_code_enum::error1> ec, info<1> const & x, info<2> y )
					{
						BOOST_TEST(ec.value()==not_error_code_enum::error1);
						BOOST_TEST_EQ(x.value, 1);
						BOOST_TEST_EQ(y.value, 2);
						BOOST_TEST_EQ(c, 0);
						c = 1;
					} );
				BOOST_TEST(r);
				BOOST_TEST_EQ(c, 1);
				return r;
			},
			[&c]( leaf::match<e_error_code,not_error_code_enum::error2> )
			{
				BOOST_TEST_EQ(c, 0);
				c = 2;
			},
			[&c]()
			{
				BOOST_TEST_EQ(c, 0);
				c = 3;
			} );
		BOOST_TEST_EQ(c, 1);
	}

	//////////////////////////////////////

	// int, handle_some (success)
	{
		leaf::result<int> r = leaf::handle_some(
			[ ]() -> leaf::result<int>
			{
				LEAF_AUTO(answer, f<int>(not_error_code_enum::ok));
				return answer;
			},
			[ ]( leaf::error_info const & unmatched )
			{
				return unmatched.error();
			} );
		BOOST_TEST_EQ(*r, 42);
	}

	// int, handle_some (failure, matched)
	{
		leaf::result<int> r = leaf::handle_some(
			[ ]() -> leaf::result<int>
			{
				LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
				return answer;
			},
			[ ]( not_error_code_enum ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec==not_error_code_enum::error1);
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				return 1;
			} );
		BOOST_TEST(r);
		BOOST_TEST_EQ(*r, 1);
	}

	// int, handle_some (failure, matched), match cond_x (single enum value)
	{
		leaf::result<int> r = leaf::handle_some(
			[ ]() -> leaf::result<int>
			{
				LEAF_AUTO(answer, f_errc<int>(errc_a::a0));
				return answer;
			},
			[ ]( leaf::match<leaf::condition<cond_x>, cond_x::x11> )
			{
				return 1;
			},
			[ ]( leaf::match<leaf::condition<cond_x>, cond_x::x00> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST_EQ(ec.value(), make_error_code(errc_a::a0));
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				return 2;
			} );
		BOOST_TEST(r);
		BOOST_TEST_EQ(*r, 2);
	}

	// int, handle_some (failure, matched), match enum (single enum value)
	{
		leaf::result<int> r = leaf::handle_some(
			[ ]() -> leaf::result<int>
			{
				LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
				return answer;
			},
			[ ]( leaf::match<not_error_code_enum,not_error_code_enum::error2> )
			{
				return 1;
			},
			[ ]( leaf::match<not_error_code_enum,not_error_code_enum::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value()==not_error_code_enum::error1);
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				return 2;
			} );
		BOOST_TEST(r);
		BOOST_TEST_EQ(*r, 2);
	}

	// int, handle_some (failure, matched), match enum (multiple enum values)
	{
		leaf::result<int> r = leaf::handle_some(
			[ ]() -> leaf::result<int>
			{
				LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
				return answer;
			},
			[ ]( leaf::match<not_error_code_enum,not_error_code_enum::error2> )
			{
				return 1;
			},
			[ ]( leaf::match<not_error_code_enum,not_error_code_enum::error2,not_error_code_enum::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value()==not_error_code_enum::error1);
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				return 2;
			} );
		BOOST_TEST(r);
		BOOST_TEST_EQ(*r, 2);
	}

	// int, handle_some (failure, matched), match value (single value)
	{
		leaf::result<int> r = leaf::handle_some(
			[ ]() -> leaf::result<int>
			{
				LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
				return answer;
			},
			[ ]( leaf::match<e_error_code,not_error_code_enum::error2> )
			{
				return 1;
			},
			[ ]( leaf::match<e_error_code,not_error_code_enum::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value()==not_error_code_enum::error1);
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				return 2;
			} );
		BOOST_TEST(r);
		BOOST_TEST_EQ(*r, 2);
	}

	// int, handle_some (failure, matched), match value (multiple values)
	{
		leaf::result<int> r = leaf::handle_some(
			[ ]() -> leaf::result<int>
			{
				LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
				return answer;
			},
			[ ]( leaf::match<e_error_code,not_error_code_enum::error2> )
			{
				return 1;
			},
			[ ]( leaf::match<e_error_code,not_error_code_enum::error2,not_error_code_enum::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value()==not_error_code_enum::error1);
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				return 2;
			} );
		BOOST_TEST(r);
		BOOST_TEST_EQ(*r, 2);
	}

	// int, handle_some (failure, initially not matched)
	{
		int r = leaf::handle_all(
			[ ]
			{
				leaf::result<int> r = leaf::handle_some(
					[ ]() -> leaf::result<int>
					{
						LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
						return answer;
					},
					[ ]( info<4> )
					{
						return 1;
					} );
				BOOST_TEST(!r);
				return r;
			},
			[ ]( not_error_code_enum ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec==not_error_code_enum::error1);
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST_EQ(r, 2);
	}

	// int, handle_some (failure, initially not matched), match cond_x (single enum value)
	{
		int r = leaf::handle_all(
			[ ]
			{
				leaf::result<int> r = leaf::handle_some(
					[ ]() -> leaf::result<int>
					{
						LEAF_AUTO(answer, f_errc<int>(errc_a::a0));
						return answer;
					},
					[ ]( leaf::match<leaf::condition<cond_x>, cond_x::x11> )
					{
						return 1;
					} );
				BOOST_TEST(!r);
				return r;
			},
			[ ]( leaf::match<leaf::condition<cond_x>, cond_x::x00> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST_EQ(ec.value(), make_error_code(errc_a::a0));
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST_EQ(r, 2);
	}

	// int, handle_some (failure, initially not matched), match enum (single enum value)
	{
		int r = leaf::handle_all(
			[ ]
			{
				leaf::result<int> r = leaf::handle_some(
					[ ]() -> leaf::result<int>
					{
						LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
						return answer;
					},
					[ ]( leaf::match<not_error_code_enum,not_error_code_enum::error2> )
					{
						return 1;
					} );
				BOOST_TEST(!r);
				return r;
			},
			[ ]( leaf::match<not_error_code_enum,not_error_code_enum::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value()==not_error_code_enum::error1);
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST_EQ(r, 2);
	}

	// int, handle_some (failure, initially not matched), match enum (multiple enum values)
	{
		int r = leaf::handle_all(
			[ ]
			{
				leaf::result<int> r = leaf::handle_some(
					[ ]() -> leaf::result<int>
					{
						LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
						return answer;
					},
					[ ]( leaf::match<not_error_code_enum,not_error_code_enum::error2> )
					{
						return 1;
					} );
				BOOST_TEST(!r);
				return r;
			},
			[ ]( leaf::match<not_error_code_enum,not_error_code_enum::error2,not_error_code_enum::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value()==not_error_code_enum::error1);
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST_EQ(r, 2);
	}

	// int, handle_some (failure, initially not matched), match value (single value)
	{
		int r = leaf::handle_all(
			[ ]
			{
				leaf::result<int> r = leaf::handle_some(
					[ ]() -> leaf::result<int>
					{
						LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
						return answer;
					},
					[ ]( leaf::match<e_error_code,not_error_code_enum::error2> )
					{
						return 1;
					} );
				BOOST_TEST(!r);
				return r;
			},
			[ ]( leaf::match<e_error_code,not_error_code_enum::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value()==not_error_code_enum::error1);
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST_EQ(r, 2);
	}

	// int, handle_some (failure, initially not matched), match value (multiple values)
	{
		int r = leaf::handle_all(
			[ ]
			{
				leaf::result<int> r = leaf::handle_some(
					[ ]() -> leaf::result<int>
					{
						LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
						return answer;
					},
					[ ]( leaf::match<e_error_code,not_error_code_enum::error2> )
					{
						return 1;
					} );
				BOOST_TEST(!r);
				return r;
			},
			[ ]( leaf::match<e_error_code,not_error_code_enum::error2,not_error_code_enum::error1> ec, info<1> const & x, info<2> y )
			{
				BOOST_TEST(ec.value()==not_error_code_enum::error1);
				BOOST_TEST_EQ(x.value, 1);
				BOOST_TEST_EQ(y.value, 2);
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST_EQ(r, 2);
	}

	// int, handle_some (failure, initially matched)
	{
		int r = leaf::handle_all(
			[ ]
			{
				leaf::result<int> r = leaf::handle_some(
					[ ]() -> leaf::result<int>
					{
						LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
						return answer;
					},
					[ ]( not_error_code_enum ec, info<1> const & x, info<2> y )
					{
						BOOST_TEST(ec==not_error_code_enum::error1);
						BOOST_TEST_EQ(x.value, 1);
						BOOST_TEST_EQ(y.value, 2);
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
		BOOST_TEST_EQ(r, 1);
	}

	// int, handle_some (failure, initially matched), match enum (single enum value)
	{
		int r = leaf::handle_all(
			[ ]
			{
				leaf::result<int> r = leaf::handle_some(
					[ ]() -> leaf::result<int>
					{
						LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
						return answer;
					},
					[ ]( leaf::match<not_error_code_enum,not_error_code_enum::error1> ec, info<1> const & x, info<2> y )
					{
						BOOST_TEST(ec.value()==not_error_code_enum::error1);
						BOOST_TEST_EQ(x.value, 1);
						BOOST_TEST_EQ(y.value, 2);
						return 1;
					} );
				BOOST_TEST(r);
				return r;
			},
			[ ]( leaf::match<not_error_code_enum,not_error_code_enum::error2> )
			{
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST_EQ(r, 1);
	}

	// int, handle_some (failure, initially matched), match enum (multiple enum values)
	{
		int r = leaf::handle_all(
			[ ]
			{
				leaf::result<int> r = leaf::handle_some(
					[ ]() -> leaf::result<int>
					{
						LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
						return answer;
					},
					[ ]( leaf::match<not_error_code_enum,not_error_code_enum::error2,not_error_code_enum::error1> ec, info<1> const & x, info<2> y )
					{
						BOOST_TEST(ec.value()==not_error_code_enum::error1);
						BOOST_TEST_EQ(x.value, 1);
						BOOST_TEST_EQ(y.value, 2);
						return 1;
					} );
				BOOST_TEST(r);
				return r;
			},
			[ ]( leaf::match<not_error_code_enum,not_error_code_enum::error2> )
			{
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST_EQ(r, 1);
	}

	// int, handle_some (failure, initially matched), match value (single value)
	{
		int r = leaf::handle_all(
			[ ]
			{
				leaf::result<int> r = leaf::handle_some(
					[ ]() -> leaf::result<int>
					{
						LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
						return answer;
					},
					[ ]( leaf::match<e_error_code,not_error_code_enum::error1> ec, info<1> const & x, info<2> y )
					{
						BOOST_TEST(ec.value()==not_error_code_enum::error1);
						BOOST_TEST_EQ(x.value, 1);
						BOOST_TEST_EQ(y.value, 2);
						return 1;
					} );
				BOOST_TEST(r);
				return r;
			},
			[ ]( leaf::match<e_error_code,not_error_code_enum::error2> )
			{
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST_EQ(r, 1);
	}

	// int, handle_some (failure, initially matched), match value (multiple values)
	{
		int r = leaf::handle_all(
			[ ]
			{
				leaf::result<int> r = leaf::handle_some(
					[ ]() -> leaf::result<int>
					{
						LEAF_AUTO(answer, f<int>(not_error_code_enum::error1));
						return answer;
					},
					[ ]( leaf::match<e_error_code,not_error_code_enum::error2,not_error_code_enum::error1> ec, info<1> const & x, info<2> y )
					{
						BOOST_TEST(ec.value()==not_error_code_enum::error1);
						BOOST_TEST_EQ(x.value, 1);
						BOOST_TEST_EQ(y.value, 2);
						return 1;
					} );
				BOOST_TEST(r);
				return r;
			},
			[ ]( leaf::match<e_error_code,not_error_code_enum::error2> )
			{
				return 2;
			},
			[ ]
			{
				return 3;
			} );
		BOOST_TEST_EQ(r, 1);
	}

	return boost::report_errors();
}
