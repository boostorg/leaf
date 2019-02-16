#ifndef BOOST_LEAF_E72958AC28C711E9998B3465EBB1FB94
#define BOOST_LEAF_E72958AC28C711E9998B3465EBB1FB94

// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/error.hpp>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <int I, class Tuple>
		struct tuple_for_each
		{
			static void activate( Tuple & tup ) noexcept
			{
				tuple_for_each<I-1,Tuple>::activate(tup);
				std::get<I-1>(tup).activate();
			}

			static void deactivate( Tuple & tup, bool propagate_errors ) noexcept
			{
				std::get<I-1>(tup).deactivate(propagate_errors);
				tuple_for_each<I-1,Tuple>::deactivate(tup, propagate_errors);
			}

			static void print( std::ostream & os, Tuple const & tup ) noexcept
			{
				tuple_for_each<I-1,Tuple>::print(os,tup);
				std::get<I-1>(tup).print(os);
			}
		};

		template <class Tuple>
		struct tuple_for_each<0, Tuple>
		{
			static void activate( Tuple & ) noexcept { }
			static void deactivate( Tuple &, bool ) noexcept { }
			static void print( std::ostream &, Tuple const & ) noexcept { }
		};
	}

	////////////////////////////////////////////

	namespace leaf_detail
	{
		class e_unexpected_count;
		class e_unexpected_info;

		template <class T> struct requires_unexpected { constexpr static bool value = false; };
		template <class T> struct requires_unexpected<T const> { constexpr static bool value = requires_unexpected<T>::value; };
		template <class T> struct requires_unexpected<T const &> { constexpr static bool value = requires_unexpected<T>::value; };
		template <class T> struct requires_unexpected<T const *> { constexpr static bool value = requires_unexpected<T>::value; };
		template <> struct requires_unexpected<e_unexpected_count> { constexpr static bool value = true; };
		template <> struct requires_unexpected<e_unexpected_info> { constexpr static bool value = true; };

		template <class L>
		struct unexpected_requested;

		template <template <class ...> class L>
		struct unexpected_requested<L<>>
		{
			constexpr static bool value = false;
		};

		template <template <class...> class L, template <class> class S, class Car, class... Cdr>
		struct unexpected_requested<L<S<Car>, S<Cdr>...>>
		{
			constexpr static bool value = requires_unexpected<Car>::value || unexpected_requested<L<S<Cdr>...>>::value;
		};
	}

	////////////////////////////////////////////

	class error_info;
	class diagnostic_info;
	class verbose_diagnostic_info;

	namespace leaf_detail
	{
		template <class T> struct translate_type_impl { using type = T; };
		template <class T> struct translate_type_impl<T const> { using type = T; };
		template <class T> struct translate_type_impl<T const *> { using type = T; };
		template <class T> struct translate_type_impl<T const &> { using type = T; };

		template <> struct translate_type_impl<diagnostic_info>;
		template <> struct translate_type_impl<diagnostic_info const>;
		template <> struct translate_type_impl<diagnostic_info const *>;
		template <> struct translate_type_impl<diagnostic_info const &> { using type = e_unexpected_count; };

		template <> struct translate_type_impl<verbose_diagnostic_info>;
		template <> struct translate_type_impl<verbose_diagnostic_info const>;
		template <> struct translate_type_impl<verbose_diagnostic_info const *>;
		template <> struct translate_type_impl<verbose_diagnostic_info const &> { using type = e_unexpected_info; };

		struct e_original_ec;
		template <> struct translate_type_impl<std::error_code> { using type = e_original_ec; };
		template <> struct translate_type_impl<std::error_code const> { using type = e_original_ec; };
		template <> struct translate_type_impl<std::error_code const *> { using type = e_original_ec; };
		template <> struct translate_type_impl<std::error_code const &> { using type = e_original_ec; };

		template <class T>
		using translate_type = typename translate_type_impl<T>::type;

		template <class... T>
		struct translate_list_impl;

		template <template<class...> class L, class... T>
		struct translate_list_impl<L<T...>>
		{
			using type = leaf_detail_mp11::mp_list<translate_type<T>...>;
		};

		template <class L> using translate_list = typename translate_list_impl<L>::type;

		template <class T> struct does_not_participate_in_context_deduction: std::false_type { };
		template <> struct does_not_participate_in_context_deduction<error_info>: std::true_type { };
		template <> struct does_not_participate_in_context_deduction<std::error_code>: std::true_type { };
		template <> struct does_not_participate_in_context_deduction<void>: std::true_type { };

		template <class L>
		struct transform_e_type_list_impl;

		template <template<class...> class L, class... T>
		struct transform_e_type_list_impl<L<T...>>
		{
			using type =
				leaf_detail_mp11::mp_remove_if<
					leaf_detail_mp11::mp_unique<
						translate_list<L<T...>>
					>,
					does_not_participate_in_context_deduction
				>;
		};

		template <class L> using transform_e_type_list = typename transform_e_type_list_impl<L>::type;

		template <class L>
		struct deduce_e_tuple_impl;

		template <template <class...> class L, class... E>
		struct deduce_e_tuple_impl<L<E...>>
		{
			using type = std::tuple<slot<E>...>;
		};

		template <class... E>
		using deduce_e_tuple = typename deduce_e_tuple_impl<leaf_detail::transform_e_type_list<leaf_detail_mp11::mp_list<E...>>>::type;
	}

	////////////////////////////////////////////

	template <class... Ex>
	class catch_;

	template <class TryBlock, class... H>
	decltype(std::declval<TryBlock>()()) try_catch( TryBlock &&, H && ... );

	template <class TryBlock, class RemoteH>
	decltype(std::declval<TryBlock>()()) remote_try_catch( TryBlock &&, RemoteH && );

	namespace leaf_detail
	{
		template <class... E>
		class context_base: public polymorphic_context
		{
			context_base( context_base const & ) = delete;
			context_base & operator=( context_base const & ) = delete;

			template <class TryBlock, class... H>
			friend decltype(std::declval<TryBlock>()()) leaf::try_catch( TryBlock &&, H && ... );

			template <class TryBlock, class RemoteH>
			friend decltype(std::declval<TryBlock>()()) leaf::remote_try_catch( TryBlock &&, RemoteH && );

		public:

			using Tup = leaf_detail::deduce_e_tuple<E...>;

		private:

			Tup tup_;
			std::thread::id thread_id_;
			bool is_active_;

		public:

			context_base() noexcept:
				is_active_(false)
			{
			}

			context_base( context_base && x ) noexcept:
				tup_(std::move(x.tup_)),
				is_active_(false)
			{
				assert(!x.is_active());
			}

			~context_base() noexcept
			{
				assert(!is_active());
			}

			Tup const & tup() const noexcept
			{
				return tup_;
			}

			void activate() noexcept final override
			{
				using namespace leaf_detail;
				assert(!is_active());
				tuple_for_each<std::tuple_size<Tup>::value,Tup>::activate(tup_);
				if( unexpected_requested<Tup>::value )
					++tl_unexpected_enabled_counter();
				thread_id_ = std::this_thread::get_id();
				is_active_ = true;
			}

			void deactivate( bool propagate_errors ) noexcept final override
			{
				using namespace leaf_detail;
				assert(is_active());
				is_active_ = false;
				thread_id_ = std::thread::id();
				if( unexpected_requested<Tup>::value )
					--tl_unexpected_enabled_counter();
				tuple_for_each<std::tuple_size<Tup>::value,Tup>::deactivate(tup_, propagate_errors);
			}

			bool is_active() const noexcept final override
			{
				return is_active_;
			}

			void print( std::ostream & os ) const final override
			{
				leaf_detail::tuple_for_each<std::tuple_size<Tup>::value,Tup>::print(os, tup_);
			}

			std::thread::id const & thread_id() const noexcept final override
			{
				return thread_id_;
			}

		protected:

			template <class R, class... H>
			typename std::decay<decltype(std::declval<R>().value())>::type handle_all( R const &, H && ... ) const noexcept;

			template <class R, class RemoteH>
			typename std::decay<decltype(std::declval<R>().value())>::type remote_handle_all( R const &, RemoteH && ) const noexcept;

			template <class R, class... H>
			R handle_some( R const &, H && ... ) const;

			template <class R, class RemoteH>
			R remote_handle_some( R const &, RemoteH && ) const;

			template <class TryBlock, class... H>
			typename std::decay<decltype(std::declval<TryBlock>()().value())>::type try_handle_all_( TryBlock &&, H && ... ) const;

			template <class TryBlock, class RemoteH>
			typename std::decay<decltype(std::declval<TryBlock>()().value())>::type remote_try_handle_all_( TryBlock &&, RemoteH && ) const;

			template <class TryBlock, class... H>
			typename std::decay<decltype(std::declval<TryBlock>()())>::type try_handle_some_( context_activator &, TryBlock &&, H && ... ) const;

			template <class TryBlock, class RemoteH>
			typename std::decay<decltype(std::declval<TryBlock>()())>::type remote_try_handle_some_( context_activator &, TryBlock &&, RemoteH && ) const;

			template <class TryBlock, class... H>
			decltype(std::declval<TryBlock>()()) try_catch_( TryBlock &&, H && ... ) const;

			template <class TryBlock, class RemoteH>
			decltype(std::declval<TryBlock>()()) remote_try_catch_( TryBlock &&, RemoteH && ) const;
		};

		template <class... E>
		class nocatch_context: public context_base<E...>
		{
			using base = context_base<E...>;

		public:

			using base::handle_all;
			using base::remote_handle_all;
			using base::handle_some;
			using base::remote_handle_some;

			template <class TryBlock, class... H>
			typename std::decay<decltype(std::declval<TryBlock>()().value())>::type try_handle_all( TryBlock &&, H && ... ) noexcept;

			template <class TryBlock, class RemoteH>
			typename std::decay<decltype(std::declval<TryBlock>()().value())>::type remote_try_handle_all( TryBlock &&, RemoteH && ) noexcept;

			template <class TryBlock, class... H>
			typename std::decay<decltype(std::declval<TryBlock>()())>::type try_handle_some( TryBlock &&, H && ... );

			template <class TryBlock, class RemoteH>
			typename std::decay<decltype(std::declval<TryBlock>()())>::type remote_try_handle_some( TryBlock &&, RemoteH && );
		};

		template <class... E>
		class catch_context: public context_base<E...>
		{
			using base = context_base<E...>;

		public:

			using base::handle_all;
			using base::remote_handle_all;
			using base::handle_some;
			using base::remote_handle_some;

			template <class TryBlock, class... H>
			typename std::decay<decltype(std::declval<TryBlock>()().value())>::type try_handle_all( TryBlock && try_block, H && ... h ) noexcept;

			template <class TryBlock, class RemoteH>
			typename std::decay<decltype(std::declval<TryBlock>()().value())>::type remote_try_handle_all( TryBlock && try_block, RemoteH && h ) noexcept;

			template <class TryBlock, class... H>
			typename std::decay<decltype(std::declval<TryBlock>()())>::type try_handle_some( TryBlock && try_block, H && ... h );

			template <class TryBlock, class RemoteH>
			typename std::decay<decltype(std::declval<TryBlock>()())>::type remote_try_handle_some( TryBlock && try_block, RemoteH && h );

			////////////////////////////////////////////

			template <class R, class... H>
			R handle_current_exception( H && ... ) const;

			template <class R, class RemoteH>
			R remote_handle_current_exception( RemoteH && ) const;

			template <class R, class... H>
			R handle_exception( std::exception_ptr const &, H && ... ) const;

			template <class R, class RemoteH>
			R remote_handle_exception( std::exception_ptr const &, RemoteH &&  ) const;
		};

		template <class T> struct requires_catch { constexpr static bool value = false; };
		template <class T> struct requires_catch<T const> { constexpr static bool value = requires_catch<T>::value; };
		template <class T> struct requires_catch<T const &> { constexpr static bool value = requires_catch<T>::value; };
		template <class... Ex> struct requires_catch<catch_<Ex...>> { constexpr static bool value = true; };

		template <class... E>
		struct catch_requested;

		template <>
		struct catch_requested<>
		{
			constexpr static bool value = false;
		};

		template <class Car, class... Cdr>
		struct catch_requested<Car, Cdr...>
		{
			constexpr static bool value = requires_catch<Car>::value || catch_requested<Cdr...>::value;
		};

		template <bool CatchRequested, class... E>
		struct select_context_base_impl;

		template <class... E>
		struct select_context_base_impl<false, E...>
		{
			using type = nocatch_context<E...>;
		};

		template <class... E>
		struct select_context_base_impl<true, E...>
		{
			using type = catch_context<E...>;
		};

		template <class... E>
		using select_context_base = typename select_context_base_impl<catch_requested<E...>::value, E...>::type;
	}

	template <class... E>
	class context: public leaf_detail::select_context_base<E...>
	{
	};

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class HandlerL>
		struct handler_args_impl;

		template <template <class...> class L, class... H>
		struct handler_args_impl<L<H...>>
		{
			using type = leaf_detail_mp11::mp_append<fn_mp_args<H>...>;
		};

		template <class HandlerL>
		using handler_args = typename handler_args_impl<HandlerL>::type;

		template <class TypeList>
		struct deduce_context_impl;

		template <template <class...> class L, class... E>
		struct deduce_context_impl<L<E...>>
		{
			using type = context<E...>;
		};

		template <class TypeList>
		using deduce_context = typename deduce_context_impl<TypeList>::type;

		template <class RemoteH>
		struct context_type_from_remote_handler_impl;

		template <template <class...> class L, class... H>
		struct context_type_from_remote_handler_impl<L<H...>>
		{
			using type = deduce_context<leaf_detail_mp11::mp_append<fn_mp_args<H>...>>;
		};

		template <class... H>
		struct context_type_from_handlers_impl
		{
			using type = deduce_context<leaf_detail_mp11::mp_append<fn_mp_args<H>...>>;
		};
	}

	template <class... H>
	using context_type_from_handlers = typename leaf_detail::context_type_from_handlers_impl<H...>::type;

	template <class RemoteH>
	using context_type_from_remote_handler = typename leaf_detail::context_type_from_remote_handler_impl<leaf_detail::fn_return_type<RemoteH>>::type;

	template <class...  H>
	context_type_from_handlers<H...> make_context()
	{
		return { };
	}

	template <class RemoteH>
	context_type_from_remote_handler<RemoteH> make_context( RemoteH const * = 0 )
	{
		return { };
	}

	template <class RemoteH>
	std::shared_ptr<polymorphic_context> make_shared_context( RemoteH const * = 0 )
	{
		return std::make_shared<context_type_from_remote_handler<RemoteH>>();
	}

	template <class RemoteH, class Alloc>
	std::shared_ptr<polymorphic_context> allocate_shared_context( Alloc alloc, RemoteH const * = 0 )
	{
		return std::allocate_shared<context_type_from_remote_handler<RemoteH>>(alloc);
	}

} }

#endif
