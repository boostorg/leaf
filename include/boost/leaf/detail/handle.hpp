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

			explicit exception_info_base( std::exception const * ) noexcept;
			~exception_info_base() noexcept;

		public:

			std::exception const * const ex_;

			virtual void print( std::ostream & os ) const = 0;
		};

		class exception_info: public exception_info_base
		{
			exception_info( exception_info const & ) = delete;
			exception_info & operator=( exception_info const & ) = delete;

			void print( std::ostream & os ) const final override;

		public:

			explicit exception_info( std::exception const * ex ) noexcept;
		};
	}

	class error_info
	{
		error_info & operator=( error_info const & ) = delete;

	protected:

		void print( std::ostream & os ) const
		{
			os << "Error ID: " << err_id_.value() << std::endl;
			if( xi_ )
				xi_->print(os);
#ifndef LEAF_NO_DIAGNOSTIC_INFO
			leaf_detail::slot_base::print_all(os,err_id_.value());
#endif
		}

		error_info( error_info  const & ) noexcept = default;

	public:

		void const * remote_handling_ctx_;
		leaf_detail::exception_info_base const * const xi_;
		error_id const err_id_;

		explicit error_info( error_id id, void const * remote_handling_ctx = 0 ) noexcept:
			remote_handling_ctx_(remote_handling_ctx),
			xi_(0),
			err_id_(id)
		{
		}

		explicit error_info( leaf_detail::exception_info const &, void const * remote_handling_ctx = 0 ) noexcept;

		error_id error() const noexcept
		{
			return err_id_;
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

#ifdef LEAF_DISCARD_UNEXPECTED

	class diagnostic_info: public error_info
	{
	public:

		diagnostic_info( error_info const & ei ) noexcept:
			error_info(ei)
		{
		}

		friend std::ostream & operator<<( std::ostream & os, diagnostic_info const & x )
		{
			return os << "leaf::diagnostic_info not available due to LEAF_DISCARD_UNEXPECTED" << std::endl;
		}
	};

	class verbose_diagnostic_info: public error_info
	{
	public:

		verbose_diagnostic_info( error_info const & ei ) noexcept:
			error_info(ei)
		{
		}

		friend std::ostream & operator<<( std::ostream & os, verbose_diagnostic_info const & x )
		{
			return os << "leaf::verbose_diagnostic_info not available due to LEAF_DISCARD_UNEXPECTED" << std::endl;
		}
	};

#else

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
			os << "leaf::diagnostic_info:";
			if( x.err_id_ )
			{
				os << std::endl;
				x.print(os);
				if( x.e_uc_  )
					x.e_uc_->print(os);
			}
			else
				os << " {No Error}" << std::endl;
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
			os << "leaf::verbose_diagnostic_info:";
			if( x.err_id_ )
			{
				os << std::endl;
				x.print(os);
				if( x.e_ui_ )
					x.e_ui_->print(os);
			}
			else
				os << " {No Error}" << std::endl;
			return os;
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
		inline E const * peek( SlotsTuple const & tup, error_id err ) noexcept
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
			static match_type const * read( SlotsTuple const & tup, error_info const & ei ) noexcept
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
			static match_type const * read( SlotsTuple const & tup, error_info const & ei ) noexcept
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
			static match_type const * read( SlotsTuple const & tup, error_info const & ei ) noexcept
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
			static match_type const * read( SlotsTuple const & tup, error_info const & ei ) noexcept
			{
				if( auto pv = peek<e_type>(tup, ei.error()) )
					return &pv->value;
				else
					return 0;
			}
		};

		template <class MatchType, class Enumerator>
		inline bool check_value_pack( MatchType const & x, Enumerator v ) noexcept
		{
			return x==v;
		}

		template <class MatchType, class Enumerator, class... EnumeratorRest>
		inline bool check_value_pack( MatchType const & x, Enumerator v1, EnumeratorRest ... v_rest ) noexcept
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

		explicit match( type const * value ) noexcept:
			value_(value)
		{
		}

		bool operator()() const noexcept
		{
			return value_ && leaf_detail::check_value_pack(*value_,V...);
		}

		type const & value() const noexcept
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
			static bool check( SlotsTuple const & tup, error_info const & ei ) noexcept
			{
				return peek<T>(tup, ei.error())!=0;
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
				T const * arg = peek<T>(tup, ei.error());
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
				return peek<T>(tup, ei.error());
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

#ifdef LEAF_DISCARD_UNEXPECTED

		template <>
		struct get_one_argument<diagnostic_info>
		{
			template <class SlotsTuple>
			static diagnostic_info get( SlotsTuple const & tup, error_info const & ei ) noexcept
			{
				return diagnostic_info(ei);
			}
		};

		template <>
		struct get_one_argument<verbose_diagnostic_info>
		{
			template <class SlotsTuple>
			static verbose_diagnostic_info get( SlotsTuple const & tup, error_info const & ei ) noexcept
			{
				return verbose_diagnostic_info(ei);
			}
		};

#else

		template <>
		struct get_one_argument<diagnostic_info>
		{
			template <class SlotsTuple>
			static diagnostic_info get( SlotsTuple const & tup, error_info const & ei ) noexcept
			{
				return diagnostic_info(ei, peek<e_unexpected_count>(tup, ei.error()));
			}
		};

		template <>
		struct get_one_argument<verbose_diagnostic_info>
		{
			template <class SlotsTuple>
			static verbose_diagnostic_info get( SlotsTuple const & tup, error_info const & ei ) noexcept
			{
				return verbose_diagnostic_info(ei, peek<e_unexpected_info>(tup, ei.error()));
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
			static match<T,V...> get( SlotsTuple const & tup, error_info const & ei ) noexcept
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
		inline bool check_handler_( Tup const & e_objects, error_info const & ei, leaf_detail_mp11::mp_list<T...> ) noexcept
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
		inline R handle_error_( Tup const & e_objects, error_info const & ei, F && f )
		{
			static_assert( handler_matches_any_error<fn_mp_args<F>>::value, "The last handler passed to handle_all must match any error." );
			return handler_caller<R, F>::call( e_objects, ei, std::forward<F>(f), fn_mp_args<F>{ } );
		}

		template <class R, class Tup, class CarF, class... CdrF>
		inline R handle_error_( Tup const & e_objects, error_info const & ei, CarF && car_f, CdrF && ... cdr_f )
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

			R get() noexcept
			{
				return std::move(r);
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

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class... E>
		template <class R, class... H>
		inline typename std::decay<decltype(std::declval<R>().value())>::type context_base<E...>::handle_all( R & r, H && ... h ) const
		{
			using namespace leaf_detail;
			using Ret = typename std::decay<decltype(std::declval<R>().value())>::type;
			static_assert(is_result_type<R>::value, "The R type used with a handle_all function must be registered with leaf::is_result_type");
			return handle_error_<Ret>(tup(), error_info(r.error()), std::forward<H>(h)...);
		}

		template <class... E>
		template <class R, class RemoteH>
		inline typename std::decay<decltype(std::declval<R>().value())>::type context_base<E...>::remote_handle_all( R & r, RemoteH && h ) const
		{
			static_assert(is_result_type<R>::value, "The R type used with a handle_all function must be registered with leaf::is_result_type");
			return std::forward<RemoteH>(h)(error_info(r.error(), this)).get();
		}

		template <class... E>
		template <class R, class... H>
		inline R context_base<E...>::handle_some( R && r, H && ... h ) const
		{
			using namespace leaf_detail;
			static_assert(is_result_type<R>::value, "The R type used with a handle_some function must be registered with leaf::is_result_type");
			return handle_error_<R>(tup(), error_info(r.error()), std::forward<H>(h)...,
				[&r]()->R { return std::move(r); });
		}

		template <class... E>
		template <class R, class RemoteH>
		inline R context_base<E...>::remote_handle_some( R && r, RemoteH && h ) const
		{
			static_assert(is_result_type<R>::value, "The R type used with a handle_some function must be registered with leaf::is_result_type");
			return std::forward<RemoteH>(h)(error_info(r.error(), this)).get();
		}

		////////////////////////////////////////////

		template <class... E>
		template <class TryBlock, class... H>
		inline typename std::decay<decltype(std::declval<TryBlock>()().value())>::type context_base<E...>::try_handle_all_( TryBlock && try_block, H && ... h ) const
		{
			using namespace leaf_detail;
			static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The return type of the try_block passed to a try_handle_all function must be registered with leaf::is_result_type");
			assert(this->is_active());
			if( auto r = std::forward<TryBlock>(try_block)() )
				return r.value();
			else
				return handle_all(r, std::forward<H>(h)...);
		}

		template <class... E>
		template <class TryBlock, class RemoteH>
		inline typename std::decay<decltype(std::declval<TryBlock>()().value())>::type context_base<E...>::remote_try_handle_all_( TryBlock && try_block, RemoteH && h ) const
		{
			using namespace leaf_detail;
			static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The return type of the try_block passed to a remote_try_handle_all function must be registered with leaf::is_result_type");
			assert(this->is_active());
			if( auto r = std::forward<TryBlock>(try_block)() )
				return r.value();
			else
				return remote_handle_all(r, std::forward<RemoteH>(h));
		}

		template <class... E>
		template <class TryBlock, class... H, class Ctx>
		inline typename std::decay<decltype(std::declval<TryBlock>()())>::type context_base<E...>::try_handle_some_( context_activator<Ctx> & active_context, TryBlock && try_block, H && ... h ) const
		{
			using namespace leaf_detail;
			static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The return type of the try_block passed to a try_handle_some function must be registered with leaf::is_result_type");
			assert(this->is_active());
			if( auto r = std::forward<TryBlock>(try_block)() )
				return r;
			else
			{
				auto rr = handle_some(std::move(r), std::forward<H>(h)...);
				if( !rr )
					active_context.set_on_deactivate(on_deactivation::propagate);
				return rr;
			}
		}

		template <class... E>
		template <class TryBlock, class RemoteH, class Ctx>
		inline typename std::decay<decltype(std::declval<TryBlock>()())>::type context_base<E...>::remote_try_handle_some_( context_activator<Ctx> & active_context, TryBlock && try_block, RemoteH && h ) const
		{
			using namespace leaf_detail;
			static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The return type of the try_block passed to a remote_try_handle_some function must be registered with leaf::is_result_type");
			assert(this->is_active());
			if( auto r = std::forward<TryBlock>(try_block)() )
				return r;
			else
			{
				auto rr = remote_handle_some(std::move(r), std::forward<RemoteH>(h));
				if( !rr )
					active_context.set_on_deactivate(on_deactivation::propagate);
				return rr;
			}
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
			static result_type handle( error_info const & err, HH && ... h )
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
			static result_type handle( error_info const & err, HH && ... h )
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
	inline typename leaf_detail::remote_error_dispatch<H...>::result_type remote_handle_all( error_info const & err, H && ... h )
	{
		return leaf_detail::remote_error_dispatch<H...>::handle(err, std::forward<H>(h)... );
	}

	template <class... H>
	inline typename leaf_detail::remote_error_dispatch<H...>::result_type remote_handle_some( error_info const & err, H && ... h )
	{
		using R = decltype(std::declval<typename leaf_detail::remote_error_dispatch<H...>::result_type>().get());
		return leaf_detail::remote_error_dispatch<H...>::handle(err, std::forward<H>(h)...,
			[&err]() -> R { return err.error(); } );
	}

	////////////////////////////////////////

	template <class TryBlock, class... H>
	inline typename std::decay<decltype(std::declval<TryBlock>()().value())>::type try_handle_all( TryBlock && try_block, H && ... h )
	{
		// Creating a named temp on purpose, to avoid C++11 and C++14 zero-initializing the context.
		context_type_from_handlers<H...> c;
		return c.try_handle_all( std::forward<TryBlock>(try_block), std::forward<H>(h)... );
	}

	template <class TryBlock, class RemoteH>
	inline typename std::decay<decltype(std::declval<TryBlock>()().value())>::type remote_try_handle_all( TryBlock && try_block, RemoteH && h )
	{
		// Creating a named temp on purpose, to avoid C++11 and C++14 zero-initializing the context.
		context_type_from_remote_handler<RemoteH> c;
		return c.remote_try_handle_all( std::forward<TryBlock>(try_block), std::forward<RemoteH>(h) );
	}

	template <class TryBlock, class... H>
	inline typename std::decay<decltype(std::declval<TryBlock>()())>::type try_handle_some( TryBlock && try_block, H && ... h )
	{
		// Creating a named temp on purpose, to avoid C++11 and C++14 zero-initializing the context.
		context_type_from_handlers<H...> c;
		return c.try_handle_some( std::forward<TryBlock>(try_block), std::forward<H>(h)... );
	}

	template <class TryBlock, class RemoteH>
	inline typename std::decay<decltype(std::declval<TryBlock>()())>::type remote_try_handle_some( TryBlock && try_block, RemoteH && h )
	{
		// Creating a named temp on purpose, to avoid C++11 and C++14 zero-initializing the context.
		context_type_from_remote_handler<RemoteH> c;
		return c.remote_try_handle_some( std::forward<TryBlock>(try_block), std::forward<RemoteH>(h) );
	}

} }

#endif
