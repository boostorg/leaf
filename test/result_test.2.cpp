//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/result.hpp>
#include <boost/leaf/expect.hpp>
#include <boost/leaf/error_capture.hpp>
#include <boost/detail/lightweight_test.hpp>

namespace leaf = boost::leaf;

struct val
{
	static int count;

	val()
	{
		++count;
	}

	val( val const & )
	{
		++count;
	}

	val( val && )
	{
		++count;
	}

	~val()
	{
		--count;
	}
};
int val::count = 0;

struct err
{
	static int count;

	err()
	{
		++count;
	}

	err( err const & )
	{
		++count;
	}

	err( err && )
	{
		++count;
	}

	~err()
	{
		--count;
	}
};
int err::count = 0;
struct e_err { err value; };

int main()
{
	{ //value default-copy
		leaf::expect<e_err> exp;
		leaf::result<val> r1;
		BOOST_TEST(r1);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==1);
		leaf::result<val> r2 = r1;
		BOOST_TEST(r2);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==2);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);
	{ //value move-copy
		leaf::expect<e_err> exp;
		leaf::result<val> r1 = val();
		BOOST_TEST(r1);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==1);
		leaf::result<val> r2 = r1;
		BOOST_TEST(r2);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==2);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);
	{ //value copy-copy
		leaf::expect<e_err> exp;
		val v;
		leaf::result<val> r1 = v;
		BOOST_TEST(r1);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==2);
		leaf::result<val> r2 = r1;
		BOOST_TEST(r2);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==3);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);

	{ //value default-move
		leaf::expect<e_err> exp;
		leaf::result<val> r1;
		BOOST_TEST(r1);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==1);
		leaf::result<val> r2 = std::move(r1);
		BOOST_TEST(r2);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==2);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);
	{ //value move-move
		leaf::expect<e_err> exp;
		leaf::result<val> r1 = val();
		BOOST_TEST(r1);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==1);
		leaf::result<val> r2 = std::move(r1);
		BOOST_TEST(r2);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==2);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);
	{ //value move-move
		leaf::expect<e_err> exp;
		val v;
		leaf::result<val> r1 = v;
		BOOST_TEST(r1);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==2);
		leaf::result<val> r2 = std::move(r1);
		BOOST_TEST(r2);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==3);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);

	{ //value default-assign copy
		leaf::expect<e_err> exp;
		leaf::result<val> r1;
		BOOST_TEST(r1);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==1);
		leaf::result<val> r2; r2=r1;
		BOOST_TEST(r2);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==2);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);
	{ //value move-assign copy
		leaf::expect<e_err> exp;
		leaf::result<val> r1 = val();
		BOOST_TEST(r1);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==1);
		leaf::result<val> r2; r2=r1;
		BOOST_TEST(r2);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==2);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);
	{ //value move-assign copy
		leaf::expect<e_err> exp;
		val v;
		leaf::result<val> r1 = v;
		BOOST_TEST(r1);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==2);
		leaf::result<val> r2; r2=r1;
		BOOST_TEST(r2);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==3);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);

	{ //value default-assign move
		leaf::expect<e_err> exp;
		leaf::result<val> r1;
		BOOST_TEST(r1);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==1);
		leaf::result<val> r2; r2=std::move(r1);
		BOOST_TEST(r2);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==2);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);
	{ //value move-assign move
		leaf::expect<e_err> exp;
		leaf::result<val> r1 = val();
		BOOST_TEST(r1);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==1);
		leaf::result<val> r2; r2=std::move(r1);
		BOOST_TEST(r2);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==2);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);
	{ //value move-assign move
		leaf::expect<e_err> exp;
		val v;
		leaf::result<val> r1 = v;
		BOOST_TEST(r1);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==2);
		leaf::result<val> r2; r2=std::move(r1);
		BOOST_TEST(r2);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==3);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);

	{ //value default - capture - copy
		leaf::expect<e_err> exp;
		leaf::result<val> r1 = capture(exp,leaf::result<val>());
		BOOST_TEST(r1);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==1);
		leaf::result<val> r2 = r1;
		BOOST_TEST(r2);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==2);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);
	{ //value move - capture - copy
		leaf::expect<e_err> exp;
		leaf::result<val> r1 = capture(exp,leaf::result<val>(val()));
		BOOST_TEST(r1);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==1);
		leaf::result<val> r2 = r1;
		BOOST_TEST(r2);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==2);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);
	{ //value copy - capture - copy
		leaf::expect<e_err> exp;
		val v;
		leaf::result<val> r1 = capture(exp,leaf::result<val>(v));
		BOOST_TEST(r1);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==2);
		leaf::result<val> r2 = r1;
		BOOST_TEST(r2);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==3);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);

	{ //value default - capture - move
		leaf::expect<e_err> exp;
		leaf::result<val> r1 = capture(exp,leaf::result<val>());
		BOOST_TEST(r1);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==1);
		leaf::result<val> r2 = std::move(r1);
		BOOST_TEST(r2);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==2);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);
	{ //value move - capture - move
		leaf::expect<e_err> exp;
		leaf::result<val> r1 = capture(exp,leaf::result<val>(val()));
		BOOST_TEST(r1);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==1);
		leaf::result<val> r2 = std::move(r1);
		BOOST_TEST(r2);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==2);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);
	{ //value move - capture - move
		leaf::expect<e_err> exp;
		val v;
		leaf::result<val> r1 = capture(exp,leaf::result<val>(v));
		BOOST_TEST(r1);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==2);
		leaf::result<val> r2 = std::move(r1);
		BOOST_TEST(r2);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==3);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);

	{ //value default - capture - assign copy
		leaf::expect<e_err> exp;
		leaf::result<val> r1 = capture(exp,leaf::result<val>());
		BOOST_TEST(r1);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==1);
		leaf::result<val> r2; r2=r1;
		BOOST_TEST(r2);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==2);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);
	{ //value move - capture - assign copy
		leaf::expect<e_err> exp;
		leaf::result<val> r1 = capture(exp,leaf::result<val>(val()));
		BOOST_TEST(r1);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==1);
		leaf::result<val> r2; r2=r1;
		BOOST_TEST(r2);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==2);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);
	{ //value move - capture - assign copy
		leaf::expect<e_err> exp;
		val v;
		leaf::result<val> r1 = capture(exp,leaf::result<val>(v));
		BOOST_TEST(r1);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==2);
		leaf::result<val> r2; r2=r1;
		BOOST_TEST(r2);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==3);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);

	{ //value default - capture - assign move
		leaf::expect<e_err> exp;
		leaf::result<val> r1 = capture(exp,leaf::result<val>());
		BOOST_TEST(r1);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==1);
		leaf::result<val> r2; r2=std::move(r1);
		BOOST_TEST(r2);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==2);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);
	{ //value move - capture - assign move
		leaf::expect<e_err> exp;
		leaf::result<val> r1 = capture(exp,leaf::result<val>(val()));
		BOOST_TEST(r1);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==1);
		leaf::result<val> r2; r2=std::move(r1);
		BOOST_TEST(r2);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==2);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);
	{ //value move - capture - assign move
		leaf::expect<e_err> exp;
		val v;
		leaf::result<val> r1 = capture(exp,leaf::result<val>(v));
		BOOST_TEST(r1);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==2);
		leaf::result<val> r2; r2=std::move(r1);
		BOOST_TEST(r2);
		BOOST_TEST(err::count==0);
		BOOST_TEST(val::count==3);
		}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);

	{ //error move - copy
		leaf::expect<e_err> exp;
		leaf::result<val> r1 = leaf::error( e_err { } );
		BOOST_TEST(!r1);
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
		leaf::result<val> r2 = r1;
		BOOST_TEST(!r2);
		BOOST_TEST(handle_error(exp,r2,[ ]{ }));
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);
	{ //error copy - copy
		leaf::expect<e_err> exp;
		leaf::error e( e_err{ } );
		leaf::result<val> r1 = e;
		BOOST_TEST(!r1);
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
		leaf::result<val> r2 = r1;
		BOOST_TEST(!r2);
		BOOST_TEST(handle_error(exp,r2,[ ]{ }));
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);

	{ //error move - move
		leaf::expect<e_err> exp;
		leaf::result<val> r1 = leaf::error( e_err { } );
		BOOST_TEST(!r1);
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
		leaf::result<val> r2 = std::move(r1);
		BOOST_TEST(!r2);
		BOOST_TEST(handle_error(exp,r2,[ ]{ }));
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);
	{ //error copy - move
		leaf::expect<e_err> exp;
		leaf::error e( e_err{ } );
		leaf::result<val> r1 = e;
		BOOST_TEST(!r1);
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
		leaf::result<val> r2 = std::move(r1);
		BOOST_TEST(!r2);
		BOOST_TEST(handle_error(exp,r2,[ ]{ }));
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);

	{ //error move - assign copy
		leaf::expect<e_err> exp;
		leaf::result<val> r1 = leaf::error( e_err { } );
		BOOST_TEST(!r1);
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
		leaf::result<val> r2; r2=r1;
		BOOST_TEST(!r2);
		BOOST_TEST(handle_error(exp,r2,[ ]{ }));
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);
	{ //error copy - assign copy
		leaf::expect<e_err> exp;
		leaf::error e( e_err{ } );
		leaf::result<val> r1 = e;
		BOOST_TEST(!r1);
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
		leaf::result<val> r2; r2=r1;
		BOOST_TEST(!r2);
		BOOST_TEST(handle_error(exp,r2,[ ]{ }));
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);

	{ //error move - assign move
		leaf::expect<e_err> exp;
		leaf::result<val> r1 = leaf::error( e_err { } );
		BOOST_TEST(!r1);
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
		leaf::result<val> r2; r2=std::move(r1);
		BOOST_TEST(!r2);
		BOOST_TEST(handle_error(exp,r2,[ ]{ }));
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);
	{ //error copy - assign move
		leaf::expect<e_err> exp;
		leaf::error e( e_err{ } );
		leaf::result<val> r1 = e;
		BOOST_TEST(!r1);
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
		leaf::result<val> r2; r2=std::move(r1);
		BOOST_TEST(!r2);
		BOOST_TEST(handle_error(exp,r2,[ ]{ }));
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);

	{ //error move - capture - copy
		leaf::expect<e_err> exp;
		leaf::result<val> r1 = capture( exp, leaf::result<val>( leaf::error( e_err { } ) ) );
		BOOST_TEST(!r1);
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
		leaf::result<val> r2 = r1;
		BOOST_TEST(!r2);
		BOOST_TEST(handle_error(exp,r2,[ ]{ }));
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);
	{ //error copy - capture - copy
		leaf::expect<e_err> exp;
		leaf::error e( e_err{ } );
		leaf::result<val> r1 = capture( exp, leaf::result<val>(e) );
		BOOST_TEST(!r1);
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
		leaf::result<val> r2 = r1;
		BOOST_TEST(!r2);
		BOOST_TEST(handle_error(exp,r2,[ ]{ }));
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);

	{ //error move - capture - move
		leaf::expect<e_err> exp;
		leaf::result<val> r1 = capture( exp, leaf::result<val>( leaf::error( e_err { } ) ) );
		BOOST_TEST(!r1);
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
		leaf::result<val> r2 = std::move(r1);
		BOOST_TEST(!r2);
		BOOST_TEST(handle_error(exp,r2,[ ]{ }));
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);
	{ //error copy - capture - move
		leaf::expect<e_err> exp;
		leaf::error e( e_err{ } );
		leaf::result<val> r1 = capture( exp, leaf::result<val>(e) );
		BOOST_TEST(!r1);
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
		leaf::result<val> r2 = std::move(r1);
		BOOST_TEST(!r2);
		BOOST_TEST(handle_error(exp,r2,[ ]{ }));
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);

	{ //error move - capture - assign copy
		leaf::expect<e_err> exp;
		leaf::result<val> r1 = capture( exp, leaf::result<val>( leaf::error( e_err { } ) ) );
		BOOST_TEST(!r1);
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
		leaf::result<val> r2; r2=r1;
		BOOST_TEST(!r2);
		BOOST_TEST(handle_error(exp,r2,[ ]{ }));
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);
	{ //error copy - capture - assign copy
		leaf::expect<e_err> exp;
		leaf::error e( e_err{ } );
		leaf::result<val> r1 = capture( exp, leaf::result<val>(e) );
		BOOST_TEST(!r1);
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
		leaf::result<val> r2; r2=r1;
		BOOST_TEST(!r2);
		BOOST_TEST(handle_error(exp,r2,[ ]{ }));
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);

	{ //error move - capture - assign move
		leaf::expect<e_err> exp;
		leaf::result<val> r1 = capture( exp, leaf::result<val>( leaf::error( e_err { } ) ) );
		BOOST_TEST(!r1);
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
		leaf::result<val> r2; r2=std::move(r1);
		BOOST_TEST(!r2);
		BOOST_TEST(handle_error(exp,r2,[ ]{ }));
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);
	{ //error copy - capture - assign move
		leaf::expect<e_err> exp;
		leaf::error e( e_err{ } );
		leaf::result<val> r1 = capture( exp, leaf::result<val>(e) );
		BOOST_TEST(!r1);
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
		leaf::result<val> r2; r2=std::move(r1);
		BOOST_TEST(!r2);
		BOOST_TEST(handle_error(exp,r2,[ ]{ }));
		BOOST_TEST(err::count==1);
		BOOST_TEST(val::count==0);
	}
	BOOST_TEST(err::count==0);
	BOOST_TEST(val::count==0);

	return boost::report_errors();
}
