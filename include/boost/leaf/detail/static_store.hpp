#ifndef BOOST_LEAF_AFBBD676B2FF11E8984C7976AE35F1A2
#define BOOST_LEAF_AFBBD676B2FF11E8984C7976AE35F1A2

//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/error.hpp>
#include <boost/leaf/detail/function_traits.hpp>
#include <boost/leaf/detail/mp11.hpp>
#include <tuple>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <class E,bool HasValue = has_data_member_value<E>::value>
		struct match_type;

		template <class E>
		struct match_type<E,true>
		{
			using type = decltype(E::value);

			static type const & get( E const & e ) noexcept
			{
				return e.value;
			}
		};

		template <class E>
		struct match_type<E,false>
		{
			using type = E;

			static type const & get( E const & e ) noexcept
			{
				return e;
			}
		};
	}

	template <class E, typename leaf_detail::match_type<E>::type... Value>
	struct match
	{
		typename leaf_detail::match_type<E>::type value;
	};

	template <class... Ex>
	struct catch_
	{
	};

	namespace leaf_detail
	{
		namespace static_store_internal
		{
			template <int I, class Tuple>
			struct tuple_for_each
			{
				static void reset( Tuple & tup ) noexcept
				{
					tuple_for_each<I-1,Tuple>::reset(tup);
					std::get<I-1>(tup).reset();
				}
			};

			template <class Tuple>
			struct tuple_for_each<0, Tuple>
			{
				static void reset( Tuple & ) noexcept { }
			};

			////////////////////////////////////////

			class enable_any
			{
			protected:

				enable_any() noexcept
				{
					++tl_unexpected_enabled_counter();
				}

				~enable_any() noexcept
				{
					--tl_unexpected_enabled_counter();
				}
			};

			template <class E>
			class static_store_slot:
				public slot<E>
			{
			public:
				optional<E> extract_optional( error const & e ) && noexcept
				{
					slot<E> const & s = *this;
					if( s.has_value() && s.value().e==e )
						return optional<E>(std::move(*this).value().v);
					else
						return optional<E>();
				}
			};

			template <>
			class static_store_slot<unexpected_error_info>:
				public slot<unexpected_error_info>,
				enable_any
			{
			};

			template <>
			class static_store_slot<complete_diagnostic_info>:
				public slot<complete_diagnostic_info>,
				enable_any
			{
			};

			////////////////////////////////////////

			template <class T, class... List>
			struct type_index;

			template <class T, class... Cdr>
			struct type_index<T, T, Cdr...>
			{
				static const int value = 0;
			};

			template <class T, class Car, class... Cdr>
			struct type_index<T, Car, Cdr...>
			{
				static const int value = 1 + type_index<T,Cdr...>::value;
			};

			template <class T, class Tuple>
			struct tuple_type_index;

			template <class T, class... TupleTypes>
			struct tuple_type_index<T,std::tuple<TupleTypes...>>
			{
				static const int value = type_index<T,TupleTypes...>::value;
			};

			////////////////////////////////////////

			template <class T>
			bool check_value_pack( T const & x, T const & v ) noexcept
			{
				return x==v;
			}

			template <class T, class... VRest>
			bool check_value_pack( T const & x, T const & v1, VRest const & ... v_rest ) noexcept
			{
				return x==v1 || check_value_pack(x,v_rest...);
			}

			template <class Ex>
			bool check_exception_pack( error_info const & ei, Ex const * ) noexcept
			{
				return dynamic_cast<Ex const *>(ei.get_exception())!=0;
			}

			template <class Ex, class... ExRest>
			bool check_exception_pack( error_info const & ei, Ex const *, ExRest const * ... ex_rest ) noexcept
			{
				return dynamic_cast<Ex const *>(ei.get_exception())!=0 || check_exception_pack(ei, ex_rest...);
			}

			template <class SlotsTuple,class T>
			struct check_one_argument
			{
				static bool check( SlotsTuple const & tup, error_info const & ei ) noexcept
				{
					auto & sl = std::get<tuple_type_index<static_store_slot<T>,SlotsTuple>::value>(tup);
					return sl.has_value() && sl.value().e==ei.get_error();
				}
			};

			template <class SlotsTuple,class T>
			struct check_one_argument<SlotsTuple,T *>
			{
				static bool check( SlotsTuple const &, error_info const & ) noexcept
				{
					return true;
				}
			};

			template <class SlotsTuple, class E, typename match_type<E>::type... Value>
			struct check_one_argument<SlotsTuple,match<E,Value...>>
			{
				static bool check( SlotsTuple const & tup, error_info const & ei ) noexcept
				{
					auto & sl = std::get<tuple_type_index<static_store_slot<E>,SlotsTuple>::value>(tup);
					if( sl.has_value() )
					{
						auto const & v = sl.value();
						return v.e==ei.get_error() && check_value_pack(match_type<E>::get(v.v),Value...);
					}
					else
						return false;
				}
			};

			template <class SlotsTuple, class... Ex>
			struct check_one_argument<SlotsTuple,catch_<Ex...>>
			{
				static bool check( SlotsTuple const &, error_info const & ei ) noexcept
				{
					return check_exception_pack(ei,static_cast<Ex const *>(0)...);
				}
			};

			template <class SlotsTuple>
			struct check_one_argument<SlotsTuple,error_info>
			{
				static constexpr bool check( SlotsTuple const &, error_info const & )
				{
					return true;
				}
			};

			template <class SlotsTuple, class... List>
			struct check_arguments;

			template <class SlotsTuple, class Car, class... Cdr>
			struct check_arguments<SlotsTuple, Car, Cdr...>
			{
				static bool check( SlotsTuple const & tup, error_info const & ei ) noexcept
				{
					return check_one_argument<SlotsTuple,Car>::check(tup,ei) && check_arguments<SlotsTuple,Cdr...>::check(tup,ei);
				}
			};

			template <class SlotsTuple>
			struct check_arguments<SlotsTuple>
			{
				static constexpr bool check( SlotsTuple const &, error_info const & ) noexcept
				{
					return true;
				}
			};

			////////////////////////////////////////

			template <class T>
			struct get_one_argument
			{
				template <class StaticStore>
				static T const & get( StaticStore const & ss, error_info const & ei ) noexcept
				{
					T const * arg = ss.template peek<T>(ei.get_error());
					assert(arg!=0);
					return *arg;
				}
			};

			template <class T>
			struct get_one_argument<T const *>
			{
				template <class StaticStore>
				static T const * get( StaticStore const & ss, error_info const & ei ) noexcept
				{
					return ss.template peek<T>(ei.get_error());
				}
			};

			template <class E, typename match_type<E>::type... Value>
			struct get_one_argument<match<E,Value...>>
			{
				template <class StaticStore>
				static match<E,Value...> get( StaticStore const & ss, error_info const & ei ) noexcept
				{
					E const * arg = ss.template peek<E>(ei.get_error());
					assert(arg!=0);
					return match<E,Value...>{match_type<E>::get(*arg)};
				}
			};

			template <class... Ex>
			struct get_one_argument<catch_<Ex...>>
			{
				template <class StaticStore>
				static constexpr catch_<Ex...> get( StaticStore const &, error_info const & ) noexcept
				{
					return { };
				}
			};

			template <>
			struct get_one_argument<error_info>
			{
				template <class StaticStore>
				static error_info const & get( StaticStore const &, error_info const & ei ) noexcept
				{
					return ei;
				}
			};

			template <>
			struct get_one_argument<complete_diagnostic_info>
			{
				template <class StaticStore>
				static complete_diagnostic_info const & get( StaticStore const & ss, error_info const & ei ) noexcept
				{
					complete_diagnostic_info const * cdi = ss.template peek<complete_diagnostic_info>(ei.get_error());
					assert(cdi!=0);
					cdi->set_error_info(ei);
					return *cdi;
				}
			};

			////////////////////////////////////////

			template <class T> struct acceptable_last_handler_argument: std::false_type { };
			template <class T> struct acceptable_last_handler_argument<T const *>: is_error_type<T> { };
			template <> struct acceptable_last_handler_argument<error_info const &>: std::true_type { };
			template <> struct acceptable_last_handler_argument<unexpected_error_info const &>: std::true_type { };
			template <> struct acceptable_last_handler_argument<complete_diagnostic_info const &>: std::true_type { };

			template <class>
			struct ensure_last_handler_matches: std::false_type
			{
			};

			template <template<class...> class L, class Car, class... Cdr>
			struct ensure_last_handler_matches<L<Car,Cdr...>>
			{
				constexpr static bool value = acceptable_last_handler_argument<Car>::value && ensure_last_handler_matches<L<Cdr...>>::value;
			};

			template <template<class...> class L>
			struct ensure_last_handler_matches<L<>>: std::true_type
			{
			};
		}

		template <class... T>
		class dynamic_store_impl;

		template <class... E>
		class static_store
		{
			template <class... T>
			friend class dynamic_store_impl;

			static_store( static_store const & ) = delete;
			static_store & operator=( static_store const & ) = delete;

			std::tuple<static_store_internal::static_store_slot<E>...>  s_;
			bool reset_;

			template <class... T>
			bool check_handler( error_info const & ei, leaf_detail_mp11::mp_list<T...> ) const noexcept
			{
				using namespace static_store_internal;
				return check_arguments<decltype(s_),typename std::remove_cv<typename std::remove_reference<T>::type>::type...>::check(s_,ei);
			}

			template <class F,class... T>
			typename function_traits<F>::return_type call_handler( error_info const & ei, F && f, leaf_detail_mp11::mp_list<T...> ) const
			{
				using namespace static_store_internal;
				return std::forward<F>(f)( get_one_argument<typename std::remove_cv<typename std::remove_reference<T>::type>::type>::get(*this,ei)... );
			}

		public:

			constexpr explicit static_store() noexcept:
				reset_(false)
			{
			}

			~static_store() noexcept
			{
				if( reset_&& !std::uncaught_exception() )
					static_store_internal::tuple_for_each<sizeof...(E),decltype(s_)>::reset(s_);
			}

			void set_reset( bool r ) noexcept
			{
				reset_ = r;
			}

			template <class P>
			P const * peek( error const & e ) const noexcept
			{
				auto & opt = std::get<static_store_internal::type_index<P,E...>::value>(s_);
				if( opt.has_value() )
				{
					auto & x = opt.value();
					if( x.e==e )
						return &x.v;
				}
				return 0;
			}

			template <class F>
			typename function_traits<F>::return_type handle_error( error_info const & ei, F && f ) const
			{
				using namespace static_store_internal;
				static_assert(ensure_last_handler_matches<typename function_traits<F>::mp_args>::value,
					"The last handler for handle_all may only take arguments of type error_info const &, complete_diagnostic_info const &, unexpected_error_info const &, or any number of pointer-to-const arguments.");
				return call_handler( ei, std::forward<F>(f), typename function_traits<F>::mp_args{ } );
			}

			template <class CarF, class CdarF, class... CddrF>
			typename function_traits<CarF>::return_type handle_error( error_info const & ei, CarF && car_f, CdarF && cdar_f, CddrF && ... cddr_f ) const
			{
				using namespace static_store_internal;
				if( check_handler( ei, typename function_traits<CarF>::mp_args{ } ) )
					return call_handler( ei, std::forward<CarF>(car_f), typename function_traits<CarF>::mp_args{ } );
				else
					return handle_error( ei, std::forward<CdarF>(cdar_f), std::forward<CddrF>(cddr_f)...);
			}
		};

		//Static store deduction

		template <class T> struct translate_expect_deduction { typedef T type; };
		template <class T> struct translate_expect_deduction<T const> { typedef T type; };
		template <class T> struct translate_expect_deduction<T const &> { typedef T type; };
		template <class T> struct translate_expect_deduction<T const *> { typedef T type; };
		template <class E,typename match_type<E>::type... Value> struct translate_expect_deduction<match<E,Value...>> { typedef E type; };
		template <class... Exceptions> struct translate_expect_deduction<catch_<Exceptions...>> { typedef void type; };

		template <class... T>
		struct translate_list_impl;

		template <template<class...> class L, class... T>
		struct translate_list_impl<L<T...>>
		{
			using type = leaf_detail_mp11::mp_list<typename translate_expect_deduction<T>::type...>;
		};

		template <class... T> using translate_list = typename translate_list_impl<T...>::type;

		template <class T> struct does_not_participate_in_expect_deduction: std::false_type { };
		template <> struct does_not_participate_in_expect_deduction<error_info>: std::true_type { };
		template <> struct does_not_participate_in_expect_deduction<void>: std::true_type { };

		template <class... Handlers>
		struct handlers_args_set
		{
			using type =
				leaf_detail_mp11::mp_remove_if<
					leaf_detail_mp11::mp_unique<
						translate_list<
							leaf_detail_mp11::mp_append<
								typename function_traits<Handlers>::mp_args...
							>
						>
					>,
					does_not_participate_in_expect_deduction
				>;
		};

		template <class L>
		struct deduce_static_store;

		template <template<class...> class L, class... T>
		struct deduce_static_store<L<T...>>
		{
			typedef static_store<T...> type;
		};
	} //leaf_detail

} }

#endif
