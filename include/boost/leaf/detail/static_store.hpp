#ifndef BOOST_LEAF_AFBBD676B2FF11E8984C7976AE35F1A2
#define BOOST_LEAF_AFBBD676B2FF11E8984C7976AE35F1A2

// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/function_traits.hpp>
#include <boost/leaf/error.hpp>
#include <tuple>

namespace boost { namespace leaf {

	template <class R>
	struct is_result_type: std::false_type
	{
	};

	namespace leaf_detail
	{
		template <class R, bool IsResult = is_result_type<R>::value>
		struct result_tag;

		template <class R>
		struct result_tag<R, false>
		{
		};

		template <class R>
		struct result_tag<R, true>
		{
		};
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		class capturing_exception;

		class exception_info
		{
			exception_info( exception_info const & ) = delete;
			exception_info & operator=( exception_info const & ) = delete;

			std::exception const * const ex_;
			leaf_detail::capturing_exception const * const cap_;
			void (* const print_ex_)( std::ostream &, std::exception const *, leaf_detail::capturing_exception const * );

		public:

			exception_info( std::exception const * ex, leaf_detail::capturing_exception const * cap, void (*print_ex)(std::ostream &, std::exception const *, leaf_detail::capturing_exception const *) ) noexcept:
				ex_(ex),
				cap_(cap),
				print_ex_(print_ex)
			{
				assert(print_ex_!=0);
			}

			std::exception const * exception() const noexcept
			{
				return ex_;
			}

			void print( std::ostream & os ) const
			{
				print_ex_(os,ex_,cap_);
			}
		};
	}

	class error_info
	{
		error_info & operator=( error_info const & ) = delete;

		int const err_id_;
		std::error_code const & ec_;

		static int get_err_id( std::error_code const & ec ) noexcept
		{
			return is_error_id(ec) ? ec.value() : 0;
		}

	protected:

		error_info( error_info const & x ) noexcept = default;

		void print( std::ostream & os ) const
		{
			os << "Error ID: " << err_id_ << std::endl;
			if( ex_ )
				ex_->print(os);
			leaf_detail::slot_base::print(os,err_id_);
		}

	public:

		leaf_detail::exception_info const * const ex_;

		explicit error_info( error_id const & id ) noexcept:
			err_id_(id.value()),
			ec_(id),
			ex_(0)
		{
		}

		explicit error_info( std::error_code const & ec ) noexcept:
			err_id_(get_err_id(ec)),
			ec_(ec),
			ex_(0)
		{
		}

		error_info( error_id const & id, leaf_detail::exception_info const & ex ) noexcept:
			err_id_(id.value()),
			ec_(id),
			ex_(&ex)
		{
		}

		error_info( std::error_code const & ec, leaf_detail::exception_info const & ex ) noexcept:
			err_id_(get_err_id(ec)),
			ec_(ec),
			ex_(&ex)
		{
		}

		int err_id() const noexcept
		{
			return err_id_;
		}

		bool has_error() const noexcept
		{
			return err_id_!=0;
		}

		std::error_code const & error_code() const noexcept
		{
			return ec_;
		}

		error_id error() const noexcept
		{
			assert(has_error());
			return leaf_detail::make_error_id(err_id_);
		}

		bool has_exception() const noexcept
		{
			return ex_!=0;
		}

		std::exception const * exception() const noexcept
		{
			assert(has_exception());
			return ex_->exception();
		}

		friend std::ostream & operator<<( std::ostream & os, error_info const & x )
		{
			os << "leaf::error_info:" << std::endl;
			x.print(os);
			return os;
		}
	};

	////////////////////////////////////////

	class diagnostic_info: public error_info
	{
		leaf_detail::e_unexpected_count const * e_uc_;

	public:

		diagnostic_info( error_info const & ei, leaf_detail::e_unexpected_count const * e_uc ) noexcept:
			error_info(ei),
			e_uc_(e_uc)
		{
		}

		friend std::ostream & operator<<( std::ostream & os, diagnostic_info const & x )
		{
			os << "leaf::diagnostic_info:" << std::endl;
			x.print(os);
			if( x.e_uc_  )
				x.e_uc_->print(os);
			return os;
		}
	};

	class verbose_diagnostic_info: public error_info
	{
		leaf_detail::e_unexpected_info const * e_ui_;

	public:

		verbose_diagnostic_info( error_info const & ei, leaf_detail::e_unexpected_info const * e_ui ) noexcept:
			error_info(ei),
			e_ui_(e_ui)
		{
		}

		friend std::ostream & operator<<( std::ostream & os, verbose_diagnostic_info const & x )
		{
			os << "leaf::verbose_diagnostic_info:" << std::endl;
			x.print(os);
			if( x.e_ui_ )
				x.e_ui_->print(os);
			return os;
		}
	};

	////////////////////////////////////////

	namespace leaf_detail
	{
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
		};

		template <>
		class static_store_slot<e_unexpected_count>:
			public slot<e_unexpected_count>,
			enable_any
		{
		};

		template <>
		class static_store_slot<e_unexpected_info>:
			public slot<e_unexpected_info>,
			enable_any
		{
		};
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
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

		template <class E, class SlotsTuple>
		E const * peek( SlotsTuple const & tup, int err_id ) noexcept
		{
			if( auto pv = std::get<tuple_type_index<static_store_slot<E>,SlotsTuple>::value>(tup).has_value() )
				if( pv->err_id==err_id )
					return &pv->e;
			return 0;
		}
	}

	////////////////////////////////////////

	template <class E, class ErrorConditionEnum = E>
	struct condition
	{
	};

	namespace leaf_detail
	{
		template <class E, bool HasValue = has_data_member_value<E>::value>
		struct match_traits;

		template <class Enum>
		struct match_traits<Enum, false>
		{
			using enumerator = Enum;
			using e_type = enumerator;
			using match_type = enumerator;

			template <class SlotsTuple>
			static match_type const * read( SlotsTuple const & tup, error_info const & ei ) noexcept
			{
				return peek<e_type>(tup, ei.err_id());
			}
		};

		template <class E>
		struct match_traits<E, true>
		{
			using enumerator = decltype(E::value);
			using e_type = E;
			using match_type = enumerator;

			template <class SlotsTuple>
			static match_type const * read( SlotsTuple const & tup, error_info const & ei ) noexcept
			{
				if( auto pv = peek<e_type>(tup, ei.err_id()) )
					return &pv->value;
				else
					return 0;
			}
		};

		template <class ErrorConditionEnum>
		struct match_traits<condition<ErrorConditionEnum, ErrorConditionEnum>, false>
		{
			static_assert(std::is_error_condition_enum<ErrorConditionEnum>::value, "If leaf::condition is instantiated with one type, that type must be a std::error_condition_enum");

			using enumerator = ErrorConditionEnum;
			using e_type = e_original_ec;
			using match_type = std::error_code;

			template <class SlotsTuple>
			static match_type const * read( SlotsTuple const & tup, error_info const & ei ) noexcept
			{
				if( e_type const * ec = peek<e_type>(tup, ei.err_id()) )
					return &ec->value;
				else
					return &ei.error_code();
			}
		};

		template <class E, class ErrorConditionEnum>
		struct match_traits<condition<E, ErrorConditionEnum>, false>
		{
			static_assert(leaf_detail::has_data_member_value<E>::value, "If leaf::condition is instantiated with two types, the first one must have a member std::error_code value");
			static_assert(std::is_error_condition_enum<ErrorConditionEnum>::value, "If leaf::condition is instantiated with two types, the second one must be a std::error_condition_enum");

			using enumerator = ErrorConditionEnum;
			using e_type = E;
			using match_type = std::error_code;

			template <class SlotsTuple>
			static match_type const * read( SlotsTuple const & tup, error_info const & ei ) noexcept
			{
				if( auto pv = peek<e_type>(tup, ei.err_id()) )
					return &pv->value;
				else
					return 0;
			}
		};

		template <class MatchType, class Enumerator>
		bool check_value_pack( MatchType const & x, Enumerator v ) noexcept
		{
			return x==v;
		}

		template <class MatchType, class Enumerator, class... EnumeratorRest>
		bool check_value_pack( MatchType const & x, Enumerator v1, EnumeratorRest ... v_rest ) noexcept
		{
			return x==v1 || check_value_pack(x,v_rest...);
		}
	}

	template <class E, typename leaf_detail::match_traits<E>::enumerator... V>
	class match
	{
		using match_type = typename leaf_detail::match_traits<E>::match_type;
		match_type const * const value_;

	public:

		explicit match( match_type const * value ):
			value_(value)
		{
		}

		bool operator()() const noexcept
		{
			return value_ && leaf_detail::check_value_pack(*value_,V...);
		}

		match_type const & value() const noexcept
		{
			assert(value_!=0);
			return *value_;
		}
	};

	////////////////////////////////////////

	namespace leaf_detail
	{
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

	template <class... Ex>
	class catch_
	{
		std::exception const * const value_;

	public:

		explicit catch_( std::exception const * value ):
			value_(value)
		{
		}

		bool operator()() const noexcept
		{
			return value_ && leaf_detail::check_exception_pack(value_,static_cast<Ex const *>(0)...);
		}

		std::exception const & value() const noexcept
		{
			assert(value_!=0);
			return *value_;
		}
	};

	////////////////////////////////////////

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

			template <class SlotsTuple,class T>
			struct check_one_argument
			{
				static bool check( SlotsTuple const & tup, error_info const & ei ) noexcept
				{
					return peek<T>(tup, ei.err_id())!=0;
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

			template <class SlotsTuple>
			struct check_one_argument<SlotsTuple,error_info>
			{
				static constexpr bool check( SlotsTuple const &, error_info const & )
				{
					return true;
				}
			};

			template <class SlotsTuple>
			struct check_one_argument<SlotsTuple,diagnostic_info>
			{
				static bool check( SlotsTuple const &, error_info const & ) noexcept
				{
					return true;
				}
			};

			template <class SlotsTuple>
			struct check_one_argument<SlotsTuple,verbose_diagnostic_info>
			{
				static bool check( SlotsTuple const &, error_info const & ) noexcept
				{
					return true;
				}
			};

			template <class SlotsTuple>
			struct check_one_argument<SlotsTuple,std::error_code>
			{
				static bool check( SlotsTuple const &, error_info const & ) noexcept
				{
					return true;
				}
			};

			template <class SlotsTuple, class T, typename match_traits<T>::enumerator... V>
			struct check_one_argument<SlotsTuple,match<T,V...>>
			{
				static bool check( SlotsTuple const & tup, error_info const & ei ) noexcept
				{
					return match<T,V...>(match_traits<T>::read(tup,ei))();
				}
			};

			template <class SlotsTuple, class... Ex>
			struct check_one_argument<SlotsTuple,catch_<Ex...>>
			{
				static bool check( SlotsTuple const &, error_info const & ei ) noexcept
				{
					if( ei.has_exception() )
						return catch_<Ex...>(ei.exception())();
					else
						return false;
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
				template <class SlotsTuple>
				static T const & get( SlotsTuple const & tup, error_info const & ei ) noexcept
				{
					T const * arg = peek<T>(tup, ei.err_id());
					assert(arg!=0);
					return *arg;
				}
			};

			template <class T>
			struct get_one_argument<T const *>
			{
				template <class SlotsTuple>
				static T const * get( SlotsTuple const & tup, error_info const & ei ) noexcept
				{
					return peek<T>(tup, ei.err_id());
				}
			};

			template <>
			struct get_one_argument<error_info>
			{
				template <class SlotsTuple>
				static error_info const & get( SlotsTuple const &, error_info const & ei ) noexcept
				{
					return ei;
				}
			};

			template <>
			struct get_one_argument<diagnostic_info>
			{
				template <class SlotsTuple>
				static diagnostic_info get( SlotsTuple const & tup, error_info const & ei ) noexcept
				{
					return diagnostic_info(ei, peek<e_unexpected_count>(tup, ei.err_id()));
				}
			};

			template <>
			struct get_one_argument<verbose_diagnostic_info>
			{
				template <class SlotsTuple>
				static verbose_diagnostic_info get( SlotsTuple const & tup, error_info const & ei ) noexcept
				{
					return verbose_diagnostic_info(ei, peek<e_unexpected_info>(tup, ei.err_id()));
				}
			};

			template <>
			struct get_one_argument<std::error_code>
			{
				template <class SlotsTuple>
				static std::error_code const & get( SlotsTuple const & tup, error_info const & ei ) noexcept
				{
					if( leaf_detail::e_original_ec const * org = peek<e_original_ec>(tup, ei.err_id()) )
						return org->value;
					else
						return ei.error_code();
				}
			};

			template <class T, typename match_traits<T>::enumerator... V>
			struct get_one_argument<match<T,V...>>
			{
				template <class SlotsTuple>
				static match<T,V...> get( SlotsTuple const & tup, error_info const & ei ) noexcept
				{
					auto const * arg = match_traits<T>::read(tup, ei);
					assert(arg!=0);
					return match<T,V...>(arg);
				}
			};

			template <class... Ex>
			struct get_one_argument<catch_<Ex...>>
			{
				template <class SlotsTuple>
				static catch_<Ex...> get( SlotsTuple const &, error_info const & ei ) noexcept
				{
					std::exception const * ex = ei.exception();
					assert(ex!=0);
					return catch_<Ex...>(ex);
				}
			};

			////////////////////////////////////////

			template <class T> struct argument_matches_any_error: std::false_type { };
			template <class T> struct argument_matches_any_error<T const *>: is_e_type<T> { };
			template <> struct argument_matches_any_error<error_info const &>: std::true_type { };
			template <> struct argument_matches_any_error<diagnostic_info const &>: std::true_type { };
			template <> struct argument_matches_any_error<verbose_diagnostic_info const &>: std::true_type { };
			template <> struct argument_matches_any_error<std::error_code const &>: std::true_type { };

			template <class>
			struct handler_matches_any_error: std::false_type
			{
			};

			template <template<class...> class L, class Car, class... Cdr>
			struct handler_matches_any_error<L<Car,Cdr...>>
			{
				constexpr static bool value = argument_matches_any_error<Car>::value && handler_matches_any_error<L<Cdr...>>::value;
			};

			template <template<class...> class L>
			struct handler_matches_any_error<L<>>: std::true_type
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

			std::tuple<static_store_slot<E>...>  s_;
			bool reset_;

			template <class... T>
			bool check_handler( error_info const & ei, leaf_detail_mp11::mp_list<T...> ) const noexcept
			{
				using namespace static_store_internal;
				return check_arguments<decltype(s_),typename std::remove_cv<typename std::remove_reference<T>::type>::type...>::check(s_, ei);
			}

			template <class F,class... T>
			typename function_traits<F>::return_type call_handler( error_info const & ei, F && f, leaf_detail_mp11::mp_list<T...> ) const
			{
				using namespace static_store_internal;
				return std::forward<F>(f)( get_one_argument<typename std::remove_cv<typename std::remove_reference<T>::type>::type>::get(s_, ei)... );
			}

		public:

			using dynamic_store_impl_type = dynamic_store_impl<E...>;

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

			template <class F>
			typename function_traits<F>::return_type handle_error_( error_info const & ei, F && f ) const
			{
				using namespace static_store_internal;
				static_assert( handler_matches_any_error<typename function_traits<F>::mp_args>::value, "The last handler passed to handle_all must match any error." );
				return call_handler( ei, std::forward<F>(f), typename function_traits<F>::mp_args{ } );
			}

			template <class CarF, class... CdrF>
			typename function_traits<CarF>::return_type handle_error_( error_info const & ei, CarF && car_f, CdrF && ... cdr_f ) const
			{
				using namespace static_store_internal;
				if( handler_matches_any_error<typename function_traits<CarF>::mp_args>::value || check_handler( ei, typename function_traits<CarF>::mp_args{ } ) )
					return call_handler( ei, std::forward<CarF>(car_f), typename function_traits<CarF>::mp_args{ } );
				else
					return handle_error_( ei, std::forward<CdrF>(cdr_f)...);
			}
		};

		// Static store deduction / handle support

		template <class T> struct translate_type { using type = T; };
		template <class T> struct translate_type<T const> { using type = T; };
		template <class T> struct translate_type<T const *> { using type = T; };
		template <class T> struct translate_type<T const &> {using type = T; };

		template <> struct translate_type<diagnostic_info>;
		template <> struct translate_type<diagnostic_info const>;
		template <> struct translate_type<diagnostic_info const *>;
		template <> struct translate_type<diagnostic_info const &> { using type = e_unexpected_count; };

		template <> struct translate_type<verbose_diagnostic_info>;
		template <> struct translate_type<verbose_diagnostic_info const>;
		template <> struct translate_type<verbose_diagnostic_info const *>;
		template <> struct translate_type<verbose_diagnostic_info const &> { using type = e_unexpected_info; };

		template <> struct translate_type<std::error_code>;
		template <> struct translate_type<std::error_code const>;
		template <> struct translate_type<std::error_code const *>;
		template <> struct translate_type<std::error_code const &> { using type = leaf_detail::e_original_ec; };

		template <class T, typename match_traits<T>::enumerator... V> struct translate_type<match<T,V...>> { using type = typename match_traits<T>::e_type; };
		template <class T, typename match_traits<T>::enumerator... V> struct translate_type<match<T,V...> const>;
		template <class T, typename match_traits<T>::enumerator... V> struct translate_type<match<T,V...> const *>;
		template <class T, typename match_traits<T>::enumerator... V> struct translate_type<match<T,V...> const &>;

		template <class... Exceptions> struct translate_type<catch_<Exceptions...>> { using type = void; };
		template <class... Exceptions> struct translate_type<catch_<Exceptions...> const>;
		template <class... Exceptions> struct translate_type<catch_<Exceptions...> const *>;
		template <class... Exceptions> struct translate_type<catch_<Exceptions...> const &>;

		template <class... T>
		struct translate_list_impl;

		template <template<class...> class L, class... T>
		struct translate_list_impl<L<T...>>
		{
			using type = leaf_detail_mp11::mp_list<typename translate_type<T>::type...>;
		};

		template <class L> using translate_list = typename translate_list_impl<L>::type;

		////////////////////////////////////////

		template <class T> struct does_not_participate_in_expect_deduction: std::false_type { };
		template <> struct does_not_participate_in_expect_deduction<error_info>: std::true_type { };
		template <> struct does_not_participate_in_expect_deduction<std::error_code>: std::true_type { };
		template <> struct does_not_participate_in_expect_deduction<void>: std::true_type { };

		template <class L>
		struct transform_error_type_list_impl;

		template <template<class...> class L, class... T>
		struct transform_error_type_list_impl<L<T...>>
		{
			using type =
				leaf_detail_mp11::mp_remove_if<
					leaf_detail_mp11::mp_unique<
						translate_list<L<T...>>
					>,
					does_not_participate_in_expect_deduction
				>;
		};

		template <class L> using transform_error_type_list = typename transform_error_type_list_impl<L>::type;

		template <class... Handler>
		struct handler_args_set
		{
			using type = transform_error_type_list<
				leaf_detail_mp11::mp_append<
					typename function_traits<Handler>::mp_args...>>;
		};

		template <class HandlerList>
		struct handler_args_list;

		template <template <class...> class L, class... Handler>
		struct handler_args_list<L<Handler...>>
		{
			using type = transform_error_type_list<
				leaf_detail_mp11::mp_append<
					typename function_traits<Handler>::mp_args...>>;
		};

		template <class... E>
		struct error_type_set
		{
			using type = transform_error_type_list<
				leaf_detail_mp11::mp_list<E...>>;
		};

		template <class L>
		struct deduce_static_store;

		template <template<class...> class L, class... T>
		struct deduce_static_store<L<T...>>
		{
			using type = static_store<T...>;
		};

		////////////////////////////////////////

		template <class TryReturn, class Handler, class HandlerReturn = typename function_traits<Handler>::return_type, class HandlerArgs = typename function_traits<Handler>::mp_args>
		struct handler_wrapper;

		template <class TryReturn, class Handler, class HandlerReturn, template<class...> class L, class... A>
		struct handler_wrapper<TryReturn, Handler, HandlerReturn, L<A...>>
		{
			Handler h_;
			explicit handler_wrapper( Handler && h ) noexcept:
				h_(std::forward<Handler>(h))
			{
			}
			TryReturn operator()( A... a ) const
			{
				return h_(std::forward<A>(a)...);
			}
		};

		template <class TryReturn, class Handler, template<class...> class L, class... A>
		struct handler_wrapper<TryReturn, Handler, void, L<A...>>
		{
			Handler h_;
			explicit handler_wrapper( Handler && h ) noexcept:
				h_(std::forward<Handler>(h))
			{
			}
			TryReturn operator()( A... a ) const
			{
				h_(std::forward<A>(a)...);
				return { };
			}
		};

		////////////////////////////////////////

		template <class... Handler>
		struct handler_pack_return_impl;

		template <class CarH, class... CdrH>
		struct handler_pack_return_impl<CarH, CdrH...>
		{
			using type = typename function_traits<CarH>::return_type;
		};

		template <class... Handler>
		using handler_pack_return = typename handler_pack_return_impl<Handler...>::type;

		template <class... Handler>
		struct handler_result
		{
			using R = handler_pack_return<Handler...>;

			R r;

			R get()
			{
				return r;
			}
		};

		template <class... Handler>
		struct handler_result_void
		{
			void get()
			{
			}
		};

		template <class R, class... Handler>
		struct handle_error_dispatch_impl
		{
			using result_type = handler_result<Handler...>;

			template <class Error>
			static result_type handle( Error const & err, Handler && ... handler )
			{
				using namespace leaf_detail;
				return { reinterpret_cast<typename deduce_static_store<typename handler_args_set<Handler...>::type>::type const *>(err.ss_)->handle_error_(err, std::forward<Handler>(handler)...) };
			}

			template <class Error>
			static result_type handle_try_( Error const & err, Handler && ... handler )
			{
				using namespace leaf_detail;
				return { reinterpret_cast<typename deduce_static_store<typename handler_args_set<Handler...>::type>::type const *>(err.ss_)->handle_error_(err, std::forward<Handler>(handler)..., [ ]() -> R {throw;}) };
			}
		};

		template <class... Handler>
		struct handle_error_dispatch_impl<void, Handler...>
		{
			using result_type = handler_result_void<Handler...>;

			template <class Error>
			static result_type handle( Error const & err, Handler && ... handler )
			{
				using namespace leaf_detail;
				reinterpret_cast<typename deduce_static_store<typename handler_args_set<Handler...>::type>::type const *>(err.ss_)->handle_error_(err, std::forward<Handler>(handler)...);
				return { };
			}

			template <class Error>
			static result_type handle_try_( Error const & err, Handler && ... handler )
			{
				using namespace leaf_detail;
				reinterpret_cast<typename deduce_static_store<typename handler_args_set<Handler...>::type>::type const *>(err.ss_)->handle_error_(err, std::forward<Handler>(handler)..., [ ] {throw;});
				return { };
			}
		};

		template <class... Handler>
		using handle_error_dispatch = handle_error_dispatch_impl<handler_pack_return<Handler...>, Handler...>;

	} // leaf_detail

} }

#endif
