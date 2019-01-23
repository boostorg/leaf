// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

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
		return std::error_code(ENOENT, std::system_category());
}

result<int,std::error_code> g( bool succeed )
{
	if( auto r = f(succeed) )
		return r;
	else
		return leaf::error_id(r.error(), info<42>{42});
}

int main()
{
	{
		result<int,std::error_code> r = leaf::handle_some(
			[ ]
			{
				return g(true);
			} );
		BOOST_TEST(r);
		BOOST_TEST(r.value()==42);
	}
	{
		result<int,std::error_code> r = leaf::handle_some(
			[&]
			{
				auto r = g(false);
				BOOST_TEST(!r);
				auto ec = r.error();
				BOOST_TEST(ec.message()=="LEAF error, use with leaf::handle_some or leaf::handle_all.");
				BOOST_TEST(!std::strcmp(ec.category().name(),"LEAF error, use with leaf::handle_some or leaf::handle_all."));
				return r;
			},
			[ ]( info<42> const & x, leaf::e_original_ec const & ec )
			{
				BOOST_TEST(x.value==42);
				BOOST_TEST(ec.value==std::error_code(ENOENT, std::system_category()));
				return ec.value;
			} );
		BOOST_TEST(!r);
		BOOST_TEST(r.error() == std::error_code(ENOENT, std::system_category()));
	}
	return boost::report_errors();
}
