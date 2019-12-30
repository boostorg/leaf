#ifndef LEAF_25AF99F6DC6F11E8803DE9BC9723C688
#define LEAF_25AF99F6DC6F11E8803DE9BC9723C688

// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/error.hpp>

namespace boost { namespace leaf {

	class augment_id
	{
		int const err_id_;

	public:

		augment_id() noexcept:
			err_id_(leaf_detail::current_id())
		{
		}

		int check_id() const noexcept
		{
			int err_id = leaf_detail::current_id();
			if( err_id != err_id_ )
				return err_id;
			else
			{
#ifndef LEAF_NO_EXCEPTIONS
#	if LEAF_STD_UNCAUGHT_EXCEPTIONS
				if( std::uncaught_exceptions() )
#	else
				if( std::uncaught_exception() )
#	endif
					return leaf_detail::new_id();
#endif
				return 0;
			}
		}

		int get_id() const noexcept
		{
			int err_id = leaf_detail::current_id();
			if( err_id != err_id_ )
				return err_id;
			else
				return leaf_detail::new_id();
		}

		error_id check_error() const noexcept
		{
			return leaf_detail::make_error_id(check_id());
		}

		template <class... E>
		error_id get_error( E && ... e ) const noexcept
		{
			return leaf_detail::make_error_id(get_id()).load(std::forward<E>(e)...);
		}
	};

	////////////////////////////////////////////

	namespace leaf_detail
	{
		template <int I, class Tuple>
		struct tuple_for_each_preload
		{
			LEAF_CONSTEXPR static void trigger( Tuple & tup, int err_id ) noexcept
			{
				assert((err_id&3)==1);
				tuple_for_each_preload<I-1,Tuple>::trigger(tup,err_id);
				std::get<I-1>(tup).trigger(err_id);
			}
		};

		template <class Tuple>
		struct tuple_for_each_preload<0, Tuple>
		{
			LEAF_CONSTEXPR static void trigger( Tuple const &, int ) noexcept { }
		};
	} // leaf_detail

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class E>
		class preloaded_item
		{
			slot<E> * s_;
			E e_;

		public:

			LEAF_CONSTEXPR explicit preloaded_item( E && e ) noexcept:
				s_(tl_slot_ptr<E>()),
				e_(std::forward<E>(e))
			{
			}

			LEAF_CONSTEXPR void trigger( int err_id ) noexcept
			{
				assert((err_id&3)==1);
				if( s_ )
				{
					if( !s_->has_value(err_id) )
						s_->put(err_id, std::move(e_));
				}
#if LEAF_DIAGNOSTICS
				else
				{
					int c = tl_unexpected_enabled_counter();
					assert(c>=0);
					if( c )
						load_unexpected(err_id, std::forward<E>(e_));
				}
#endif
			}
		};

		template <class... E>
		class preloaded
		{
			preloaded & operator=( preloaded const & ) = delete;

			std::tuple<preloaded_item<E>...> p_;
			bool moved_;
			augment_id id_;

		public:

			LEAF_CONSTEXPR explicit preloaded( E && ... e ) noexcept:
				p_(preloaded_item<E>(std::forward<E>(e))...),
				moved_(false)
			{
			}

			LEAF_CONSTEXPR preloaded( preloaded && x ) noexcept:
				p_(std::move(x.p_)),
				moved_(false),
				id_(std::move(x.id_))
			{
				x.moved_ = true;
			}

			~preloaded() noexcept
			{
				if( moved_ )
					return;
				if( auto id = id_.check_id() )
					leaf_detail::tuple_for_each_preload<sizeof...(E),decltype(p_)>::trigger(p_,id);
			}
		};
	} // leaf_detail

	template <class... E>
	LEAF_CONSTEXPR inline leaf_detail::preloaded<E...> preload( E && ... e ) noexcept
	{
		return leaf_detail::preloaded<E...>(std::forward<E>(e)...);
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class F>
		class deferred_item
		{
			typedef decltype(std::declval<F>()()) E;
			slot<E> * s_;
			F f_;

		public:

			LEAF_CONSTEXPR explicit deferred_item( F && f ) noexcept:
				s_(tl_slot_ptr<E>()),
				f_(std::forward<F>(f))
			{
			}

			LEAF_CONSTEXPR void trigger( int err_id ) noexcept
			{
				assert((err_id&3)==1);
				if( s_ )
				{
					if( !s_->has_value(err_id) )
						s_->put(err_id, f_());
				}
#if LEAF_DIAGNOSTICS
				else
				{
					int c = tl_unexpected_enabled_counter();
					assert(c>=0);
					if( c )
						load_unexpected(err_id, std::forward<E>(f_()));
				}
#endif
			}
		};

		template <class... F>
		class deferred
		{
			deferred & operator=( deferred const & ) = delete;
			std::tuple<deferred_item<F>...> d_;
			bool moved_;
			augment_id id_;

		public:

			LEAF_CONSTEXPR explicit deferred( F && ... f ) noexcept:
				d_(deferred_item<F>(std::forward<F>(f))...),
				moved_(false)
			{
			}

			LEAF_CONSTEXPR deferred( deferred && x ) noexcept:
				d_(std::move(x.d_)),
				moved_(false),
				id_(std::move(x.id_))
			{
				x.moved_ = true;
			}

			~deferred() noexcept
			{
				if( moved_ )
					return;
				if( auto id = id_.check_id() )
					leaf_detail::tuple_for_each_preload<sizeof...(F),decltype(d_)>::trigger(d_,id);
			}
		};
	} // leaf_detail

	template <class... F>
	LEAF_CONSTEXPR inline leaf_detail::deferred<F...> defer( F && ... f ) noexcept
	{
		return leaf_detail::deferred<F...>(std::forward<F>(f)...);
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class F, class A0 = fn_arg_type<F,0>, int arity = function_traits<F>::arity>
		class accumulating_item;

		template <class F, class A0>
		class accumulating_item<F, A0 &, 1>
		{
			using E = A0;
			slot<E> * s_;
			F f_;

		public:

			LEAF_CONSTEXPR explicit accumulating_item( F && f ) noexcept:
				s_(tl_slot_ptr<E>()),
				f_(std::forward<F>(f))
			{
			}

			LEAF_CONSTEXPR void trigger( int err_id ) noexcept
			{
				assert((err_id&3)==1);
				if( s_ )
					if( E * e = s_->has_value(err_id) )
						(void) f_(*e);
					else
						(void) f_(s_->put(err_id, E()));
			}
		};

		template <class... F>
		class accumulating
		{
			accumulating & operator=( accumulating const & ) = delete;
			std::tuple<accumulating_item<F>...> a_;
			bool moved_;
			augment_id id_;

		public:

			LEAF_CONSTEXPR explicit accumulating( F && ... f ) noexcept:
				a_(accumulating_item<F>(std::forward<F>(f))...),
				moved_(false)
			{
			}

			LEAF_CONSTEXPR accumulating( accumulating && x ) noexcept:
				a_(std::move(x.a_)),
				moved_(false),
				id_(std::move(x.id_))
			{
				x.moved_ = true;
			}

			~accumulating() noexcept
			{
				if( moved_ )
					return;
				if( auto id = id_.check_id() )
					leaf_detail::tuple_for_each_preload<sizeof...(F),decltype(a_)>::trigger(a_,id);
			}
		};
	} // leaf_detail

	template <class... F>
	LEAF_CONSTEXPR inline leaf_detail::accumulating<F...> accumulate( F && ... f ) noexcept
	{
		return leaf_detail::accumulating<F...>(std::forward<F>(f)...);
	}

} }

#endif
