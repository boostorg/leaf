#ifndef BOOST_LEAF_25AF99F6DC6F11E8803DE9BC9723C688
#define BOOST_LEAF_25AF99F6DC6F11E8803DE9BC9723C688

// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/error.hpp>
#include <boost/leaf/detail/function_traits.hpp>
#include <tuple>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <int I, class Tuple>
		struct tuple_for_each_preload
		{
			static void trigger( Tuple & tup, int err_id ) noexcept
			{
				assert(err_id);
				tuple_for_each_preload<I-1,Tuple>::trigger(tup,err_id);
				std::get<I-1>(tup).trigger(err_id);
			}
		};

		template <class Tuple>
		struct tuple_for_each_preload<0, Tuple>
		{
			static void trigger( Tuple const &, int ) noexcept { }
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

			explicit preloaded_item( E && e ) noexcept:
				s_(tl_slot_ptr<E>()),
				e_(std::forward<E>(e))
			{
			}

			void trigger( int err_id ) noexcept
			{
				assert(err_id);
				if( s_ )
				{
					if( !s_->has_value(err_id) )
						s_->put(err_id, std::move(e_));
				}
				else
				{
					int c = tl_unexpected_enabled_counter();
					assert(c>=0);
					if( c )
						no_expect_slot(err_id, std::forward<E>(e_));
				}
			}
		};

		template <class... E>
		class preloaded
		{
			preloaded & operator=( preloaded const & ) = delete;

			std::tuple<preloaded_item<E>...> p_;
			leaf_detail::id_factory * ids_;
			int err_id_;

		public:

			explicit preloaded( E && ... e ) noexcept:
				p_(preloaded_item<E>(std::forward<E>(e))...),
				ids_(&id_factory::tl_instance()),
				err_id_(ids_->last_id())
			{
			}

			preloaded( preloaded && x ) noexcept:
				p_(std::move(x.p_)),
				ids_(x.ids_),
				err_id_(std::move(x.err_id_))
			{
				x.ids_ = 0;
			}

			~preloaded() noexcept
			{
				if( !ids_ )
					return;
				int const err_id = ids_->last_id();
				if( err_id==err_id_ )
				{
					if( std::uncaught_exception() )
						leaf_detail::tuple_for_each_preload<sizeof...(E),decltype(p_)>::trigger(p_,ids_->next_id());
				}
				else
					leaf_detail::tuple_for_each_preload<sizeof...(E),decltype(p_)>::trigger(p_,err_id);
			}
		};
	} // leaf_detail

	template <class... E>
	leaf_detail::preloaded<E...> preload( E && ... e ) noexcept
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

			explicit deferred_item( F && f ) noexcept:
				s_(tl_slot_ptr<E>()),
				f_(std::forward<F>(f))
			{
			}

			void trigger( int err_id ) noexcept
			{
				assert(err_id);
				if( s_ )
				{
					if( !s_->has_value(err_id) )
						s_->put(err_id, f_());
				}
				else
				{
					int c = tl_unexpected_enabled_counter();
					assert(c>=0);
					if( c )
						no_expect_slot(err_id, std::forward<E>(f_()));
				}
			}
		};

		template <class... F>
		class deferred
		{
			deferred & operator=( deferred const & ) = delete;
			std::tuple<deferred_item<F>...> d_;
			leaf_detail::id_factory * ids_;
			int err_id_;

		public:

			explicit deferred( F && ... f ) noexcept:
				d_(deferred_item<F>(std::forward<F>(f))...),
				ids_(&id_factory::tl_instance()),
				err_id_(ids_->last_id())
			{
			}

			deferred( deferred && x ) noexcept:
				d_(std::move(x.d_)),
				ids_(x.ids_),
				err_id_(std::move(x.err_id_))
			{
				x.ids_ = 0;
			}

			~deferred() noexcept
			{
				if( !ids_ )
					return;
				int const err_id = ids_->last_id();
				if( err_id==err_id_ )
				{
					if( std::uncaught_exception() )
						leaf_detail::tuple_for_each_preload<sizeof...(F),decltype(d_)>::trigger(d_,ids_->next_id());
				}
				else
					leaf_detail::tuple_for_each_preload<sizeof...(F),decltype(d_)>::trigger(d_,err_id);
			}
		};
	} // leaf_detail

	template <class... F>
	leaf_detail::deferred<F...> defer( F && ... f ) noexcept
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

			explicit accumulating_item( F && f ) noexcept:
				s_(tl_slot_ptr<E>()),
				f_(std::forward<F>(f))
			{
			}

			void trigger( int err_id ) noexcept
			{
				assert(err_id);
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
			leaf_detail::id_factory * ids_;
			int err_id_;

		public:

			explicit accumulating( F && ... f ) noexcept:
				a_(accumulating_item<F>(std::forward<F>(f))...),
				ids_(&id_factory::tl_instance()),
				err_id_(ids_->last_id())
			{
			}

			accumulating( accumulating && x ) noexcept:
				a_(std::move(x.a_)),
				ids_(x.ids_),
				err_id_(std::move(x.err_id_))
			{
				x.ids_ = 0;
			}

			~accumulating() noexcept
			{
				if( !ids_ )
					return;
				int const err_id = ids_->last_id();
				if( err_id==err_id_ )
				{
					if( std::uncaught_exception() )
						leaf_detail::tuple_for_each_preload<sizeof...(F),decltype(a_)>::trigger(a_,ids_->next_id());
				}
				else
					leaf_detail::tuple_for_each_preload<sizeof...(F),decltype(a_)>::trigger(a_,err_id);
			}
		};
	} // leaf_detail

	template <class... F>
	leaf_detail::accumulating<F...> accumulate( F && ... f ) noexcept
	{
		return leaf_detail::accumulating<F...>(std::forward<F>(f)...);
	}

} }

#endif
