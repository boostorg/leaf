#ifndef BOOST_LEAF_AFBBD676B2FF11E8984C7976AE35F1A2
#define BOOST_LEAF_AFBBD676B2FF11E8984C7976AE35F1A2

// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

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
		bool check_exception_pack( std::exception const * ex, Ex const * ) noexcept
		{
			return dynamic_cast<Ex const *>(ex)!=0;
		}

		template <class Ex, class... ExRest>
		bool check_exception_pack( std::exception const * ex, Ex const *, ExRest const * ... ex_rest ) noexcept
		{
			return dynamic_cast<Ex const *>(ex)!=0 || check_exception_pack(ex, ex_rest...);
		}
	}

	template <class E, typename leaf_detail::match_type<E>::type... V>
	struct match
	{
		using type = typename leaf_detail::match_type<E>::type;
		type const & value;

		explicit match( E const & e ):
			value(leaf_detail::match_type<E>::get(e))
		{
		}

		bool operator()() const noexcept
		{
			return leaf_detail::check_value_pack(value,V...);
		}
	};

	template <class... Ex>
	struct catch_
	{
		std::exception const & value;

		explicit catch_( std::exception const & ex ):
			value(ex)
		{
		}

		bool operator()() const noexcept
		{
			return leaf_detail::check_exception_pack(&value,static_cast<Ex const *>(0)...);
		}
	};

	template <class R>
	struct returned
	{
		R value;
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
				optional<E> extract_optional( error_id id ) && noexcept
				{
					slot<E> const & s = *this;
					if( s.has_value() && s.value().id==id )
						return optional<E>(std::move(*this).value().e);
					else
						return optional<E>();
				}
			};

			template <>
			class static_store_slot<diagnostic_info>:
				public slot<diagnostic_info>,
				enable_any
			{
			};

			template <>
			class static_store_slot<verbose_diagnostic_info>:
				public slot<verbose_diagnostic_info>,
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

			template <class SlotsTuple, class R, class T>
			struct check_one_argument
			{
				static bool check( SlotsTuple const & tup, error_info const & ei ) noexcept
				{
					auto & sl = std::get<tuple_type_index<static_store_slot<T>,SlotsTuple>::value>(tup);
					return sl.has_value() && sl.value().id==ei.error();
				}
			};

			template <class SlotsTuple, class R, class T>
			struct check_one_argument<SlotsTuple, R, T *>
			{
				static bool check( SlotsTuple const &, error_info const & ) noexcept
				{
					return true;
				}
			};

			template <class SlotsTuple, class R>
			struct check_one_argument<SlotsTuple, R, R>
			{
				static constexpr bool check( SlotsTuple const &, error_info const & )
				{
					return true;
				}
			};

			template <class SlotsTuple, class R>
			struct check_one_argument<SlotsTuple, R, error_info>
			{
				static constexpr bool check( SlotsTuple const &, error_info const & )
				{
					return true;
				}
			};

			template <class SlotsTuple, class R, class E, typename match_type<E>::type... V>
			struct check_one_argument<SlotsTuple, R, match<E,V...>>
			{
				static bool check( SlotsTuple const & tup, error_info const & ei ) noexcept
				{
					auto & sl = std::get<tuple_type_index<static_store_slot<E>,SlotsTuple>::value>(tup);
					if( sl.has_value() )
					{
						auto const & v = sl.value();
						return v.id==ei.error() && match<E,V...>(v.e)();
					}
					else
						return false;
				}
			};

			template <class SlotsTuple, class R, class... Ex>
			struct check_one_argument<SlotsTuple, R, catch_<Ex...>>
			{
				static bool check( SlotsTuple const &, error_info const & ei ) noexcept
				{
					if( std::exception const * ex = ei.exception() )
						return catch_<Ex...>(*ex)();
					else
						return false;
				}
			};

			template <class SlotsTuple, class R, class... List>
			struct check_arguments;

			template <class SlotsTuple, class R, class Car, class... Cdr>
			struct check_arguments<SlotsTuple, R, Car, Cdr...>
			{
				static bool check( SlotsTuple const & tup, error_info const & ei ) noexcept
				{
					return check_one_argument<SlotsTuple, R, Car>::check(tup,ei) && check_arguments<SlotsTuple, R, Cdr...>::check(tup,ei);
				}
			};

			template <class SlotsTuple, class R>
			struct check_arguments<SlotsTuple, R>
			{
				static constexpr bool check( SlotsTuple const &, error_info const & ) noexcept
				{
					return true;
				}
			};

			////////////////////////////////////////

			template <class R, class T>
			struct get_one_argument
			{
				template <class StaticStore>
				static T const & get( StaticStore const & ss, error_info const & ei, R * ) noexcept
				{
					T const * arg = ss.template peek<T>(ei.error());
					assert(arg!=0);
					return *arg;
				}
			};

			template <class R, class T>
			struct get_one_argument<R, T const *>
			{
				template <class StaticStore>
				static T const * get( StaticStore const & ss, error_info const & ei, R * ) noexcept
				{
					return ss.template peek<T>(ei.error());
				}
			};

			template <class R>
			struct get_one_argument<R,R>
			{
				template <class StaticStore>
				static R get( StaticStore const & ss, error_info const & ei, R * r ) noexcept
				{
					assert(r!=0);
					return *r;
				}
			};

			template <class R>
			struct get_one_argument<R, error_info>
			{
				template <class StaticStore>
				static error_info const & get( StaticStore const &, error_info const & ei, R * ) noexcept
				{
					return ei;
				}
			};

			template <class R>
			struct get_one_argument<R, diagnostic_info>
			{
				template <class StaticStore>
				static diagnostic_info const & get( StaticStore const & ss, error_info const & ei, R * ) noexcept
				{
					diagnostic_info const * uei = ss.template peek<diagnostic_info>(ei.error());
					assert(uei!=0);
					uei->set_error_info(ei);
					return *uei;
				}
			};

			template <class R>
			struct get_one_argument<R, verbose_diagnostic_info>
			{
				template <class StaticStore>
				static verbose_diagnostic_info const & get( StaticStore const & ss, error_info const & ei, R * ) noexcept
				{
					verbose_diagnostic_info const * vdi = ss.template peek<verbose_diagnostic_info>(ei.error());
					assert(vdi!=0);
					vdi->set_error_info(ei);
					return *vdi;
				}
			};

			template <class R, class E, typename match_type<E>::type... V>
			struct get_one_argument<R, match<E,V...>>
			{
				template <class StaticStore>
				static match<E,V...> get( StaticStore const & ss, error_info const & ei, R * ) noexcept
				{
					E const * arg = ss.template peek<E>(ei.error());
					assert(arg!=0);
					return match<E,V...>(*arg);
				}
			};

			template <class R, class... Ex>
			struct get_one_argument<R, catch_<Ex...>>
			{
				template <class StaticStore>
				static catch_<Ex...> get( StaticStore const &, error_info const & ei, R * ) noexcept
				{
					std::exception const * ex = ei.exception();
					assert(ex!=0);
					return catch_<Ex...>(*ex);
				}
			};

			////////////////////////////////////////

			template <class R, class T> struct argument_matches_any_error: std::false_type { };
			template <class R, class T> struct argument_matches_any_error<R, T const *>: is_error_type<T> { };
			template <class R> struct argument_matches_any_error<R, error_info>: std::true_type { };
			template <class R> struct argument_matches_any_error<R, diagnostic_info>: std::true_type { };
			template <class R> struct argument_matches_any_error<R, verbose_diagnostic_info>: std::true_type { };
			template <class R> struct argument_matches_any_error<R,R>: std::true_type { };

			template <class R, class>
			struct handler_matches_any_error: std::false_type
			{
			};

			template <class R, template<class...> class L, class Car, class... Cdr>
			struct handler_matches_any_error<R, L<Car,Cdr...>>
			{
				constexpr static bool value =
					argument_matches_any_error<R, typename std::remove_cv<typename std::remove_reference<Car>::type>::type>::value &&
					handler_matches_any_error<R, L<Cdr...>>::value;
			};

			template <class R, template<class...> class L>
			struct handler_matches_any_error<R, L<>>: std::true_type
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

			template <class R, class... T>
			bool check_handler( error_info const & ei, leaf_detail_mp11::mp_list<T...> ) const noexcept
			{
				using namespace static_store_internal;
				return check_arguments<decltype(s_), R, typename std::remove_cv<typename std::remove_reference<T>::type>::type...>::check(s_,ei);
			}

			template <class R, class F, class... T>
			typename function_traits<F>::return_type call_handler( error_info const & ei, R * r, F && f, leaf_detail_mp11::mp_list<T...> ) const
			{
				using namespace static_store_internal;
				return std::forward<F>(f)( get_one_argument<R, typename std::remove_cv<typename std::remove_reference<T>::type>::type>::get(*this, ei, r)... );
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
			P const * peek( error_id id ) const noexcept
			{
				auto & opt = std::get<static_store_internal::type_index<P,E...>::value>(s_);
				if( opt.has_value() )
				{
					auto & v = opt.value();
					if( v.id==id )
						return &v.e;
				}
				return 0;
			}

			template <class R, class F>
			typename function_traits<F>::return_type handle_error( error_info const & ei, R * r, F && f ) const
			{
				using namespace static_store_internal;
				assert(!ei.error());
				static_assert( handler_matches_any_error<R, typename function_traits<F>::mp_args>::value, "The last handler passed to handle_all must match any error." );
				return call_handler( ei, r, std::forward<F>(f), typename function_traits<F>::mp_args{ } );
			}

			template <class R, class CarF, class CdarF, class... CddrF>
			typename function_traits<CarF>::return_type handle_error( error_info const & ei, R * r, CarF && car_f, CdarF && cdar_f, CddrF && ... cddr_f ) const
			{
				using namespace static_store_internal;
				assert(!ei.error());
				if( handler_matches_any_error<R, typename function_traits<CarF>::mp_args>::value || check_handler<R>( ei, typename function_traits<CarF>::mp_args{ } ) )
					return call_handler( ei, r, std::forward<CarF>(car_f), typename function_traits<CarF>::mp_args{ } );
				else
					return handle_error( ei, r, std::forward<CdarF>(cdar_f), std::forward<CddrF>(cddr_f)...);
			}
		};

		// Static store deduction

		template <class T> struct translate_expect_deduction { typedef T type; };
		template <class T> struct translate_expect_deduction<T const> { typedef T type; };
		template <class T> struct translate_expect_deduction<T const &> { typedef T type; };
		template <class T> struct translate_expect_deduction<T const *> { typedef T type; };
		template <class E,typename match_type<E>::type... V> struct translate_expect_deduction<match<E,V...>> { typedef E type; };
		template <class... Exceptions> struct translate_expect_deduction<catch_<Exceptions...>> { typedef void type; };

		template <class... T>
		struct translate_list_impl;

		template <template<class...> class L, class... T>
		struct translate_list_impl<L<T...>>
		{
			using type = leaf_detail_mp11::mp_list<typename translate_expect_deduction<T>::type...>;
		};

		template <class... T> using translate_list = typename translate_list_impl<T...>::type;

		template <class R, class T> struct does_not_participate_in_expect_deduction: std::false_type { };
		template <class R> struct does_not_participate_in_expect_deduction<R, error_info>: std::true_type { };
		template <class R> struct does_not_participate_in_expect_deduction<R, R>: std::true_type { };
		template <class R> struct does_not_participate_in_expect_deduction<R, void>: std::true_type { };
		template <> struct does_not_participate_in_expect_deduction<void, void>: std::true_type { };

		template <class R, class... Handler>
		struct handler_args_set
		{
			template<class T> using to_remove = does_not_participate_in_expect_deduction<R, T>;

			using type =
				leaf_detail_mp11::mp_remove_if<
					leaf_detail_mp11::mp_unique<
						translate_list<
							leaf_detail_mp11::mp_append<
								typename function_traits<Handler>::mp_args...
							>
						>
					>,
					to_remove
				>;
		};

		template <class L>
		struct deduce_static_store;

		template <template<class...> class L, class... T>
		struct deduce_static_store<L<T...>>
		{
			typedef static_store<T...> type;
		};
	} // leaf_detail

} }

#endif
