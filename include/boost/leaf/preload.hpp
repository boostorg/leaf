//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_8F1C53BEB39F11E8A1C5B6F3E99C4353
#define UUID_8F1C53BEB39F11E8A1C5B6F3E99C4353

#include <boost/leaf/error.hpp>
#include <type_traits>
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
			template<class F, class E = void> struct can_haz_call { static constexpr bool value=false; };
			template<class F> struct can_haz_call<F, decltype(std::declval<F>()(), void())> { static constexpr bool value=true; };
			template <class T,bool Deferred=can_haz_call<T>::value>
			struct
			defer_dispatch
				{
				static
				void
				propagate_( error const & e, T && x ) noexcept
					{
					e.propagate(std::move(x));
					}
				};
			template <class F>
			struct
			defer_dispatch<F,true>
				{
				static
				void
				propagate_( error const & e, F && x ) noexcept
					{
					e.propagate(std::forward<F>(x)());
					}
				};
			template <int I,class Tuple>
			struct
			propagate_meta
				{
				static
				void
				propagate( error const & e, Tuple && t ) noexcept
					{
					typedef typename std::tuple_element<I-1,Tuple>::type ith_type;
					defer_dispatch<ith_type>::propagate_(e,std::move(std::get<I-1>(std::move(t))));
					propagate_meta<I-1,Tuple>::propagate(e,std::move(t));
					}
				};
			template <class Tuple>
			struct
			propagate_meta<0,Tuple>
				{
				static void propagate( error const &, Tuple && ) noexcept { }
				};
			template <class... T>
			class
			preloaded
				{
				preloaded( preloaded const & ) = delete;
				preloaded & operator=( preloaded const & ) = delete;
				typedef std::tuple<typename std::remove_const<typename std::remove_reference<T>::type>::type...> tuple_type;
				optional<tuple_type> to_propagate_;
				public:
				template <class... U>
				explicit
				preloaded( U && ... a ):
					to_propagate_(tuple_type(std::forward<U>(a)...))
					{
					}
				preloaded( preloaded && x ) noexcept:
					to_propagate_(std::move(x.to_propagate_))
					{
					assert(!x.to_propagate_.has_value());
					}
				~preloaded() noexcept
					{
					if( to_propagate_.has_value() )
						if( error const * e = current_error() )
							propagate_meta<sizeof...(T),tuple_type>::propagate(*e,to_propagate_.extract_value());
					}
				void
				cancel() noexcept
					{
					to_propagate_.reset();
					}
				};
			}
		template <class... T>
		leaf_detail::preloaded<T...>
		preload( T && ... a )
			{
			return leaf_detail::preloaded<T...>(std::forward<T>(a)...);
			}
		}
	}

#endif
