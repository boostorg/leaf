//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_BA049396D0D411E8B45DF7D4A759E189
#define UUID_BA049396D0D411E8B45DF7D4A759E189

#include <boost/leaf/detail/optional.hpp>
#include <atomic>
#include <cstdio>
#include <climits>
#include <ostream>

namespace boost { namespace leaf {

	class error
	{
		unsigned id_;

		explicit error( unsigned id ) noexcept:
			id_(id)
		{
		}

		class id_factory
		{
			id_factory( id_factory const & ) = delete;
			id_factory & operator=( id_factory const & ) = delete;

			static unsigned new_error_id() noexcept
			{
				static std::atomic<int> c;
				return ++c;
			}

			unsigned next_id_;

			id_factory() noexcept:
				next_id_(new_error_id())
			{
			}

		public:

			static id_factory & tl_instance() noexcept
			{
				static thread_local id_factory s;
				return s;
			}

			unsigned peek() noexcept
			{
				return next_id_;
			}

			unsigned get() noexcept
			{
				unsigned id = next_id_;
				next_id_ = new_error_id();
				return id;
			}
		};

	public:

		error() noexcept:
			id_(id_factory::tl_instance().get())
		{
		}

		template <class... E>
		explicit error( E && ... e ) noexcept:
			id_(id_factory::tl_instance().get())
		{
			propagate(std::forward<E>(e)...);
		}

		friend bool operator==( error const & e1, error const & e2 ) noexcept
		{
			return e1.id_==e2.id_;
		}

		friend bool operator!=( error const & e1, error const & e2 ) noexcept
		{
			return e1.id_!=e2.id_;
		}

		friend std::ostream & operator<<( std::ostream & os, error const & e )
		{
			char buf[sizeof(e.id_)*CHAR_BIT/4+1];
			int nw = std::sprintf(buf,"%X",e.id_);
			assert(nw>=0);
			assert(nw<sizeof(buf));
			os << buf;
			return os;
		}

		static error peek_next_error() noexcept
		{
			return error(id_factory::tl_instance().peek());
		}

		template <class... E>
		error propagate( E && ... ) const noexcept;

	};

	////////////////////////////////////////

	namespace leaf_detail
	{

		template <class E>
		struct error_info
		{
			E v;
			error e;
		};

		template <class E>
		class slot:
			optional<error_info<E>>
		{
			slot( slot const & ) = delete;
			slot & operator=( slot const & ) = delete;
			typedef optional<error_info<E>> base;
			slot<E> * prev_;
			public:
			typedef decltype(E::value) value_type;
			slot() noexcept;
			~slot() noexcept;
			using base::put;
			using base::has_value;
			using base::value;
			using base::reset;
		};

		template <class E>
		slot<E> * & tl_slot_ptr() noexcept
		{
			static thread_local slot<E> * s;
			return s;
		}

		template <class E>
		slot<E>::slot() noexcept
		{
			slot * & p = tl_slot_ptr<E>();
			prev_ = p;
			p = this;
		}

		template <class E>
		slot<E>::~slot() noexcept
		{
			if( prev_ )
			{
				optional<error_info<E>> & p = *prev_;
				p = std::move(*this);
			}
			tl_slot_ptr<E>() = prev_;
		}

		template <class E>
		E * put_slot( E && v, error const & e ) noexcept
		{
			if( leaf_detail::slot<E> * p = leaf_detail::tl_slot_ptr<E>() )
				return &p->put(leaf_detail::error_info<E>{std::forward<E>(v),e}).v;
			else
				return 0;
		}
	} //leaf_detail

	template <class... E>
	error error:: propagate( E && ... e ) const noexcept
	{
		{ using _ = void const * [ ]; (void) _ { 0, leaf_detail::put_slot(std::forward<E>(e),*this)... }; }
		return *this;
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class F,class... T>
		struct match_fn
		{
			F f;
		};

		template <class... T>
		struct match_no_fn
		{
		};
	} //leaf_detail

	template <class... T,class F>
	leaf_detail::match_fn<F,T...> match( F && f ) noexcept
	{
		return leaf_detail::match_fn<F,T...> { std::move(f) };
	}

	template <class... T>
	leaf_detail::match_no_fn<T...> match() noexcept
	{
		return leaf_detail::match_no_fn<T...> { };
	}

} }

#endif
