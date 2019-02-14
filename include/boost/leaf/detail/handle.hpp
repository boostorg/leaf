#ifndef BOOST_LEAF_AFBBD676B2FF11E8984C7976AE35F1A2
#define BOOST_LEAF_AFBBD676B2FF11E8984C7976AE35F1A2

// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/context.hpp>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		class exception_info_base
		{
		protected:

			explicit exception_info_base( std::exception const * ) noexcept;
			~exception_info_base() noexcept;

		public:

			std::exception const * const ex_;

			virtual void print( std::ostream & os ) const = 0;
		};

		class exception_info_;
	}

	class error_info
	{
		error_info & operator=( error_info const & ) = delete;

		bool has_error() const noexcept
		{
			return bool(err_id_);
		}

		bool has_error_code() const noexcept
		{
			return ec_!=0;
		}

	protected:

		void print( std::ostream & os ) const
		{
			os << "Error ID: " << err_id_.value() << std::endl;
			if( xi_ )
				xi_->print(os);
			leaf_detail::slot_base::print(os,err_id_.value());
		}

	public:

		polymorphic_context const & ctx_;
		leaf_detail::exception_info_base const * const xi_;
		std::error_code const * const ec_;
		error_id const err_id_;

		explicit error_info( polymorphic_context const & ctx, error_id const & id ) noexcept:
			ctx_(ctx),
			xi_(0),
			ec_(&id),
			err_id_(id)
		{
			assert(err_id_);
		}

		explicit error_info( polymorphic_context const & ctx, std::error_code const & ec ) noexcept:
			ctx_(ctx),
			xi_(0),
			ec_(&ec),
			err_id_(ec)
		{
		}

		explicit error_info( polymorphic_context const & ctx, leaf_detail::exception_info_ const & ) noexcept;

		explicit error_info( error_info const & x ) noexcept:
			ctx_(x.ctx_),
			xi_(x.xi_),
			ec_(x.ec_),
			err_id_(x.err_id_)
		{
		}

		int err_id() const noexcept
		{
			return err_id_.value();
		}

		error_id const & error() const noexcept
		{
			assert(has_error());
			return err_id_;
		}

		std::error_code const & error_code() const noexcept
		{
			assert(has_error());
			return ec_ ? *ec_ : err_id_;
		}

		bool exception_caught() const noexcept
		{
			return xi_!=0;
		}

		std::exception const * exception() const noexcept
		{
			assert(exception_caught());
			return xi_->ex_;
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
			return err_id ? std::get<tuple_type_index<slot<E>,SlotsTuple>::value>(tup).has_value(err_id) : 0;
		}
	}

	////////////////////////////////////////

	template <class E, class ErrorConditionEnum = E>
	struct condition;

	namespace leaf_detail
	{
		template <class Enum, bool HasValue = has_value<Enum>::value>
		struct match_traits
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
			static_assert(leaf_detail::has_value<E>::value, "If leaf::condition is instantiated with two types, the first one must have a member std::error_code value");
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

		//This specialization provided as a workaround for compile errors in g++ 4.9
		template <class ErrorConditionEnum>
		struct match_traits<condition<ErrorConditionEnum, ErrorConditionEnum>, true>
		{
			using enumerator = ErrorConditionEnum;
			using e_type = e_original_ec;
			using match_type = std::error_code;
		};

		//This specialization provided as a workaround for compile errors in g++ 4.9
		template <class E, class ErrorConditionEnum>
		struct match_traits<condition<E, ErrorConditionEnum>, true>
		{
			using enumerator = ErrorConditionEnum;
			using e_type = E;
			using match_type = std::error_code;
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

		explicit match( match_type const * value ) noexcept:
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

	namespace leaf_detail
	{
		template <class T, typename match_traits<T>::enumerator... V> struct translate_type_impl<match<T,V...>> { using type = typename match_traits<T>::e_type; };
		template <class T, typename match_traits<T>::enumerator... V> struct translate_type_impl<match<T,V...> const>;
		template <class T, typename match_traits<T>::enumerator... V> struct translate_type_impl<match<T,V...> const *>;
		template <class T, typename match_traits<T>::enumerator... V> struct translate_type_impl<match<T,V...> const &>;
	}

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

		explicit catch_( std::exception const * value ) noexcept:
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

	template <class Ex>
	class catch_<Ex>
	{
		Ex const * const value_;

	public:

		explicit catch_( std::exception const * value ) noexcept:
			value_(dynamic_cast<Ex const *>(value))
		{
		}

		bool operator()() const noexcept
		{
			return this->value_!=0;
		}

		Ex const & value() const noexcept
		{
			assert(this->value_!=0);
			return *this->value_;
		}
	};

	namespace leaf_detail
	{
		template <class... Exceptions> struct translate_type_impl<catch_<Exceptions...>> { using type = void; };
		template <class... Exceptions> struct translate_type_impl<catch_<Exceptions...> const>;
		template <class... Exceptions> struct translate_type_impl<catch_<Exceptions...> const *>;
		template <class... Exceptions> struct translate_type_impl<catch_<Exceptions...> const &>;
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
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
				if( ei.exception_caught() )
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
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
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
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
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

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class Tup, class... T>
		bool check_handler_( Tup const & e_objects, error_info const & ei, leaf_detail_mp11::mp_list<T...> ) noexcept
		{
			return check_arguments<Tup,typename std::remove_cv<typename std::remove_reference<T>::type>::type...>::check(e_objects, ei);
		}

		template <class R, class F, bool IsResult = is_result_type<R>::value, class FReturnType = fn_return_type<F>>
		struct handler_caller
		{
			template <class Tup, class... T>
			static R call( Tup const & e_objects, error_info const & ei, F && f, leaf_detail_mp11::mp_list<T...> )
			{
				return std::forward<F>(f)( get_one_argument<typename std::remove_cv<typename std::remove_reference<T>::type>::type>::get(e_objects, ei)... );
			}
		};

		template <template <class...> class Result, class... E, class F>
		struct handler_caller<Result<void, E...>, F, true, void>
		{
			using R = Result<void, E...>;

			template <class Tup,class... T>
			static R call( Tup const & e_objects, error_info const & ei, F && f, leaf_detail_mp11::mp_list<T...> )
			{
				std::forward<F>(f)( get_one_argument<typename std::remove_cv<typename std::remove_reference<T>::type>::type>::get(e_objects, ei)... );
				return { };
			}
		};

		template <class R, class Tup, class F>
		static R handle_error_( Tup const & e_objects, error_info const & ei, F && f )
		{
			static_assert( handler_matches_any_error<fn_mp_args<F>>::value, "The last handler passed to handle_all must match any error." );
			return handler_caller<R, F>::call( e_objects, ei, std::forward<F>(f), fn_mp_args<F>{ } );
		}

		template <class R, class Tup, class CarF, class... CdrF>
		static R handle_error_( Tup const & e_objects, error_info const & ei, CarF && car_f, CdrF && ... cdr_f )
		{
			if( handler_matches_any_error<fn_mp_args<CarF>>::value || check_handler_( e_objects, ei, fn_mp_args<CarF>{ } ) )
				return handler_caller<R, CarF>::call( e_objects, ei, std::forward<CarF>(car_f), fn_mp_args<CarF>{ } );
			else
				return handle_error_<R>( e_objects, ei, std::forward<CdrF>(cdr_f)...);
		}
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class... T>
		struct handler_pack_return_impl;

		template <class T>
		struct handler_pack_return_impl<T>
		{
			using type = T;
		};

		template <class Car, class... Cdr>
		struct handler_pack_return_impl<Car, Car, Cdr...>
		{
			using type = typename handler_pack_return_impl<Car, Cdr...>::type;
		};

		template <class... H>
		using handler_pack_return = typename handler_pack_return_impl<typename std::decay<fn_return_type<H>>::type...>::type;

		template <class... H>
		struct handler_result
		{
			using R = handler_pack_return<H...>;

			R r;

			R get() noexcept
			{
				return r;
			}
		};

		template <class... H>
		struct handler_result_void
		{
			void get() noexcept
			{
			}
		};
	}

} }

#endif
