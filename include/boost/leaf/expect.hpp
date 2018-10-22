//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_AFBBD676B2FF11E8984C7976AE35F1A2
#define UUID_AFBBD676B2FF11E8984C7976AE35F1A2

#include <boost/leaf/throw_exception.hpp>
#include <boost/leaf/detail/tuple.hpp>
#include <boost/leaf/detail/print.hpp>

namespace
boost
	{
	namespace
	leaf
	 	{
		namespace
		leaf_detail
			{
			template <class Tuple,class... List>
			struct all_available_slot;
			template <class Tuple,class Car,class... Cdr>
			struct
			all_available_slot<Tuple,Car,Cdr...>
				{
				static
				bool
				check( Tuple const & tup, error const & e ) noexcept
					{
					auto & sl = std::get<tuple_type_index<Car,Tuple>::value>(tup);
					return sl.has_value() && sl.value().e==e && all_available_slot<Tuple,Cdr...>::check(tup,e);
					}
				};
			template <class Tuple>
			struct
			all_available_slot<Tuple>
				{
				static bool check( Tuple const &, error const & ) noexcept { return true; }
				};
			////////////////////////////////////////
			template <int I,class Tuple>
			struct
			tuple_print_slot
				{
				static
				void
				print( std::ostream & os, Tuple const & tup )
					{
					tuple_print_slot<I-1,Tuple>::print(os,tup);
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
					tuple_print_slot<I-1,Tuple>::print(os,tup,e);
					auto & opt = std::get<I-1>(tup);
					if( opt.has_value() )
						{
						auto & x = opt.value();
						if( x.e==e && diagnostic<decltype(x.v)>::print(os,x.v) )
							os << std::endl;
						}
					}
				};
			template <class Tuple>
			struct
			tuple_print_slot<0,Tuple>
				{
				static void print( std::ostream &, Tuple const & ) { }
				static void print( std::ostream &, Tuple const &, error const & ) { }
				};
			////////////////////////////////////////
			template <class T>
			optional<T>
			convert_optional( slot<T> && x, error const & e ) noexcept
				{
				if( x.has_value() && x.value().e==e )
					return optional<T>(std::move(x.extract_value().v));
				else
					return optional<T>();
				}
			}
		template <class... E>
		class expect;

		template <class P,class... E>
		decltype(P::value) const * peek( expect<E...> const &, error const & ) noexcept;

		class error_capture;

		template <class... E>
		class
		expect
			{
			friend class error;

			template <class P,class... E_>
			friend decltype(P::value) const * leaf::peek( expect<E_...> const &, error const & ) noexcept;

			template <class>
			struct
			dependent_type
				{
				typedef leaf::error_capture error_capture;
				};

			expect( expect const & ) = delete;
			expect & operator=( expect const & ) = delete;

			std::tuple<leaf_detail::slot<E>...>  s_;

			template <class F,class... MatchTypes>
			int
			unwrap( leaf_detail::match_impl<F,MatchTypes...> const & m, error const & e, bool & matched ) const noexcept
				{
				using namespace leaf_detail;
				if( !matched && (matched=all_available_slot<decltype(s_),slot<MatchTypes>...>::check(s_,e)) )
					(void) m.f( *leaf::peek<MatchTypes>(*this,e)... );
				return 42;
				}
			public:
			expect() noexcept
				{
				leaf_detail::clear_current_error();
				}
			~expect() noexcept
				{
				}
			template <class... M>
			friend
			void
			handle_error( expect const & exp, error const & e, M && ... m )
				{
				bool matched = false;
				{ using _ = int[ ]; (void) _ { 42, exp.unwrap(m,e,matched)... }; }
				if( !matched )
					throw_exception(mismatch_error());
				leaf_detail::clear_current_error(e);
				}
			friend
			void
			diagnostic_print( std::ostream & os, expect const & exp )
				{
				leaf_detail::tuple_print_slot<sizeof...(E),decltype(exp.s_)>::print(os,exp.s_);
				}
			friend
			void
			diagnostic_print( std::ostream & os, expect const & exp, error const & e )
				{
				leaf_detail::tuple_print_slot<sizeof...(E),decltype(exp.s_)>::print(os,exp.s_,e);
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
				clear_current_error(e);
				return cap;
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
