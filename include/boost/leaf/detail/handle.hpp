#ifndef BOOST_LEAF_DETAIL_HANDLE_HPP_INCLUDED
#define BOOST_LEAF_DETAIL_HANDLE_HPP_INCLUDED

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

	////////////////////////////////////////

	class error_info
	{
		error_info & operator=( error_info const & ) = delete;

		leaf_detail::exception_info_base const * const xi_;
		error_id const err_id_;

	protected:

		error_info( error_info const & ) noexcept = default;

		void print( std::ostream & os ) const
		{
			os << "Error ID = " << err_id_.value();
			if( xi_ )
				xi_->print(os);
		}

	public:

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

	protected:

		diagnostic_info( diagnostic_info const & ) noexcept = default;

		template <class Tup>
		BOOST_LEAF_CONSTEXPR diagnostic_info( error_info const & ei, leaf_detail::e_unexpected_count const * e_uc, Tup const & tup ) noexcept:
			error_info(ei),
			e_uc_(e_uc),
			tup_(&tup),
			print_(&leaf_detail::tuple_for_each<std::tuple_size<Tup>::value, Tup>::print)
		{
		}

	public:

		friend std::ostream & operator<<( std::ostream & os, diagnostic_info const & x )
		{
			os << "leaf::diagnostic_info for ";
			x.print(os);
			os << ":\n";
			x.print_(os, x.tup_, x.error().value());
			if( x.e_uc_  )
				x.e_uc_->print(os);
			return os;
		}
	};

	namespace leaf_detail
	{
		struct diagnostic_info_: diagnostic_info
		{
			template <class Tup>
			BOOST_LEAF_CONSTEXPR diagnostic_info_( error_info const & ei, leaf_detail::e_unexpected_count const * e_uc, Tup const & tup ) noexcept:
				diagnostic_info(ei, e_uc, tup)
			{
			}
		};

		template <>
		struct handler_argument_traits<diagnostic_info const &>: handler_argument_always_available<e_unexpected_count>
		{
			template <class Tup>
			BOOST_LEAF_CONSTEXPR static diagnostic_info_ get( Tup const & tup, error_info const & ei ) noexcept
			{
				return diagnostic_info_(ei, handler_argument_traits_defaults<e_unexpected_count>::check(tup, ei), tup);
			}
		};
	}

#else

	class diagnostic_info: public error_info
	{
	protected:

		diagnostic_info( diagnostic_info const & ) noexcept = default;

		BOOST_LEAF_CONSTEXPR diagnostic_info( error_info const & ei ) noexcept:
			error_info(ei)
		{
		}

	public:

		friend std::ostream & operator<<( std::ostream & os, diagnostic_info const & x )
		{
			os <<
				"leaf::diagnostic_info requires #define BOOST_LEAF_DIAGNOSTICS 1\n"
				"leaf::error_info: ";
			x.print(os);
			return os << '\n';
		}
	};

	namespace leaf_detail
	{
		struct diagnostic_info_: diagnostic_info
		{
			BOOST_LEAF_CONSTEXPR diagnostic_info_( error_info const & ei ) noexcept:
				diagnostic_info(ei)
			{
			}
		};

		template <>
		struct handler_argument_traits<diagnostic_info const &>: handler_argument_always_available<void>
		{
			template <class Tup>
			BOOST_LEAF_CONSTEXPR static diagnostic_info_ get( Tup const & tup, error_info const & ei ) noexcept
			{
				return diagnostic_info_(ei);
			}
		};
	}

#endif

	////////////////////////////////////////

#if BOOST_LEAF_DIAGNOSTICS

	class verbose_diagnostic_info: public error_info
	{
		leaf_detail::e_unexpected_info const * e_ui_;
		void const * tup_;
		void (*print_)( std::ostream &, void const * tup, int key_to_print );

	protected:

		verbose_diagnostic_info( verbose_diagnostic_info const & ) noexcept = default;

		template <class Tup>
		BOOST_LEAF_CONSTEXPR verbose_diagnostic_info( error_info const & ei, leaf_detail::e_unexpected_info const * e_ui, Tup const & tup ) noexcept:
			error_info(ei),
			e_ui_(e_ui),
			tup_(&tup),
			print_(&leaf_detail::tuple_for_each<std::tuple_size<Tup>::value, Tup>::print)
		{
		}

	public:

		friend std::ostream & operator<<( std::ostream & os, verbose_diagnostic_info const & x )
		{
			os << "leaf::verbose_diagnostic_info for ";
			x.print(os);
			os << ":\n";
			x.print_(os, x.tup_, x.error().value());
			if( x.e_ui_ )
				x.e_ui_->print(os);
			return os;
		}
	};

	namespace leaf_detail
	{
		struct verbose_diagnostic_info_: verbose_diagnostic_info
		{
			template <class Tup>
			BOOST_LEAF_CONSTEXPR verbose_diagnostic_info_( error_info const & ei, leaf_detail::e_unexpected_info const * e_ui, Tup const & tup ) noexcept:
				verbose_diagnostic_info(ei, e_ui, tup)
			{
			}
		};

		template <>
		struct handler_argument_traits<verbose_diagnostic_info const &>: handler_argument_always_available<e_unexpected_info>
		{
			template <class Tup>
			BOOST_LEAF_CONSTEXPR static verbose_diagnostic_info_ get( Tup const & tup, error_info const & ei ) noexcept
			{
				return verbose_diagnostic_info_(ei, handler_argument_traits_defaults<e_unexpected_info>::check(tup, ei), tup);
			}
		};
	}

#else

	class verbose_diagnostic_info: public error_info
	{
	protected:

		verbose_diagnostic_info( verbose_diagnostic_info const & ) noexcept = default;

		BOOST_LEAF_CONSTEXPR verbose_diagnostic_info( error_info const & ei ) noexcept:
			error_info(ei)
		{
		}

	public:

		friend std::ostream & operator<<( std::ostream & os, verbose_diagnostic_info const & x )
		{
			os <<
				"leaf::verbose_diagnostic_info requires #define BOOST_LEAF_DIAGNOSTICS 1\n"
				"leaf::error_info: ";
			x.print(os);
			return os << '\n';
		}
	};

	namespace leaf_detail
	{
		struct verbose_diagnostic_info_: verbose_diagnostic_info
		{
			BOOST_LEAF_CONSTEXPR verbose_diagnostic_info_( error_info const & ei ) noexcept:
				verbose_diagnostic_info(ei)
			{
			}
		};


		template <>
		struct handler_argument_traits<verbose_diagnostic_info const &>: handler_argument_always_available<void>
		{
			template <class Tup>
			BOOST_LEAF_CONSTEXPR static verbose_diagnostic_info_ get( Tup const & tup, error_info const & ei ) noexcept
			{
				return verbose_diagnostic_info_(ei);
			}
		};
	}

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

#ifndef BOOST_LEAF_NO_EXCEPTIONS

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

#endif

		template <class E, class SlotsTuple>
		BOOST_LEAF_CONSTEXPR inline
		E const *
		peek( SlotsTuple const & tup, error_info const & ei ) noexcept
		{
			if( error_id err = ei.error() )
				if( E const * e = std::get<tuple_type_index<slot<E>,SlotsTuple>::value>(tup).has_value(err.value()) )
					return e;
#ifndef BOOST_LEAF_NO_EXCEPTIONS
				else
					return peek_exception<E const>::peek(ei);
#endif
			return 0;
		}

		template <class E, class SlotsTuple>
		BOOST_LEAF_CONSTEXPR inline
		E *
		peek( SlotsTuple & tup, error_info const & ei ) noexcept
		{
			if( error_id err = ei.error() )
				if( E * e = std::get<tuple_type_index<slot<E>,SlotsTuple>::value>(tup).has_value(err.value()) )
					return e;
#ifndef BOOST_LEAF_NO_EXCEPTIONS
				else
					return peek_exception<E>::peek(ei);
#endif
			return 0;
		}
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class A, bool RequiresCatch>
		template <class Tup>
		BOOST_LEAF_CONSTEXPR inline
		typename handler_argument_traits_defaults<A, RequiresCatch, false>::error_type const *
		handler_argument_traits_defaults<A, RequiresCatch, false>::
		check( Tup const & tup, error_info const & ei ) noexcept
		{
			return peek<typename std::decay<A>::type>(tup, ei);
		}

		template <class A, bool RequiresCatch>
		template <class Tup>
		BOOST_LEAF_CONSTEXPR inline
		typename handler_argument_traits_defaults<A, RequiresCatch, false>::error_type *
		handler_argument_traits_defaults<A, RequiresCatch, false>::
		check( Tup & tup, error_info const & ei ) noexcept
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
		BOOST_LEAF_CONSTEXPR
		inline
		typename std::enable_if<!is_tuple<H>::value, R>::type
		handle_error_( Tup & tup, error_info const & ei, H && h )
		{
			static_assert( handler_matches_any_error<fn_mp_args<H>>::value, "The last handler passed to handle_all must match any error." );
			return handler_caller<R, H>::call( tup, ei, std::forward<H>(h), fn_mp_args<H>{ } );
		}

		template <class R, class Tup, class Car, class... Cdr>
		BOOST_LEAF_CONSTEXPR inline
		typename std::enable_if<!is_tuple<Car>::value, R>::type
		handle_error_( Tup & tup, error_info const & ei, Car && car, Cdr && ... cdr )
		{
			if( handler_matches_any_error<fn_mp_args<Car>>::value || check_handler_( tup, ei, fn_mp_args<Car>{ } ) )
				return handler_caller<R, Car>::call( tup, ei, std::forward<Car>(car), fn_mp_args<Car>{ } );
			else
				return handle_error_<R>( tup, ei, std::forward<Cdr>(cdr)...);
		}

		template <class R, class Tup, class HTup, size_t ... I>
		BOOST_LEAF_CONSTEXPR inline
		R
		handle_error_tuple_( Tup & tup, error_info const & ei, leaf_detail_mp11::index_sequence<I...>, HTup && htup )
		{
			return handle_error_<R>(tup, ei, std::get<I>(std::forward<HTup>(htup))...);
		}

		template <class R, class Tup, class HTup, class... Cdr, size_t ... I>
		BOOST_LEAF_CONSTEXPR inline
		R
		handle_error_tuple_( Tup & tup, error_info const & ei, leaf_detail_mp11::index_sequence<I...>, HTup && htup, Cdr && ... cdr )
		{
			return handle_error_<R>(tup, ei, std::get<I>(std::forward<HTup>(htup))..., std::forward<Cdr>(cdr)...);
		}

		template <class R, class Tup, class H>
		BOOST_LEAF_CONSTEXPR inline
		typename std::enable_if<is_tuple<H>::value, R>::type
		handle_error_( Tup & tup, error_info const & ei, H && h )
		{
			return handle_error_tuple_<R>(
				tup,
				ei,
				leaf_detail_mp11::make_index_sequence<std::tuple_size<typename std::decay<H>::type>::value>(),
				std::forward<H>(h));
		}

		template <class R, class Tup, class Car, class... Cdr>
		BOOST_LEAF_CONSTEXPR inline
		typename std::enable_if<is_tuple<Car>::value, R>::type
		handle_error_( Tup & tup, error_info const & ei, Car && car, Cdr && ... cdr )
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

	template <class... E>
	template <class R, class... H>
	BOOST_LEAF_CONSTEXPR BOOST_LEAF_ALWAYS_INLINE
	R
	context<E...>::
	handle_error( error_id id, H && ... h ) const
	{
		BOOST_LEAF_ASSERT(!is_active());
		return leaf_detail::handle_error_<R>(tup(), error_info(id), std::forward<H>(h)...);
	}

	template <class... E>
	template <class R, class... H>
	BOOST_LEAF_CONSTEXPR BOOST_LEAF_ALWAYS_INLINE
	R
	context<E...>::
	handle_error( error_id id, H && ... h )
	{
		BOOST_LEAF_ASSERT(!is_active());
		return leaf_detail::handle_error_<R>(tup(), error_info(id), std::forward<H>(h)...);
	}

	////////////////////////////////////////

	template <class TryBlock, class... H>
	BOOST_LEAF_CONSTEXPR inline
	typename std::decay<decltype(std::declval<TryBlock>()().value())>::type
	try_handle_all( TryBlock && try_block, H && ... h )
	{
		// Creating a named temp on purpose, to avoid C++11 and C++14 zero-initializing the context.
		context_type_from_handlers<H...> c;
		return c.try_handle_all( std::forward<TryBlock>(try_block), std::forward<H>(h)... );
	}

	template <class TryBlock, class... H>
	BOOST_LEAF_NODISCARD BOOST_LEAF_CONSTEXPR inline
	typename std::decay<decltype(std::declval<TryBlock>()())>::type
	try_handle_some( TryBlock && try_block, H && ... h )
	{
		// Creating a named temp on purpose, to avoid C++11 and C++14 zero-initializing the context.
		context_type_from_handlers<H...> c;
		return c.try_handle_some( std::forward<TryBlock>(try_block), std::forward<H>(h)... );
	}

} }

#endif
