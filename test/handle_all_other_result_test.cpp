// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/handle.hpp>
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

template <int> struct info { int value; };

template <class T, class E>
class result
{
	enum class variant
	{
		value,
		error
	};
	T value_;
	E error_;
	variant which_;
public:
	result( T const & value ) noexcept:
		value_(value),
		which_(variant::value)
	{
	}
	result( E const & error ) noexcept:
		error_(error),
		which_(variant::error)
	{
	}
	explicit operator bool() const noexcept
	{
		return which_==variant::value;
	}
	T const & value() const
	{
		assert(which_==variant::value);
		return value_;
	}
	E const & error() const
	{
		assert(which_==variant::error);
		return error_;
	}
};

namespace boost { namespace leaf {
	template <class T, class E>
	struct is_result_type<result<T, E>>: std::true_type
	{
	};
} }

result<int,std::error_code> f( bool succeed )
{
	if( succeed )
		return 42;
	else
		return make_error_code(std::errc::no_such_file_or_directory);
}

result<int,std::error_code> g( bool succeed )
{
	if( auto r = f(succeed) )
		return r;
	else
		return leaf::error_id(r.error()).propagate(info<42>{42});
}

int main()
{
	{
		int r = leaf::handle_all(
			[ ]
			{
				return g(true);
			},
			[ ]
			{
				return -1;
			} );
		BOOST_TEST(r==42);
	}
	{
		int r = leaf::handle_all(
			[&]
			{
				auto r = g(false);
				BOOST_TEST(!r);
				auto ec = r.error();
				BOOST_TEST(ec.message()=="LEAF error, use with leaf::handle_some or leaf::handle_all.");
				BOOST_TEST(!std::strcmp(ec.category().name(),"LEAF error, use with leaf::handle_some or leaf::handle_all."));
				return r;
			},
			[ ]( info<42> const & x, std::error_code const & ec )
			{
				BOOST_TEST(x.value==42);
				BOOST_TEST(ec==make_error_code(std::errc::no_such_file_or_directory));
				return 1;
			},
			[ ]
			{
				return 2;
			} );
		BOOST_TEST(r==1);
	}
	return boost::report_errors();
}
