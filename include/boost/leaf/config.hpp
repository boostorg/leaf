#ifndef LEAF_13D3591AFC6811E9883D0A836044C98A
#define LEAF_13D3591AFC6811E9883D0A836044C98A

// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// The following is based on Boost Config.

// (C) Copyright John Maddock 2001 - 2003.
// (C) Copyright Martin Wille 2003.
// (C) Copyright Guillaume Melquiond 2003.

////////////////////////////////////////

// Configure LEAF_NO_EXCEPTIONS, unless already #defined
#ifndef LEAF_NO_EXCEPTIONS

#	if defined __clang__ && !defined(__ibmxl__)
//	Clang C++ emulates GCC, so it has to appear early.

#		if !__has_feature(cxx_exceptions)
#			define LEAF_NO_EXCEPTIONS
#		endif

#	elif defined __DMC__
//	Digital Mars C++

#		if !defined(_CPPUNWIND)
#			define LEAF_NO_EXCEPTIONS
#		endif

#	elif defined(__GNUC__) && !defined(__ibmxl__)
//	GNU C++:

#		if !defined(__EXCEPTIONS)
#			define LEAF_NO_EXCEPTIONS
#		endif

#	elif defined __KCC
//	Kai C++

#		if !defined(_EXCEPTIONS)
#			define LEAF_NO_EXCEPTIONS
#		endif

#	elif defined __CODEGEARC__
//	CodeGear - must be checked for before Borland

#		if !defined(_CPPUNWIND) && !defined(__EXCEPTIONS)
#			define LEAF_NO_EXCEPTIONS
#		endif

#	elif defined __BORLANDC__
//	Borland

#		if !defined(_CPPUNWIND) && !defined(__EXCEPTIONS)
# 			define LEAF_NO_EXCEPTIONS
#		endif

#	elif defined  __MWERKS__
//	Metrowerks CodeWarrior

#		if !__option(exceptions)
#			define LEAF_NO_EXCEPTIONS
#		endif

#	elif defined(__IBMCPP__) && defined(__COMPILER_VER__) && defined(__MVS__)
//	IBM z/OS XL C/C++

#		if !defined(_CPPUNWIND) && !defined(__EXCEPTIONS)
#			define LEAF_NO_EXCEPTIONS
#		endif

#	elif defined(__ibmxl__)
//	IBM XL C/C++ for Linux (Little Endian)

#		if !__has_feature(cxx_exceptions)
#			define LEAF_NO_EXCEPTIONS
#		endif

#	elif defined _MSC_VER
//	Microsoft Visual C++
//
//	Must remain the last #elif since some other vendors (Metrowerks, for
//	example) also #define _MSC_VER

#		if !defined(_CPPUNWIND)
#			define LEAF_NO_EXCEPTIONS
#		endif
#	endif

#endif

#ifndef LEAF_DIAGNOSTICS
#	define LEAF_DIAGNOSTICS 1
#endif

#if LEAF_DIAGNOSTICS!=0 && LEAF_DIAGNOSTICS!=1
#	error LEAF_DIAGNOSTICS must be 0 or 1.
#endif

#ifdef _MSC_VER
#	define LEAF_ALWAYS_INLINE __forceinline
#else
#	define LEAF_ALWAYS_INLINE __attribute__((always_inline)) inline
#endif

#if __cplusplus > 201402L
#	define LEAF_CONSTEXPR constexpr
#	define LEAF_STD_UNCAUGHT_EXCEPTIONS 1
#else
#	define LEAF_CONSTEXPR
#	define LEAF_STD_UNCAUGHT_EXCEPTIONS 0
#endif

#endif
