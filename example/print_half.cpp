//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//This program is an adaptation of the following Boost Outcome example:
//https://github.com/ned14/outcome/blob/master/doc/src/snippets/using_result.cpp

#include <boost/leaf/handle.hpp>
#include <algorithm>
#include <ctype.h>
#include <string>
#include <iostream>

namespace leaf = boost::leaf;

enum class ConversionErrc
{
	EmptyString = 1,
	IllegalChar,
	TooLong
};

namespace boost { namespace leaf {
	template <> struct is_error_type<ConversionErrc>: public std::true_type { };
} }

leaf::result<int> convert(const std::string& str) noexcept
{
	if (str.empty())
		return leaf::new_error(ConversionErrc::EmptyString);

	if (!std::all_of(str.begin(), str.end(), ::isdigit))
		return leaf::new_error(ConversionErrc::IllegalChar);

	if (str.length() > 9)
		return leaf::new_error(ConversionErrc::TooLong);

	return atoi(str.c_str());
}

//Do not static_store BigInt to actually work -- it's a stub.
struct BigInt
{
	static leaf::result<BigInt> fromString(const std::string& s) { return BigInt{s}; }
	explicit BigInt(const std::string&) { }
	BigInt half() const { return BigInt{""}; }
	friend std::ostream& operator<<(std::ostream& o, const BigInt&) { return o << "big int half"; }
};

//This function handles ConversionErrc::TooLong errors, forwards any other error to the caller.
leaf::result<void> print_half(const std::string& text)
{
	return leaf::handle_some(
		[&]() -> leaf::result<void>
		{
			LEAF_AUTO(r,convert(text));
			std::cout << r / 2 << std::endl;
			return { };
		},
		[&]( leaf::match<ConversionErrc,ConversionErrc::TooLong> ) -> leaf::result<void>
		{
			LEAF_AUTO(i, BigInt::fromString(text));
			std::cout << i.half() << std::endl;
			return { };
		} );
}

int main( int argc, char const * argv[ ] )
{
	return leaf::handle_all(
		[&]() -> leaf::result<int>
		{
			LEAF_CHECK( print_half(argc<2 ? "" : argv[1]) );
			std::cout << "ok" << std::endl;
			return 0;
		},

		[ ]( leaf::match<ConversionErrc,ConversionErrc::EmptyString> )
		{
			std::cerr << "Empty string!" << std::endl;
			return 1;
		},

		[ ]( leaf::match<ConversionErrc,ConversionErrc::IllegalChar> )
		{
			std::cerr << "Illegal char!" << std::endl;
			return 2;
		},

		[ ]( leaf::error e )
		{
			//This will never execute in this program, but it would detect logic errors where an unknown error reaches main.
			//In this case, we print diagnostic information.
			std::cerr << "Unknown error, cryptic diagnostic information follows." << std::endl;
			e.diagnostic_output(std::cerr);
			return 3;
		} );
}
