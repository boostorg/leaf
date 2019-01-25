// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/handle.hpp>
#include "boost/core/lightweight_test.hpp"

namespace leaf = boost::leaf;

////////////////////////////////

enum class api_error
{
	no_such_device = 1,
	no_such_file_or_directory,
	no_such_process
};

namespace std { template<> struct is_error_condition_enum<api_error>: std::true_type { }; }

std::error_category const & api_category()
{
	class cat : public std::error_category
	{
		char const * name() const noexcept
		{
			return "api";
		}
		std::string message(int ev) const
		{
			return "API error";
		}
		bool equivalent(std::error_code const & code, int condition) const noexcept
		{
			switch( api_error(condition) )
			{
			case api_error::no_such_device:
				return code==std::error_code(ENODEV, std::system_category());
			case api_error::no_such_file_or_directory:
				return code==std::error_code(ENOENT, std::system_category());
			case api_error::no_such_process:
				return code==std::error_code(ESRCH, std::system_category());
			default:
				return false;
			}
		}
	};
	static cat c;
	return c;
}

std::error_condition make_error_condition(api_error e)
{
	return std::error_condition(int(e), api_category());
}

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

struct e_ec { std::error_code value; };

int main()
{
	{
		int r = leaf::handle_all(
			[ ]() -> result<int,std::error_code>
			{
				return std::error_code(ENOENT, std::system_category());
			},
			[ ]( std::error_code const & ec )
			{
				BOOST_TEST(ec==std::error_code(ENOENT, std::system_category()));
				return 42;
			} );
		BOOST_TEST(r==42);
	}
	{
		int r = leaf::handle_all(
			[ ]() -> result<int,std::error_code>
			{
				return std::error_code(ENOENT, std::system_category());
			},
			[ ]( leaf::match<leaf::error_condition<api_error>, api_error::no_such_file_or_directory> )
			{
				return 42;
			},
			[ ]
			{
				return -42;
			} );
		BOOST_TEST(r==42);
	}
	{
		int r = leaf::handle_all(
			[ ]() -> result<int, std::error_code>
			{
				auto r1 = leaf::handle_some(
					[ ]() -> result<int, std::error_code>
					{
						return std::error_code(ENOENT, std::system_category());
					} );
				auto r2 = leaf::handle_some(
					[ ]() -> result<int, std::error_code>
					{
						return std::error_code(ESRCH, std::system_category());
					} );
				(void) r2;
				return r1;
			},
			[ ]( leaf::match<leaf::error_condition<api_error>, api_error::no_such_file_or_directory> )
			{
				return 42;
			},
			[ ]
			{
				return -42;
			} );
		BOOST_TEST(r==42);
	}
	{
		int r = leaf::handle_all(
			[ ]() -> result<int, std::error_code>
			{
				return leaf::error_id(
					std::error_code(ENOENT, std::system_category()),
					e_ec{std::error_code(ESRCH, std::system_category())});
			},
			[ ]( leaf::match<leaf::error_condition<api_error>, api_error::no_such_file_or_directory>, leaf::match<leaf::error_condition<e_ec, api_error>, api_error::no_such_process, api_error::no_such_device> )
			{
				return 42;
			},
			[ ]
			{
				return -42;
			} );
		BOOST_TEST(r==42);
	}
	{
		int r = leaf::handle_all(
			[ ]() -> result<int, std::error_code>
			{
				return leaf::error_id(
					std::error_code(ENOENT, std::system_category()),
					e_ec{std::error_code(ENODEV, std::system_category())});
			},
			[ ]( leaf::match<leaf::error_condition<api_error>, api_error::no_such_file_or_directory>, leaf::match<leaf::error_condition<e_ec, api_error>, api_error::no_such_process, api_error::no_such_device> )
			{
				return 42;
			},
			[ ]
			{
				return -42;
			} );
		BOOST_TEST(r==42);
	}
	return boost::report_errors();
}
