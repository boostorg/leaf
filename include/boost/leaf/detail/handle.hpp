#ifndef LEAF_AFBBD676B2FF11E8984C7976AE35F1A2
#define LEAF_AFBBD676B2FF11E8984C7976AE35F1A2

// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/context.hpp>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		class exception_info_base
		{
		protected:

			LEAF_CONSTEXPR explicit exception_info_base( std::exception const * ) noexcept;
			~exception_info_base() noexcept;

		public:

			std::exception const * const ex_;

			virtual void print( std::ostream & os ) const = 0;
		};

		class exception_info_: public exception_info_base
		{
			exception_info_( exception_info_ const & ) = delete;
			exception_info_ & operator=( exception_info_ const & ) = delete;

			void print( std::ostream & os ) const final override;

		public:

			LEAF_CONSTEXPR explicit exception_info_( std::exception const * ex ) noexcept;
		};
	}

	class error_info
	{
		error_info & operator=( error_info const & ) = delete;

	protected:

		void print( std::ostream & os ) const
		{
			os << "Error ID = " << err_id_.value();
			if( xi_ )
				xi_->print(os);
		}

		LEAF_CONSTEXPR error_info( error_info  const & ) noexcept = default;

	public:

		void const * remote_handling_ctx_;
		leaf_detail::exception_info_base const * const xi_;
		error_id const err_id_;

		LEAF_CONSTEXPR explicit error_info( error_id id, void const * remote_handling_ctx = 0 ) noexcept:
			remote_handling_ctx_(remote_handling_ctx),
			xi_(0),
			err_id_(id)
		{
		}

		explicit error_info( leaf_detail::exception_info_ const &, void const * remote_handling_ctx = 0 ) noexcept;

		LEAF_CONSTEXPR error_id error() const noexcept
		{
			return err_id_;
		}

		LEAF_CONSTEXPR bool exception_caught() const noexcept
		{
			return xi_!=0;
		}

		LEAF_CONSTEXPR std::exception const * exception() const noexcept
		{
			assert(exception_caught());
			return xi_->ex_;
		}

		friend std::ostream & operator<<( std::ostream & os, error_info const & x )
		{
			os << "leaf::error_info: ";
			x.print(os);
			return os << '\n';
		}
	};

	////////////////////////////////////////

#if LEAF_DIAGNOSTICS

	class diagnostic_info: public error_info
	{
		leaf_detail::e_unexpected_count const * e_uc_;
		void const * tup_;
		void (*print_)( std::ostream &, void const * tup, int key_to_print );

	public:

		template <class Tup>
		LEAF_CONSTEXPR diagnostic_info( error_info const & ei, leaf_detail::e_unexpected_count const * e_uc, Tup const & tup ) noexcept:
			error_info(ei),
			e_uc_(e_uc),
			tup_(&tup),
			print_(&leaf_detail::tuple_for_each<std::tuple_size<Tup>::value, Tup>::print)
		{
		}

		friend std::ostream & operator<<( std::ostream & os, diagnostic_info const & x )
		{
			os << "leaf::diagnostic_info for ";
			x.print(os);
			os << ":\n";
			x.print_(os, x.tup_, x.err_id_.value());
			if( x.e_uc_  )
				x.e_uc_->print(os);
			return os;
		}
	};

	class verbose_diagnostic_info: public error_info
	{
		leaf_detail::e_unexpected_info const * e_ui_;
		void const * tup_;
		void (*print_)( std::ostream &, void const * tup, int key_to_print );

	public:

		template <class Tup>
		LEAF_CONSTEXPR verbose_diagnostic_info( error_info const & ei, leaf_detail::e_unexpected_info const * e_ui, Tup const & tup ) noexcept:
			error_info(ei),
			e_ui_(e_ui),
			tup_(&tup),
			print_(&leaf_detail::tuple_for_each<std::tuple_size<Tup>::value, Tup>::print)
		{
		}

		friend std::ostream & operator<<( std::ostream & os, verbose_diagnostic_info const & x )
		{
			os << "leaf::verbose_diagnostic_info for ";
			x.print(os);
			os << ":\n";
			x.print_(os, x.tup_, x.err_id_.value());
			if( x.e_ui_ )
				x.e_ui_->print(os);
			return os;
		}
	};

#else

	class diagnostic_info: public error_info
	{
	public:

		LEAF_CONSTEXPR diagnostic_info( error_info const & ei ) noexcept:
			error_info(ei)
		{
		}

		friend std::ostream & operator<<( std::ostream & os, diagnostic_info const & x )
		{
			os <<
				"leaf::diagnostic_info requires #define LEAF_DIAGNOSTICS 1\n"
				"leaf::error_info: ";
			x.print(os);
			return os << '\n';
		}
	};

	class verbose_diagnostic_info: public error_info
	{
	public:

		LEAF_CONSTEXPR verbose_diagnostic_info( error_info const & ei ) noexcept:
			error_info(ei)
		{
		}

		friend std::ostream & operator<<( std::ostream & os, verbose_diagnostic_info const & x )
		{
			os <<
				"leaf::verbose_diagnostic_info requires #define LEAF_DIAGNOSTICS 1\n"
				"leaf::error_info: ";
			x.print(os);
			return os << '\n';
		}
	};

#endif

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
		LEAF_CONSTEXPR inline E const * peek( SlotsTuple const & tup, error_id err ) noexcept
		{
			return err ? std::get<tuple_type_index<slot<E>,SlotsTuple>::value>(tup).has_value(err.value()) : 0;
		}
	}

	////////////////////////////////////////

	template <class E, class ErrorConditionEnum = E>
	struct condition
	{
	};

	namespace leaf_detail
	{
		template <class Enum, bool HasValue = has_value<Enum>::value>
		struct match_traits
		{
			using enumerator = Enum;
			using e_type = enumerator;
			using match_type = enumerator;

			template <class SlotsTuple>
			LEAF_CONSTEXPR static match_type const * read( SlotsTuple const & tup, error_info const & ei ) noexcept
			{
				return peek<e_type>(tup, ei.error());
			}
		};

		template <class E>
		struct match_traits<E, true>
		{
			using enumerator = decltype(E::value);
			using e_type = E;
			using match_type = enumerator;

			template <class SlotsTuple>
			LEAF_CONSTEXPR static match_type const * read( SlotsTuple const & tup, error_info const & ei ) noexcept
			{
				if( auto pv = peek<e_type>(tup, ei.error()) )
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
			LEAF_CONSTEXPR static match_type const * read( SlotsTuple const & tup, error_info const & ei ) noexcept
			{
				if( e_type const * ec = peek<e_type>(tup, ei.error()) )
					return &ec->value;
				else
					return 0;
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
			LEAF_CONSTEXPR static match_type const * read( SlotsTuple const & tup, error_info const & ei ) noexcept
			{
				if( auto pv = peek<e_type>(tup, ei.error()) )
					return &pv->value;
				else
					return 0;
			}
		};

		template <class MatchType, class Enumerator>
		LEAF_CONSTEXPR inline bool check_value_pack( MatchType const & x, Enumerator v ) noexcept
		{
			return x==v;
		}

		template <class MatchType, class Enumerator, class... EnumeratorRest>
		LEAF_CONSTEXPR inline bool check_value_pack( MatchType const & x, Enumerator v1, EnumeratorRest ... v_rest ) noexcept
		{
			return x==v1 || check_value_pack(x,v_rest...);
		}
	}

	template <class E, typename leaf_detail::match_traits<E>::enumerator... V>
	class match
	{
	public:
		using type = typename leaf_detail::match_traits<E>::match_type;

	private:
		type const * const value_;

	public:

		LEAF_CONSTEXPR explicit match( type const * value ) noexcept:
			value_(value)
		{
		}

		LEAF_CONSTEXPR bool operator()() const noexcept
		{
			return value_ && leaf_detail::check_value_pack(*value_,V...);
		}

		LEAF_CONSTEXPR type const & value() const noexcept
		{
			assert(value_!=0);
			return *value_;
		}
	};

	namespace leaf_detail
	{
		template <class T, typename match_traits<T>::enumerator... V> struct translate_type_impl<match<T,V...>> { using type = typename match_traits<T>::e_type; };
		template <class T, typename match_traits<T>::enumerator... V> struct translate_type_impl<match<T,V...> const>;
		template <class T, typename match_traits<T>::enumerator... V> struct translate_type_impl<match<T,V...> const *> { static_assert(sizeof(match<T,V...>)==0, "Handlers should take match<> by value, not as match<> const *"); };
		template <class T, typename match_traits<T>::enumerator... V> struct translate_type_impl<match<T,V...> const &> { static_assert(sizeof(match<T,V...>)==0, "Handlers should take match<> by value, not as match<> const &"); };
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class SlotsTuple,class T>
		struct check_one_argument
		{
			LEAF_CONSTEXPR static bool check( SlotsTuple const & tup, error_info const & ei ) noexcept
			{
				return peek<T>(tup, ei.error())!=0;
			}
		};

		template <class SlotsTuple,class T>
		struct check_one_argument<SlotsTuple,T *>
		{
			constexpr static bool check( SlotsTuple const &, error_info const & ) noexcept
			{
				return true;
			}
		};

		template <class SlotsTuple>
		struct check_one_argument<SlotsTuple,error_info>
		{
			constexpr static bool check( SlotsTuple const &, error_info const & )
			{
				return true;
			}
		};

		template <class SlotsTuple>
		struct check_one_argument<SlotsTuple,diagnostic_info>
		{
			constexpr static bool check( SlotsTuple const &, error_info const & ) noexcept
			{
				return true;
			}
		};

		template <class SlotsTuple>
		struct check_one_argument<SlotsTuple,verbose_diagnostic_info>
		{
			constexpr static bool check( SlotsTuple const &, error_info const & ) noexcept
			{
				return true;
			}
		};

		template <class SlotsTuple>
		struct check_one_argument<SlotsTuple,std::error_code>
		{
			constexpr static bool check( SlotsTuple const &, error_info const & ) noexcept
			{
				return true;
			}
		};

		template <class SlotsTuple, class T, typename match_traits<T>::enumerator... V>
		struct check_one_argument<SlotsTuple,match<T,V...>>
		{
			LEAF_CONSTEXPR static bool check( SlotsTuple const & tup, error_info const & ei ) noexcept
			{
				return match<T,V...>(match_traits<T>::read(tup,ei))();
			}
		};

		template <class SlotsTuple, class... List>
		struct check_arguments;

		template <class SlotsTuple, class Car, class... Cdr>
		struct check_arguments<SlotsTuple, Car, Cdr...>
		{
			LEAF_CONSTEXPR static bool check( SlotsTuple const & tup, error_info const & ei ) noexcept
			{
				return check_one_argument<SlotsTuple,Car>::check(tup,ei) && check_arguments<SlotsTuple,Cdr...>::check(tup,ei);
			}
		};

		template <class SlotsTuple>
		struct check_arguments<SlotsTuple>
		{
			constexpr static bool check( SlotsTuple const &, error_info const & ) noexcept
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
			LEAF_CONSTEXPR static T const & get( SlotsTuple const & tup, error_info const & ei ) noexcept
			{
				T const * arg = peek<T>(tup, ei.error());
				assert(arg!=0);
				return *arg;
			}
		};

		template <class T>
		struct get_one_argument<T const *>
		{
			template <class SlotsTuple>
			LEAF_CONSTEXPR static T const * get( SlotsTuple const & tup, error_info const & ei ) noexcept
			{
				return peek<T>(tup, ei.error());
			}
		};

		template <>
		struct get_one_argument<error_info>
		{
			template <class SlotsTuple>
			LEAF_CONSTEXPR static error_info const & get( SlotsTuple const &, error_info const & ei ) noexcept
			{
				return ei;
			}
		};

#if LEAF_DIAGNOSTICS

		template <>
		struct get_one_argument<diagnostic_info>
		{
			template <class SlotsTuple>
			LEAF_CONSTEXPR static diagnostic_info get( SlotsTuple const & tup, error_info const & ei ) noexcept
			{
				return diagnostic_info(ei, peek<e_unexpected_count>(tup, ei.error()), tup);
			}
		};

		template <>
		struct get_one_argument<verbose_diagnostic_info>
		{
			template <class SlotsTuple>
			LEAF_CONSTEXPR static verbose_diagnostic_info get( SlotsTuple const & tup, error_info const & ei ) noexcept
			{
				return verbose_diagnostic_info(ei, peek<e_unexpected_info>(tup, ei.error()), tup);
			}
		};

#else

		template <>
		struct get_one_argument<diagnostic_info>
		{
			template <class SlotsTuple>
			LEAF_CONSTEXPR static diagnostic_info get( SlotsTuple const & tup, error_info const & ei ) noexcept
			{
				return diagnostic_info(ei);
			}
		};

		template <>
		struct get_one_argument<verbose_diagnostic_info>
		{
			template <class SlotsTuple>
			LEAF_CONSTEXPR static verbose_diagnostic_info get( SlotsTuple const & tup, error_info const & ei ) noexcept
			{
				return verbose_diagnostic_info(ei);
			}
		};

#endif

		template <>
		struct get_one_argument<std::error_code>
		{
			template <class SlotsTuple>
			static std::error_code get( SlotsTuple const & tup, error_info const & ei ) noexcept
			{
				leaf_detail::e_original_ec const * org = peek<e_original_ec>(tup, ei.error());
				assert(org!=0);
				return org->value;
			}
		};

		template <class T, typename match_traits<T>::enumerator... V>
		struct get_one_argument<match<T,V...>>
		{
			template <class SlotsTuple>
			LEAF_CONSTEXPR static match<T,V...> get( SlotsTuple const & tup, error_info const & ei ) noexcept
			{
				auto const * arg = match_traits<T>::read(tup, ei);
				assert(arg!=0);
				return match<T,V...>(arg);
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
		LEAF_CONSTEXPR inline bool check_handler_( Tup const & e_objects, error_info const & ei, leaf_detail_mp11::mp_list<T...> ) noexcept
		{
			return check_arguments<Tup,typename std::remove_cv<typename std::remove_reference<T>::type>::type...>::check(e_objects, ei);
		}

		template <class R, class F, bool IsResult = is_result_type<R>::value, class FReturnType = fn_return_type<F>>
		struct handler_caller
		{
			template <class Tup, class... T>
			LEAF_CONSTEXPR static R call( Tup const & e_objects, error_info const & ei, F && f, leaf_detail_mp11::mp_list<T...> )
			{
				return std::forward<F>(f)( get_one_argument<typename std::remove_cv<typename std::remove_reference<T>::type>::type>::get(e_objects, ei)... );
			}
		};

		template <template <class...> class Result, class... E, class F>
		struct handler_caller<Result<void, E...>, F, true, void>
		{
			using R = Result<void, E...>;

			template <class Tup,class... T>
			LEAF_CONSTEXPR static R call( Tup const & e_objects, error_info const & ei, F && f, leaf_detail_mp11::mp_list<T...> )
			{
				std::forward<F>(f)( get_one_argument<typename std::remove_cv<typename std::remove_reference<T>::type>::type>::get(e_objects, ei)... );
				return { };
			}
		};

		template <class R, class Tup, class F>
		LEAF_CONSTEXPR inline R handle_error_( Tup const & e_objects, error_info const & ei, F && f )
		{
			static_assert( handler_matches_any_error<fn_mp_args<F>>::value, "The last handler passed to handle_all must match any error." );
			return handler_caller<R, F>::call( e_objects, ei, std::forward<F>(f), fn_mp_args<F>{ } );
		}

		template <class R, class Tup, class CarF, class... CdrF>
		LEAF_CONSTEXPR inline R handle_error_( Tup const & e_objects, error_info const & ei, CarF && car_f, CdrF && ... cdr_f )
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
		template <class T, template <class...> class R, class... E>
		struct add_result
		{
			using type = R<T, E...>;
		};

		template <class T, template <class...> class R, class... E>
		struct add_result<R<T, E...>, R, E...>
		{
			using type = R<T, E...>;
		};

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

		template <template <class...> class R, class... E, class Car, class... Cdr>
		struct handler_pack_return_impl<R<Car,E...>, Car, Cdr...>
		{
			using type = typename handler_pack_return_impl<R<Car,E...>, typename add_result<Cdr,R,E...>::type...>::type;
		};

		template <template <class...> class R, class... E, class Car, class... Cdr>
		struct handler_pack_return_impl<Car, R<Car,E...>, Cdr...>
		{
			using type = typename handler_pack_return_impl<R<Car,E...>, typename add_result<Cdr,R,E...>::type...>::type;
		};

		template <class... H>
		using handler_pack_return = typename handler_pack_return_impl<typename std::decay<fn_return_type<H>>::type...>::type;

		template <class... H>
		struct handler_result
		{
			using R = handler_pack_return<H...>;

			R r;

			LEAF_CONSTEXPR R get() noexcept
			{
				return std::move(r);
			}
		};

		template <class... H>
		struct handler_result_void
		{
			LEAF_CONSTEXPR void get() noexcept
			{
			}
		};
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class... E>
		template <class R, class... H>
		LEAF_CONSTEXPR inline typename std::decay<decltype(std::declval<R>().value())>::type context_base<E...>::handle_all_( R & r, H && ... h )
		{
			using Ret = typename std::decay<decltype(std::declval<R>().value())>::type;
			static_assert(is_result_type<R>::value, "The R type used with a handle_all function must be registered with leaf::is_result_type");
			assert(is_active());
			error_info const ei(r.error());
			deactivate();
			return handle_error_<Ret>(tup(), ei, std::forward<H>(h)...);
		}

		template <class... E>
		template <class R, class RemoteH>
		LEAF_CONSTEXPR inline typename std::decay<decltype(std::declval<R>().value())>::type context_base<E...>::remote_handle_all_( R & r, RemoteH && h )
		{
			static_assert(is_result_type<R>::value, "The R type used with a handle_all function must be registered with leaf::is_result_type");
			assert(is_active());
			error_info const ei(r.error(), this);
			deactivate();
			return std::forward<RemoteH>(h)(ei).get();
		}

		template <class... E>
		template <class R, class... H>
		LEAF_CONSTEXPR inline R context_base<E...>::handle_some_( R && r, H && ... h )
		{
			static_assert(is_result_type<R>::value, "The R type used with a handle_some function must be registered with leaf::is_result_type");
			assert(is_active());
			error_info const ei(r.error());
			deactivate();
			return handle_error_<R>(tup(), ei, std::forward<H>(h)...,
				[&r]()->R { return std::move(r); });
		}

		template <class... E>
		template <class R, class RemoteH>
		LEAF_CONSTEXPR inline R context_base<E...>::remote_handle_some_( R && r, RemoteH && h )
		{
			static_assert(is_result_type<R>::value, "The R type used with a handle_some function must be registered with leaf::is_result_type");
			assert(is_active());
			error_info const ei(r.error(), this);
			deactivate();
			return std::forward<RemoteH>(h)(ei).get();
		}

		template <class... E>
		template <class R, class... H>
		LEAF_CONSTEXPR inline typename std::decay<decltype(std::declval<R>().value())>::type context_base<E...>::handle_all( R & r, H && ... h )
		{
			auto active_context = activate_context(*this);
			return handle_all_(r, std::forward<H>(h)...);
		}

		template <class... E>
		template <class R, class RemoteH>
		LEAF_CONSTEXPR inline typename std::decay<decltype(std::declval<R>().value())>::type context_base<E...>::remote_handle_all( R & r, RemoteH && h )
		{
			auto active_context = activate_context(*this);
			return remote_handle_all_(r, std::forward<RemoteH>(h));
		}

		template <class... E>
		template <class R, class... H>
		LEAF_CONSTEXPR inline R context_base<E...>::handle_some( R && r, H && ... h )
		{
			auto active_context = activate_context(*this);
			return handle_some_(std::forward<R>(r), std::forward<H>(h)...);
		}

		template <class... E>
		template <class R, class RemoteH>
		LEAF_CONSTEXPR inline R context_base<E...>::remote_handle_some( R && r, RemoteH && h )
		{
			auto active_context = activate_context(*this);
			return remote_handle_some_(std::forward<R>(r), std::forward<RemoteH>(h));
		}
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class R, class... H>
		struct remote_error_dispatch_impl
		{
			using result_type = handler_result<H...>;

			//When can HH... be different than H...? It isn't, except that the _some
			//versions internally need to add a match-all handler at the end, which we
			//know is not going to affect the deduced context type Ctx.
			template <class... HH>
			LEAF_CONSTEXPR static result_type handle( error_info const & err, HH && ... h )
			{
				using Ctx = context_type_from_handlers<HH...>;
				using CtxHH = context_type_from_handlers<HH...>;
				static_assert(std::is_same<Ctx,CtxHH>::value, "Buggy context deduction logic");
				return { handle_error_<R>(static_cast<Ctx const *>(err.remote_handling_ctx_)->tup(), err, std::forward<HH>(h)...) };
			}
		};

		template <class... H>
		struct remote_error_dispatch_impl<void, H...>
		{
			using result_type = handler_result_void<H...>;

			//When can HH... be different than H...? It isn't, except that the _some
			//versions internally need to add a match-all handler at the end, which we
			//know is not going to affect the deduced context type Ctx.
			template <class... HH>
			LEAF_CONSTEXPR static result_type handle( error_info const & err, HH && ... h )
			{
				using Ctx = context_type_from_handlers<HH...>;
				using CtxHH = context_type_from_handlers<HH...>;
				static_assert(std::is_same<Ctx,CtxHH>::value, "Buggy context deduction logic");
				handle_error_<void>(static_cast<Ctx const *>(err.remote_handling_ctx_)->tup(), err, std::forward<HH>(h)...);
				return { };
			}
		};

		template <class... H>
		using remote_error_dispatch = remote_error_dispatch_impl<handler_pack_return<H...>, H...>;
	}

	template <class... H>
	LEAF_CONSTEXPR inline typename leaf_detail::remote_error_dispatch<H...>::result_type remote_handle_all( error_info const & err, H && ... h )
	{
		return leaf_detail::remote_error_dispatch<H...>::handle(err, std::forward<H>(h)... );
	}

	template <class... H>
	LEAF_CONSTEXPR inline typename leaf_detail::remote_error_dispatch<H...>::result_type remote_handle_some( error_info const & err, H && ... h )
	{
		using R = decltype(std::declval<typename leaf_detail::remote_error_dispatch<H...>::result_type>().get());
		return leaf_detail::remote_error_dispatch<H...>::handle(err, std::forward<H>(h)...,
			[&err]() -> R { return err.error(); } );
	}

	////////////////////////////////////////

	template <class TryBlock, class... H>
	LEAF_CONSTEXPR inline typename std::decay<decltype(std::declval<TryBlock>()().value())>::type try_handle_all( TryBlock && try_block, H && ... h )
	{
		// Creating a named temp on purpose, to avoid C++11 and C++14 zero-initializing the context.
		context_type_from_handlers<H...> c;
		return c.try_handle_all( std::forward<TryBlock>(try_block), std::forward<H>(h)... );
	}

	template <class TryBlock, class RemoteH>
	LEAF_CONSTEXPR inline typename std::decay<decltype(std::declval<TryBlock>()().value())>::type remote_try_handle_all( TryBlock && try_block, RemoteH && h )
	{
		// Creating a named temp on purpose, to avoid C++11 and C++14 zero-initializing the context.
		context_type_from_remote_handler<RemoteH> c;
		return c.remote_try_handle_all( std::forward<TryBlock>(try_block), std::forward<RemoteH>(h) );
	}

	template <class TryBlock, class... H>
	LEAF_CONSTEXPR inline typename std::decay<decltype(std::declval<TryBlock>()())>::type try_handle_some( TryBlock && try_block, H && ... h )
	{
		// Creating a named temp on purpose, to avoid C++11 and C++14 zero-initializing the context.
		context_type_from_handlers<H...> c;
		return c.try_handle_some( std::forward<TryBlock>(try_block), std::forward<H>(h)... );
	}

	template <class TryBlock, class RemoteH>
	LEAF_CONSTEXPR inline typename std::decay<decltype(std::declval<TryBlock>()())>::type remote_try_handle_some( TryBlock && try_block, RemoteH && h )
	{
		// Creating a named temp on purpose, to avoid C++11 and C++14 zero-initializing the context.
		context_type_from_remote_handler<RemoteH> c;
		return c.remote_try_handle_some( std::forward<TryBlock>(try_block), std::forward<RemoteH>(h) );
	}

} }

#endif
