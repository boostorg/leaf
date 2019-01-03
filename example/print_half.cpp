//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//This program is an adaptation of the following Boost Outcome example:
//https://github.com/ned14/outcome/blob/master/doc/src/snippets/using_result.cpp

#include <boost/leaf/result.hpp>
#include <boost/leaf/expect.hpp>
#include <boost/leaf/diagnostic_output.hpp>
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
		return leaf::error(ConversionErrc::EmptyString);

	if (!std::all_of(str.begin(), str.end(), ::isdigit))
		return leaf::error(ConversionErrc::IllegalChar);

	if (str.length() > 9)
		return leaf::error(ConversionErrc::TooLong);

	return atoi(str.c_str());
}

//Do not expect BigInt to actually work -- it's a stub.
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
	leaf::expect<ConversionErrc> exp;
	if (leaf::result<int> r = convert(text))
	{
		std::cout << (r.value() / 2) << std::endl;
		return { };
	}
	else
	{
		return leaf::handle_error( exp, r,
			[&]( ConversionErrc ec ) -> leaf::result<void>
			{
				if( ec != ConversionErrc::TooLong )
					return r.error();
				LEAF_AUTO(i, BigInt::fromString(text));
				std::cout << i.half() << std::endl;
				return { };
			} );
	}
}

int main( int argc, char const * argv[ ] )
{
	leaf::expect<ConversionErrc> exp;
	if( leaf::result<void> r = print_half(argc<2 ? "" : argv[1]) )
	{
		std::cout << "ok" << std::endl;
		return 0;
	}
	else
	{
		return handle_error( exp, r,
			[ ]( ConversionErrc ec )
			{
				switch(ec)
				{
				case ConversionErrc::EmptyString:
					std::cerr << "Empty string!" << std::endl;
					break;
				default:
					assert(ec==ConversionErrc::IllegalChar); //Because print_half deals with ConversionErrc::TooLong.
					std::cerr << "Illegal char!" << std::endl;
				}
				return 1;
			},
			[&r]
			{
				//This will never execute in this program, but it would detect logic errors where an unknown error reaches main.
				//In this case, we print diagnostic information. Consider using leaf::unexpected_diagnostic_output in the
				//definition of exp.
				std::cerr << "Unknown error, cryptic diagnostic information follows." << std::endl;
				leaf::diagnostic_output(std::cerr,r);
				return 2;
			} );
	}
}
