#ifndef BOOST_LEAF_25AF99F6DC6F11E8803DE9BC9723C688
#define BOOST_LEAF_25AF99F6DC6F11E8803DE9BC9723C688

// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/error.hpp>
#include <tuple>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <int I, class Tuple>
		struct tuple_for_each_preload
		{
			static void trigger( Tuple & tup, error_id id ) noexcept
			{
				tuple_for_each_preload<I-1,Tuple>::trigger(tup,id);
				std::get<I-1>(tup).trigger(id);
			}
		};

		template <class Tuple>
		struct tuple_for_each_preload<0, Tuple>
		{
			static void trigger( Tuple const &, error_id ) noexcept { }
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

			void trigger( error_id id ) noexcept
			{
				if( s_ )
				{
					if( !s_->has_value() || s_->value().id!=id )
						s_->put( leaf_detail::id_e_pair<E>(id,std::move(e_)) );
				}
				else
				{
					using T = typename std::remove_cv<typename std::remove_reference<E>::type>::type;
					int c = tl_unexpected_enabled_counter();
					assert(c>=0);
					if( c )
						no_expect_slot( id_e_pair<T>(id,std::forward<E>(e_)) );
				}
			}
		};

		template <class... E>
		class preloaded
		{
			preloaded & operator=( preloaded const & ) = delete;

			std::tuple<preloaded_item<E>...> p_;
			error_id id_;
			bool moved_;

		public:

			explicit preloaded( E && ... e ) noexcept:
				p_(preloaded_item<E>(std::forward<E>(e))...),
				id_(last_error()),
				moved_(false)
			{
			}

			preloaded( preloaded && x ) noexcept:
				p_(std::move(x.p_)),
				id_(std::move(x.id_)),
				moved_(false)
			{
				x.moved_ = true;
			}

			~preloaded() noexcept
			{
				if( moved_ )
					return;
				error_id const id = last_error();
				if( id==id_ )
				{
					if( std::uncaught_exception() )
						leaf_detail::tuple_for_each_preload<sizeof...(E),decltype(p_)>::trigger(p_,next_error());
				}
				else
					leaf_detail::tuple_for_each_preload<sizeof...(E),decltype(p_)>::trigger(p_,id);
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

			void trigger( error_id id ) noexcept
			{
				if( s_ )
				{
					if( !s_->has_value() || s_->value().id!=id )
						s_->put( leaf_detail::id_e_pair<E>(id,f_()) );
				}
				else
				{
					using T = typename std::remove_cv<typename std::remove_reference<E>::type>::type;
					int c = tl_unexpected_enabled_counter();
					assert(c>=0);
					if( c )
						no_expect_slot( id_e_pair<T>(id,std::forward<E>(f_())) );
				}
			}
		};

		template <class... F>
		class deferred
		{
			deferred & operator=( deferred const & ) = delete;
			std::tuple<deferred_item<F>...> d_;
			error_id id_;
			bool moved_;

		public:

			explicit deferred( F && ... f ) noexcept:
				d_(deferred_item<F>(std::forward<F>(f))...),
				id_(last_error()),
				moved_(false)
			{
			}

			deferred( deferred && x ) noexcept:
				d_(std::move(x.d_)),
				id_(std::move(x.id_)),
				moved_(false)
			{
				x.moved_ = true;
			}

			~deferred() noexcept
			{
				if( moved_ )
					return;
				error_id const id = last_error();
				if( id==id_ )
				{
					if( std::uncaught_exception() )
						leaf_detail::tuple_for_each_preload<sizeof...(F),decltype(d_)>::trigger(d_,next_error());
				}
				else
					leaf_detail::tuple_for_each_preload<sizeof...(F),decltype(d_)>::trigger(d_,id);
			}
		};
	} // leaf_detail

	template <class... F>
	leaf_detail::deferred<F...> defer( F && ... f ) noexcept
	{
		return leaf_detail::deferred<F...>(std::forward<F>(f)...);
	}

} }

#endif
