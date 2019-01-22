#ifndef BOOST_LEAF_25AF99F6DC6F11E8803DE9BC9723C688
#define BOOST_LEAF_25AF99F6DC6F11E8803DE9BC9723C688

// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/teleport.hpp>
#include <tuple>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <int I, class Tuple>
		struct tuple_for_each_preload
		{
			static void trigger( Tuple & tup, int error_id ) noexcept
			{
				assert(error_id);
				tuple_for_each_preload<I-1,Tuple>::trigger(tup,error_id);
				std::get<I-1>(tup).trigger(error_id);
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

			void trigger( int error_id ) noexcept
			{
				assert(error_id);
				if( s_ )
				{
					if( !s_->has_value() || s_->value().error_id!=error_id )
						s_->put( leaf_detail::id_e_pair<E>(error_id,std::move(e_)) );
				}
				else
				{
					using T = typename std::remove_cv<typename std::remove_reference<E>::type>::type;
					int c = tl_unexpected_enabled_counter();
					assert(c>=0);
					if( c )
						no_expect_slot( id_e_pair<T>(error_id,std::forward<E>(e_)) );
				}
			}
		};

		template <class... E>
		class preloaded
		{
			preloaded & operator=( preloaded const & ) = delete;

			std::tuple<preloaded_item<E>...> p_;
			leaf_detail::id_factory & ids_;
			int error_id_;
			bool moved_;

		public:

			explicit preloaded( E && ... e ) noexcept:
				p_(preloaded_item<E>(std::forward<E>(e))...),
				ids_(id_factory::tl_instance()),
				error_id_(ids_.last_id()),
				moved_(false)
			{
			}

			preloaded( preloaded && x ) noexcept:
				p_(std::move(x.p_)),
				ids_(x.ids_),
				error_id_(std::move(x.error_id_)),
				moved_(false)
			{
				x.moved_ = true;
			}

			~preloaded() noexcept
			{
				if( moved_ )
					return;
				int const error_id = ids_.last_id();
				if( error_id==error_id_ )
				{
					if( std::uncaught_exception() )
						leaf_detail::tuple_for_each_preload<sizeof...(E),decltype(p_)>::trigger(p_,ids_.next_id());
				}
				else
					leaf_detail::tuple_for_each_preload<sizeof...(E),decltype(p_)>::trigger(p_,error_id);
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

			void trigger( int error_id ) noexcept
			{
				assert(error_id);
				if( s_ )
				{
					if( !s_->has_value() || s_->value().error_id!=error_id )
						s_->put( leaf_detail::id_e_pair<E>(error_id,f_()) );
				}
				else
				{
					using T = typename std::remove_cv<typename std::remove_reference<E>::type>::type;
					int c = tl_unexpected_enabled_counter();
					assert(c>=0);
					if( c )
						no_expect_slot( id_e_pair<T>(error_id,std::forward<E>(f_())) );
				}
			}
		};

		template <class... F>
		class deferred
		{
			deferred & operator=( deferred const & ) = delete;
			std::tuple<deferred_item<F>...> d_;
			leaf_detail::id_factory & ids_;
			int error_id_;
			bool moved_;

		public:

			explicit deferred( F && ... f ) noexcept:
				d_(deferred_item<F>(std::forward<F>(f))...),
				ids_(id_factory::tl_instance()),
				error_id_(ids_.last_id()),
				moved_(false)
			{
			}

			deferred( deferred && x ) noexcept:
				d_(std::move(x.d_)),
				ids_(x.ids_),
				error_id_(std::move(x.error_id_)),
				moved_(false)
			{
				x.moved_ = true;
			}

			~deferred() noexcept
			{
				if( moved_ )
					return;
				int const error_id = ids_.last_id();
				if( error_id==error_id_ )
				{
					if( std::uncaught_exception() )
						leaf_detail::tuple_for_each_preload<sizeof...(F),decltype(d_)>::trigger(d_,ids_.next_id());
				}
				else
					leaf_detail::tuple_for_each_preload<sizeof...(F),decltype(d_)>::trigger(d_,error_id);
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
