//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_AFBBD676B2FF11E8984C7976AE35F1A2
#define UUID_AFBBD676B2FF11E8984C7976AE35F1A2

#include <boost/leaf/throw_exception.hpp>
#include <boost/leaf/detail/print.hpp>
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
			////////////////////////////////////////
			template <class SlotsTuple,class... List>
			struct slots_subset;
			template <class SlotsTuple,class Car,class... Cdr>
			struct
			slots_subset<SlotsTuple,Car,Cdr...>
				{
				static
				bool
				have_values( SlotsTuple const & tup, error const & e ) noexcept
					{
					auto & sl = std::get<tuple_type_index<Car,SlotsTuple>::value>(tup);
					return sl.has_value() && sl.value().e==e && slots_subset<SlotsTuple,Cdr...>::have_values(tup,e);
					}
				};
			template <class SlotsTuple>
			struct
			slots_subset<SlotsTuple>
				{
				static bool have_values( SlotsTuple const &, error const & ) noexcept { return true; }
				};
			////////////////////////////////////////
			template <int I,class Tuple>
			struct
			tuple_for_each
				{
				static
				void
				print( std::ostream & os, Tuple const & tup )
					{
					tuple_for_each<I-1,Tuple>::print(os,tup);
					auto & opt = std::get<I-1>(tup);
					if( opt.has_value() )
						{
						auto & x = opt.value();
						if( diagnostic<decltype(x.v)>::print(os,x.v) )
							os << "(0x" << x.e << ')' << std::endl;
						}
					}
				static
				void
				print( std::ostream & os, Tuple const & tup, error const & e )
					{
					tuple_for_each<I-1,Tuple>::print(os,tup,e);
					auto & opt = std::get<I-1>(tup);
					if( opt.has_value() )
						{
						auto & x = opt.value();
						if( x.e==e && diagnostic<decltype(x.v)>::print(os,x.v) )
							os << std::endl;
						}
					}
				static
				void
				clear( Tuple & tup ) noexcept
					{
					tuple_for_each<I-1,Tuple>::clear(tup);
					std::get<I-1>(tup).reset();
					}
				};
			template <class Tuple>
			struct
			tuple_for_each<0,Tuple>
				{
				static void print( std::ostream &, Tuple const & ) noexcept { }
				static void print( std::ostream &, Tuple const &, error const & ) noexcept { }
				static void clear( Tuple & ) noexcept { }
				};
			////////////////////////////////////////
			template <class T>
			optional<T>
			convert_optional( slot<T> && x, error const & e ) noexcept
				{
				if( x.has_value() && x.value().e==e )
					return optional<T>(std::move(x).value().v);
				else
					return optional<T>();
				}
			}
		class error_capture;

		template <class>
		struct
		dependent_type
			{
			typedef leaf::error_capture error_capture;
			};

		template <class... E>
		class expect;

		template <class P,class... E>
		decltype(P::value) const * peek( expect<E...> const &, error const & ) noexcept;

		template <class... E>
		class
		expect
			{
			friend class error;

			template <class P,class... E_>
			friend decltype(P::value) const * leaf::peek( expect<E_...> const &, error const & ) noexcept;

			expect( expect const & ) = delete;
			expect & operator=( expect const & ) = delete;

			std::tuple<leaf_detail::slot<E>...>  s_;

			template <class F,class... MatchTypes>
			int
			unwrap( leaf_detail::match_fn<F,MatchTypes...> const & m, error const & e, bool & matched ) const
				{
				using namespace leaf_detail;
				if( !matched && (matched=slots_subset<decltype(s_),slot<MatchTypes>...>::have_values(s_,e)) )
					(void) m.f( *leaf::peek<MatchTypes>(*this,e)... );
				return 42;
				}
			template <class... MatchTypes>
			int
			unwrap( leaf_detail::match_no_fn<MatchTypes...> const & m, error const & e, bool & matched ) const noexcept
				{
				using namespace leaf_detail;
				if( !matched )
					matched = slots_subset<decltype(s_),slot<MatchTypes>...>::have_values(s_,e);
				return 42;
				}
			bool propagate_;
			public:
			expect() noexcept:
				propagate_(true)
				{
				}
			~expect() noexcept
				{
				if( !propagate_ )
					{
					leaf_detail::tuple_for_each<sizeof...(E),decltype(s_)>::clear(s_);
					error::clear_next_error();
					}
				}
			template <class... M>
			friend
			bool
			handle_error( expect & exp, error const & e, M && ... m ) noexcept
				{
				bool matched = false;
				{ using _ = int[ ]; (void) _ { 42, exp.unwrap(m,e,matched)... }; }
				if( matched )
					exp.propagate_ = false;
				return matched;
				}
			friend
			void
			diagnostic_print( std::ostream & os, expect const & exp )
				{
				leaf_detail::tuple_for_each<sizeof...(E),decltype(exp.s_)>::print(os,exp.s_);
				}
			friend
			void
			diagnostic_print( std::ostream & os, expect const & exp, error const & e )
				{
				leaf_detail::tuple_for_each<sizeof...(E),decltype(exp.s_)>::print(os,exp.s_,e);
				}
			friend
			typename dependent_type<expect>::error_capture
			capture( expect & exp, error const & e )
				{
				using namespace leaf_detail;
				typename dependent_type<expect>::error_capture cap(
					e,
					std::make_tuple(
						convert_optional(
							std::move(std::get<tuple_type_index<slot<E>,decltype(exp.s_)>::value>(exp.s_)),e)...));
				return cap;
				}
			void
			propagate() noexcept
				{
				propagate_ = true;
				}
			};
		template <class P,class... E>
		decltype(P::value) const *
		peek( expect<E...> const & exp, error const & e ) noexcept
			{
			auto & opt = std::get<leaf_detail::type_index<P,E...>::value>(exp.s_);
			if( opt.has_value() )
				{
				auto & x = opt.value();
				if( x.e==e )
					return &x.v.value;
				}
			return 0;
			}
		}
	}

#endif
