// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/result_traits.hpp>
#include <boost/leaf/result.hpp>

template <class T, class E>
struct result_type
{
	T value() const;
	E error() const;
};

struct not_result_type1
{
	int value() const;
};

struct not_result_type2
{
	int error() const;
};

namespace leaf = boost::leaf;
using namespace leaf::leaf_detail;

static_assert(is_result_type<leaf::result<int>>::value, "");
static_assert(std::is_same<int &, typename result_traits<leaf::result<int>>::value_type>::value, "");
static_assert(std::is_same<leaf::error_id, typename result_traits<leaf::result<int>>::error_type>::value, "");

static_assert(is_result_type<leaf::result<void>>::value, "");
static_assert(std::is_same<void, typename result_traits<leaf::result<void>>::value_type>::value, "");
static_assert(std::is_same<leaf::error_id, typename result_traits<leaf::result<void>>::error_type>::value, "");

static_assert(is_result_type<result_type<int,float>>::value, "");
static_assert(std::is_same<int, typename result_traits<result_type<int,float>>::value_type>::value, "");
static_assert(std::is_same<float, typename result_traits<result_type<int,float>>::error_type>::value, "");

static_assert(is_result_type<result_type<void,float>>::value, "");
static_assert(std::is_same<void, typename result_traits<result_type<void,float>>::value_type>::value, "");
static_assert(std::is_same<float, typename result_traits<result_type<void,float>>::error_type>::value, "");

static_assert(!is_result_type<result_type<int,void>>::value, "");
static_assert(!is_result_type<not_result_type1>::value, "");
static_assert(!is_result_type<not_result_type2>::value, "");

int main()
{
	return 0;
}
