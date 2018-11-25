//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_25AF99F6DC6F11E8803DE9BC9723C688
#define UUID_25AF99F6DC6F11E8803DE9BC9723C688

#include <boost/leaf/error.hpp>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <int I, class Tuple>
		struct tuple_for_each_preload
		{
			static void trigger( Tuple & tup, error const & e ) noexcept
			{
				tuple_for_each_preload<I-1,Tuple>::trigger(tup,e);
				std::get<I-1>(tup).trigger(e);
			}
		};

		template <class Tuple>
		struct tuple_for_each_preload<0, Tuple>
		{
			static void trigger( Tuple const &, error const & ) noexcept { }
		};
	} //leaf_detail

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class E>
		class preloaded_item
		{
			slot<E> * s_;
			E v_;
		public:
			explicit preloaded_item( E && v ) noexcept:
				s_(tl_slot_ptr<E>()),
				v_(std::forward<E>(v))
			{
			}

			void trigger( error e ) noexcept
			{
				if( s_ && (!s_->has_value() || s_->value().e!=e) )
					s_->put( leaf_detail::error_info<E>{std::move(v_),e} );
			}
		};

		template <class... E>
		class preloaded
		{
			preloaded & operator=( preloaded const & ) = delete;

			std::tuple<preloaded_item<E>...> p_;
			error e_;
			bool moved_;

		public:

			explicit preloaded( E && ... e ) noexcept:
				p_(preloaded_item<E>(std::forward<E>(e))...),
				e_(next_error_value()),
				moved_(false)
			{
			}

			preloaded( preloaded && x ) noexcept:
				p_(std::move(x.p_)),
				e_(std::move(x.e_)),
				moved_(false)
			{
				x.moved_ = true;
			}

			~preloaded() noexcept
			{
				if( !moved_ && (e_!=next_error_value() || std::uncaught_exception()) )
					leaf_detail::tuple_for_each_preload<sizeof...(E),decltype(p_)>::trigger(p_,e_);
			}
		};
	} //leaf_detail

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

			void trigger( error e ) noexcept
			{
				if( s_ && (!s_->has_value() || s_->value().e!=e) )
					s_->put( leaf_detail::error_info<E>{f_(),e} );
			}
		};

		template <class... F>
		class deferred
		{
			deferred & operator=( deferred const & ) = delete;
			std::tuple<deferred_item<F>...> d_;
			error e_;
			bool moved_;

		public:

			explicit deferred( F && ... f ) noexcept:
				d_(deferred_item<F>(std::forward<F>(f))...),
				e_(next_error_value()),
				moved_(false)
			{
			}

			deferred( deferred && x ) noexcept:
				d_(std::move(x.d_)),
				e_(std::move(x.e_)),
				moved_(false)
			{
				x.moved_ = true;
			}

			~deferred() noexcept
			{
				if( !moved_ && (e_!=next_error_value() || std::uncaught_exception()) )
					leaf_detail::tuple_for_each_preload<sizeof...(F),decltype(d_)>::trigger(d_,e_);
			}
		};
	} //leaf_detail

	template <class... F>
	leaf_detail::deferred<F...> defer( F && ... f ) noexcept
	{
		return leaf_detail::deferred<F...>(std::forward<F>(f)...);
	}

} }

#endif
