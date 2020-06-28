#ifndef BOOST_LEAF_HANDLE_ERROR_HPP_INCLUDED
#define BOOST_LEAF_HANDLE_ERROR_HPP_INCLUDED

// Copyright (c) 2018-2020 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if defined(__clang__)
#	pragma clang system_header
#elif (__GNUC__*100+__GNUC_MINOR__>301) && !defined(BOOST_LEAF_ENABLE_WARNINGS)
#	pragma GCC system_header
#elif defined(_MSC_VER) && !defined(BOOST_LEAF_ENABLE_WARNINGS)
#	pragma warning(push,1)
#endif

#include <boost/leaf/context.hpp>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		class exception_info_base
		{
		protected:

			BOOST_LEAF_CONSTEXPR explicit exception_info_base( std::exception * ) noexcept;
			~exception_info_base() noexcept;

		public:

			std::exception * const ex_;

			virtual void print( std::ostream & os ) const = 0;
		};

		class exception_info_: public exception_info_base
		{
			exception_info_( exception_info_ const & ) = delete;
			exception_info_ & operator=( exception_info_ const & ) = delete;

			void print( std::ostream & os ) const final override;

		public:

			BOOST_LEAF_CONSTEXPR explicit exception_info_( std::exception * ex ) noexcept;
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

		BOOST_LEAF_CONSTEXPR error_info( error_info  const & ) noexcept = default;

	public:

		leaf_detail::exception_info_base const * const xi_;
		error_id const err_id_;

		BOOST_LEAF_CONSTEXPR explicit error_info( error_id id ) noexcept:
			xi_(0),
			err_id_(id)
		{
		}

		explicit error_info( leaf_detail::exception_info_ const & ) noexcept;

		BOOST_LEAF_CONSTEXPR error_id error() const noexcept
		{
			return err_id_;
		}

		BOOST_LEAF_CONSTEXPR bool exception_caught() const noexcept
		{
			return xi_!=0;
		}

		BOOST_LEAF_CONSTEXPR std::exception * exception() const noexcept
		{
			BOOST_LEAF_ASSERT(exception_caught());
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

#if BOOST_LEAF_DIAGNOSTICS

	class diagnostic_info: public error_info
	{
		leaf_detail::e_unexpected_count const * e_uc_;
		void const * tup_;
		void (*print_)( std::ostream &, void const * tup, int key_to_print );

	public:

		template <class Tup>
		BOOST_LEAF_CONSTEXPR diagnostic_info( error_info const & ei, leaf_detail::e_unexpected_count const * e_uc, Tup const & tup ) noexcept:
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
		BOOST_LEAF_CONSTEXPR verbose_diagnostic_info( error_info const & ei, leaf_detail::e_unexpected_info const * e_ui, Tup const & tup ) noexcept:
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

		BOOST_LEAF_CONSTEXPR diagnostic_info( error_info const & ei ) noexcept:
			error_info(ei)
		{
		}

		friend std::ostream & operator<<( std::ostream & os, diagnostic_info const & x )
		{
			os <<
				"leaf::diagnostic_info requires #define BOOST_LEAF_DIAGNOSTICS 1\n"
				"leaf::error_info: ";
			x.print(os);
			return os << '\n';
		}
	};

	class verbose_diagnostic_info: public error_info
	{
	public:

		BOOST_LEAF_CONSTEXPR verbose_diagnostic_info( error_info const & ei ) noexcept:
			error_info(ei)
		{
		}

		friend std::ostream & operator<<( std::ostream & os, verbose_diagnostic_info const & x )
		{
			os <<
				"leaf::verbose_diagnostic_info requires #define BOOST_LEAF_DIAGNOSTICS 1\n"
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
			constexpr static int value = 0;
		};

		template <class T, class Car, class... Cdr>
		struct type_index<T, Car, Cdr...>
		{
			constexpr static int value = 1 + type_index<T,Cdr...>::value;
		};

		template <class T, class Tuple>
		struct tuple_type_index;

		template <class T, class... TupleTypes>
		struct tuple_type_index<T,std::tuple<TupleTypes...>>
		{
			constexpr static int value = type_index<T,TupleTypes...>::value;
		};

		template <class E, bool = handler_argument_traits<E>::requires_catch>
		struct peek_exception;

		template <>
		struct peek_exception<std::exception, true>
		{
			BOOST_LEAF_CONSTEXPR static std::exception * peek( error_info const & ei ) noexcept
			{
				return ei.exception_caught() ? ei.exception() : 0;
			}
		};

		template <class E>
		struct peek_exception<E, true>
		{
			BOOST_LEAF_CONSTEXPR static E * peek( error_info const & ei ) noexcept
			{
				return ei.exception_caught() ? dynamic_cast<E *>(ei.exception()) : 0;
			}
		};

		template <class E>
		struct peek_exception<E, false>
		{
			BOOST_LEAF_CONSTEXPR static E * peek( error_info const & ) noexcept
			{
				return 0;
			}
		};

		template <class E, class SlotsTuple>
		BOOST_LEAF_CONSTEXPR inline E const * peek( SlotsTuple const & tup, error_info const & ei ) noexcept
		{
			if( error_id err = ei.error() )
				if( E const * e = std::get<tuple_type_index<slot<E>,SlotsTuple>::value>(tup).has_value(err.value()) )
					return e;
				else
					return peek_exception<E const>::peek(ei);
			else
				return 0;
		}

		template <class E, class SlotsTuple>
		BOOST_LEAF_CONSTEXPR inline E * peek( SlotsTuple & tup, error_info const & ei ) noexcept
		{
			if( error_id err = ei.error() )
				if( E * e = std::get<tuple_type_index<slot<E>,SlotsTuple>::value>(tup).has_value(err.value()) )
					return e;
				else
					return peek_exception<E>::peek(ei);
			else
				return 0;
		}
	}

	////////////////////////////////////////

	template <class ErrorCodeEnum>
	BOOST_LEAF_CONSTEXPR inline bool category( std::error_code const & ec ) noexcept
	{
		static_assert(std::is_error_code_enum<ErrorCodeEnum>::value, "leaf::category requires an error code enum");
		return &ec.category() == &std::error_code(ErrorCodeEnum{}).category();
	}

	template <class E, class EnumType = E>
	struct condition
	{
		using enum_type = EnumType;
		static_assert(std::is_error_condition_enum<enum_type>::value || std::is_error_code_enum<enum_type>::value, "leaf::condition requires the enum to be registered either with std::is_error_condition or std::is_error_code.");
	};

	namespace leaf_detail
	{
		template <class T> using has_member_value_impl = decltype( std::declval<T>().value );
		template <class T> using has_member_value_fn_impl = decltype( std::declval<T>().value() );

		template <class T>
		struct has_member_value
		{
			enum { value = leaf_detail_mp11::mp_valid<has_member_value_impl, T>::value || leaf_detail_mp11::mp_valid<has_member_value_fn_impl, T>::value };
		};

		template <class Enum>
		struct match_traits<Enum, false>
		{
			using enum_type = Enum;
			using error_type = Enum;
			using match_type = Enum;

			BOOST_LEAF_CONSTEXPR static enum_type get_value( match_type x ) noexcept
			{
				return x;
			}
		};

		template <class E, bool = leaf_detail_mp11::mp_valid<has_member_value_fn_impl, E>::value>
		struct match_traits_value;

		template <class E>
		struct match_traits_value<E, false>
		{
			using enum_type = decltype(std::declval<E>().value);
			using error_type = E;
			using match_type = error_type const &;

			BOOST_LEAF_CONSTEXPR static enum_type const & get_value( match_type x ) noexcept
			{
				return x.value;
			}
		};

		template <class E>
		struct match_traits_value<E, true>
		{
			using enum_type = typename std::remove_reference<decltype(std::declval<E>().value())>::type;
			using error_type = E;
			using match_type = error_type const &;

			BOOST_LEAF_CONSTEXPR static decltype(std::declval<match_type>().value()) get_value( match_type x ) noexcept
			{
				return x.value();
			}
		};

		template <class E>
		struct match_traits<E, true>: match_traits_value<E>
		{
		};

		template <class EnumType>
		struct match_traits<condition<EnumType, EnumType>, false>
		{
			using enum_type = EnumType;
			using error_type = std::error_code;
			using match_type = error_type const &;

			BOOST_LEAF_CONSTEXPR static match_type get_value( match_type x ) noexcept
			{
				return x;
			}
		};

		template <class E, class EnumType>
		struct match_traits<condition<E, EnumType>, false>
		{
			using enum_type = EnumType;
			using error_type = E;
			using match_type = error_type const &;

			BOOST_LEAF_CONSTEXPR static std::error_code const & get_value( match_type x ) noexcept
			{
				return x.value;
			}
		};

		template <>
		struct match_traits<std::error_code, true>
		{
			using enum_type = void;
			using error_type = std::error_code;
			using match_type = error_type const &;

			BOOST_LEAF_CONSTEXPR static match_type get_value( match_type x ) noexcept
			{
				return x;
			}
		};

		// Use match<std::error_code, ...> with an error condition enum to match a specific error condition.
		// This type intentionally left undefined to detect such bugs.
		template <>
		struct match_traits<std::error_condition, true>;

		template <class MatchType>
		inline bool check_value_pack( MatchType const & x, bool (*pred)(MatchType const &) noexcept ) noexcept
		{
			BOOST_LEAF_ASSERT(pred != 0);
			return pred(x);
		}

		template <class MatchType, class V>
		inline bool check_value_pack( MatchType const & x, V v ) noexcept
		{
			return x == v;
		}

		template <class MatchType, class VCar, class... VCdr>
		inline bool check_value_pack( MatchType const & x, VCar car, VCdr ... cdr ) noexcept
		{
			return check_value_pack(x, car) || check_value_pack(x, cdr...);
		}
	}

#if __cplusplus >= 201703L
	template <class E, auto V1, auto... V>
#else
	template <class E, typename leaf_detail::match_traits<E>::enum_type V1, typename leaf_detail::match_traits<E>::enum_type... V>
#endif
	class match
	{
	public:

		using match_type = typename leaf_detail::match_traits<E>::match_type;

	private:

		match_type m_;

	public:

		BOOST_LEAF_CONSTEXPR explicit match( match_type m ) noexcept:
			m_(m)
		{
		}

		BOOST_LEAF_CONSTEXPR explicit operator bool() const noexcept
		{
			return leaf_detail::check_value_pack(leaf_detail::match_traits<E>::get_value(this->matched()), V1, V...);
		}

		BOOST_LEAF_CONSTEXPR match_type matched() const noexcept
		{
			return m_;
		}
	};

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class Tup>
		BOOST_LEAF_CONSTEXPR diagnostic_info handler_argument_traits<diagnostic_info>::get( Tup const & tup, error_info const & ei ) noexcept
		{
#if BOOST_LEAF_DIAGNOSTICS
			return diagnostic_info(ei, handler_argument_traits_defaults<e_unexpected_count>::check(tup, ei), tup);
#else
			return diagnostic_info(ei);
#endif
		}

		template <class Tup>
		BOOST_LEAF_CONSTEXPR verbose_diagnostic_info handler_argument_traits<verbose_diagnostic_info>::get( Tup const & tup, error_info const & ei ) noexcept
		{
#if BOOST_LEAF_DIAGNOSTICS
			return verbose_diagnostic_info(ei, handler_argument_traits_defaults<e_unexpected_info>::check(tup, ei), tup);
#else
			return verbose_diagnostic_info(ei);
#endif
		}

		template <class A, bool RequiresCatch>
		template <class Tup>
		BOOST_LEAF_CONSTEXPR typename handler_argument_traits_defaults<A, RequiresCatch>::error_type const * handler_argument_traits_defaults<A, RequiresCatch>::check( Tup const & tup, error_info const & ei ) noexcept
		{
			return peek<typename std::decay<A>::type>(tup, ei);
		}

		template <class A, bool RequiresCatch>
		template <class Tup>
		BOOST_LEAF_CONSTEXPR typename handler_argument_traits_defaults<A, RequiresCatch>::error_type * handler_argument_traits_defaults<A, RequiresCatch>::check( Tup & tup, error_info const & ei ) noexcept
		{
			return peek<typename std::decay<A>::type>(tup, ei);
		}

		template <class Tup, class... List>
		struct check_arguments;

		template <class Tup>
		struct check_arguments<Tup>
		{
			BOOST_LEAF_CONSTEXPR static bool check( Tup const &, error_info const & )
			{
				return true;
			}
		};

		template <class Tup, class Car, class... Cdr>
		struct check_arguments<Tup, Car, Cdr...>
		{
			BOOST_LEAF_CONSTEXPR static bool check( Tup & tup, error_info const & ei ) noexcept
			{
				return handler_argument_traits<Car>::check(tup, ei) && check_arguments<Tup, Cdr...>::check(tup, ei);
			}
		};
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class>
		struct handler_matches_any_error: std::false_type
		{
		};

		template <template<class...> class L>
		struct handler_matches_any_error<L<>>: std::true_type
		{
		};

		template <template<class...> class L, class Car, class... Cdr>
		struct handler_matches_any_error<L<Car, Cdr...>>
		{
			constexpr static bool value = handler_argument_traits<Car>::always_available && handler_matches_any_error<L<Cdr...>>::value;
		};
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class Tup, class... A>
		BOOST_LEAF_CONSTEXPR inline bool check_handler_( Tup & tup, error_info const & ei, leaf_detail_mp11::mp_list<A...> ) noexcept
		{
			return check_arguments<Tup, A...>::check(tup, ei);
		}

		template <class R, class F, bool IsResult = is_result_type<R>::value, class FReturnType = fn_return_type<F>>
		struct handler_caller
		{
			template <class Tup, class... A>
			BOOST_LEAF_CONSTEXPR static R call( Tup & tup, error_info const & ei, F && f, leaf_detail_mp11::mp_list<A...> )
			{
				return std::forward<F>(f)( handler_argument_traits<A>::get(tup, ei)... );
			}
		};

		template <template <class...> class Result, class... E, class F>
		struct handler_caller<Result<void, E...>, F, true, void>
		{
			using R = Result<void, E...>;

			template <class Tup, class... A>
			BOOST_LEAF_CONSTEXPR static R call( Tup & tup, error_info const & ei, F && f, leaf_detail_mp11::mp_list<A...> )
			{
				std::forward<F>(f)( handler_argument_traits<A>::get(tup, ei)... );
				return { };
			}
		};

		template <class T>
		struct is_tuple: std::false_type { };

		template <class... T>
		struct is_tuple<std::tuple<T...>>: std::true_type { };

		template <class... T>
		struct is_tuple<std::tuple<T...> &>: std::true_type { };

		template <class R, class Tup, class H>
		BOOST_LEAF_CONSTEXPR inline typename std::enable_if<!is_tuple<H>::value, R>::type handle_error_( Tup & tup, error_info const & ei, H && h )
		{
			static_assert( handler_matches_any_error<fn_mp_args<H>>::value, "The last handler passed to handle_all must match any error." );
			return handler_caller<R, H>::call( tup, ei, std::forward<H>(h), fn_mp_args<H>{ } );
		}

		template <class R, class Tup, class Car, class... Cdr>
		BOOST_LEAF_CONSTEXPR inline typename std::enable_if<!is_tuple<Car>::value, R>::type handle_error_( Tup & tup, error_info const & ei, Car && car, Cdr && ... cdr )
		{
			if( handler_matches_any_error<fn_mp_args<Car>>::value || check_handler_( tup, ei, fn_mp_args<Car>{ } ) )
				return handler_caller<R, Car>::call( tup, ei, std::forward<Car>(car), fn_mp_args<Car>{ } );
			else
				return handle_error_<R>( tup, ei, std::forward<Cdr>(cdr)...);
		}

		template <class R, class Tup, class HTup, size_t ... I>
		BOOST_LEAF_CONSTEXPR inline R handle_error_tuple_( Tup & tup, error_info const & ei, leaf_detail_mp11::index_sequence<I...>, HTup && htup )
		{
			return handle_error_<R>(tup, ei, std::get<I>(std::forward<HTup>(htup))...);
		}

		template <class R, class Tup, class HTup, class... Cdr, size_t ... I>
		BOOST_LEAF_CONSTEXPR inline R handle_error_tuple_( Tup & tup, error_info const & ei, leaf_detail_mp11::index_sequence<I...>, HTup && htup, Cdr && ... cdr )
		{
			return handle_error_<R>(tup, ei, std::get<I>(std::forward<HTup>(htup))..., std::forward<Cdr>(cdr)...);
		}

		template <class R, class Tup, class H>
		BOOST_LEAF_CONSTEXPR inline typename std::enable_if<is_tuple<H>::value, R>::type handle_error_( Tup & tup, error_info const & ei, H && h )
		{
			return handle_error_tuple_<R>(
				tup,
				ei,
				leaf_detail_mp11::make_index_sequence<std::tuple_size<typename std::decay<H>::type>::value>(),
				std::forward<H>(h));
		}

		template <class R, class Tup, class Car, class... Cdr>
		BOOST_LEAF_CONSTEXPR inline typename std::enable_if<is_tuple<Car>::value, R>::type handle_error_( Tup & tup, error_info const & ei, Car && car, Cdr && ... cdr )
		{
			return handle_error_tuple_<R>(
				tup,
				ei,
				leaf_detail_mp11::make_index_sequence<std::tuple_size<typename std::decay<Car>::type>::value>(),
				std::forward<Car>(car),
				std::forward<Cdr>(cdr)...);
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

			BOOST_LEAF_CONSTEXPR R get() noexcept
			{
				return std::move(r);
			}
		};

		template <class... H>
		struct handler_result_void
		{
			BOOST_LEAF_CONSTEXPR void get() noexcept
			{
			}
		};
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class... E>
		template <class R, class... H>
		BOOST_LEAF_CONSTEXPR inline R context_base<E...>::handle_error( error_id id, H && ... h ) const
		{
			BOOST_LEAF_ASSERT(!is_active());
			return handle_error_<R>(tup(), error_info(id), std::forward<H>(h)...);
		}

		template <class... E>
		template <class R, class... H>
		BOOST_LEAF_CONSTEXPR inline R context_base<E...>::handle_error( error_id id, H && ... h )
		{
			BOOST_LEAF_ASSERT(!is_active());
			return handle_error_<R>(tup(), error_info(id), std::forward<H>(h)...);
		}
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class... E>
		class nocatch_context: public context_base<E...>
		{
		public:

			template <class TryBlock, class... H>
			BOOST_LEAF_CONSTEXPR BOOST_LEAF_ALWAYS_INLINE typename std::decay<decltype(std::declval<TryBlock>()().value())>::type try_handle_all( TryBlock && try_block, H && ... h )
			{
				using namespace leaf_detail;
				static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The return type of the try_block passed to a try_handle_all function must be registered with leaf::is_result_type");
				auto active_context = activate_context(*this);
				if( auto r = std::forward<TryBlock>(try_block)() )
					return r.value();
				else
				{
					error_id id = r.error();
					this->deactivate();
					using R = typename std::decay<decltype(std::declval<TryBlock>()().value())>::type;
					return this->template handle_error<R>(std::move(id), std::forward<H>(h)...);
				}
			}

			template <class TryBlock, class... H>
			BOOST_LEAF_CONSTEXPR BOOST_LEAF_ALWAYS_INLINE typename std::decay<decltype(std::declval<TryBlock>()())>::type try_handle_some( TryBlock && try_block, H && ... h )
			{
				using namespace leaf_detail;
				static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The return type of the try_block passed to a try_handle_some function must be registered with leaf::is_result_type");
				auto active_context = activate_context(*this);
				if( auto r = std::forward<TryBlock>(try_block)() )
					return r;
				else
				{
					error_id id = r.error();
					this->deactivate();
					using R = typename std::decay<decltype(std::declval<TryBlock>()())>::type;
					auto rr = this->template handle_error<R>(std::move(id), std::forward<H>(h)..., [&r]()->R { return std::move(r); });
					if( !rr )
						this->propagate();
					return rr;
				}
			}
		};
	}

	////////////////////////////////////////

	template <class TryBlock, class... H>
	BOOST_LEAF_CONSTEXPR inline typename std::decay<decltype(std::declval<TryBlock>()().value())>::type try_handle_all( TryBlock && try_block, H && ... h )
	{
		// Creating a named temp on purpose, to avoid C++11 and C++14 zero-initializing the context.
		context_type_from_handlers<H...> c;
		return c.try_handle_all( std::forward<TryBlock>(try_block), std::forward<H>(h)... );
	}

	template <class TryBlock, class... H>
	BOOST_LEAF_CONSTEXPR inline typename std::decay<decltype(std::declval<TryBlock>()())>::type try_handle_some( TryBlock && try_block, H && ... h )
	{
		// Creating a named temp on purpose, to avoid C++11 and C++14 zero-initializing the context.
		context_type_from_handlers<H...> c;
		return c.try_handle_some( std::forward<TryBlock>(try_block), std::forward<H>(h)... );
	}

} }

#endif
