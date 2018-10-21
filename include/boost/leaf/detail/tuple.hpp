//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_171F6820D42311E89C0E377FC92C3C47
#define UUID_171F6820D42311E89C0E377FC92C3C47

#include <tuple>

namespace
boost
	{
	namespace
	leaf
		{
		namespace
		leaf_detail
			{
			template <class T,class... List>
			struct type_index;
			template <class T,class... Cdr>
			struct
			type_index<T,T,Cdr...>
				{
				static const int value = 0;
				};
			template <class T,class Car,class... Cdr>
			struct
			type_index<T,Car,Cdr...>
				{
				static const int value = 1 + type_index<T,Cdr...>::value;
				};
			template <class T,class Tuple>
			struct tuple_type_index;
			template <class T,class... TupleTypes>
			struct
			tuple_type_index<T,std::tuple<TupleTypes...>>
				{
				static const int value = type_index<T,TupleTypes...>::value;
				};
			}
		}
	}

#endif
