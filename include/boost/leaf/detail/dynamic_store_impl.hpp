#ifndef BOOST_LEAF_C86E4C4ED0F011E8BB777EB8A659E189
#define BOOST_LEAF_C86E4C4ED0F011E8BB777EB8A659E189

// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/dynamic_store.hpp>
#include <boost/leaf/detail/static_store.hpp>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		namespace dynamic_store_internal
		{
			template <int I, class Tuple>
			struct tuple_for_each
			{
				static void unload( int error_id, Tuple && tup ) noexcept
				{
					tuple_for_each<I-1,Tuple>::unload(error_id,std::move(tup));
					auto && opt = std::get<I-1>(std::move(tup));
					if( opt.has_value() )
						put_slot(error_id, std::move(opt).value());
				}

				static void print( std::ostream & os, Tuple const & tup )
				{
					typedef typename std::tuple_element<I-1,Tuple>::type ith_type;
					tuple_for_each<I-1,Tuple>::print(os,tup);
					auto & opt = std::get<I-1>(tup);
					if( opt.has_value() && diagnostic<typename ith_type::value_type>::print(os,opt.value()) )
						os << std::endl;
				}
			};

			template <class Tuple>
			struct tuple_for_each<0, Tuple>
			{
				static void unload( int, Tuple && ) noexcept { }
				static void print( std::ostream &, Tuple const & ) noexcept { }
			};
		}

		template <class... E>
		class dynamic_store_impl:
			public dynamic_store
		{
			int error_id_;
			std::tuple<leaf_detail::optional<E>...> s_;

			int error_id() const noexcept
			{
				return error_id_;
			}

			int unload() noexcept
			{
				return unload(error_id_);
			}

			int unload( int error_id ) noexcept
			{
				dynamic_store_internal::tuple_for_each<sizeof...(E),decltype(s_)>::unload(error_id,std::move(s_));
				return error_id;
			}

		public:

			dynamic_store_impl( int error_id, static_store<E...> && ss ) noexcept:
				error_id_(error_id),
				s_(std::make_tuple( std::get<static_store_internal::tuple_type_index<static_store_internal::static_store_slot<E>,decltype(ss.s_)>::value>(std::move(ss.s_)).extract_optional(error_id)... ))
			{
			}
		};
	} // leaf_detail

} }

#endif
