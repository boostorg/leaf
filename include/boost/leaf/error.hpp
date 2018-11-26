//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_BA049396D0D411E8B45DF7D4A759E189
#define UUID_BA049396D0D411E8B45DF7D4A759E189

#include <boost/leaf/detail/optional.hpp>
#include <boost/leaf/detail/print.hpp>
#include <atomic>
#include <cstdio>
#include <climits>
#include <ostream>
#include <type_traits>
#include <system_error>

#define LEAF_ERROR ::boost::leaf::next_error_value().propagate(::boost::leaf::meta::e_source_location{__FILE__,__LINE__,__FUNCTION__}),::boost::leaf::error

namespace boost { namespace system { class error_code; } }

namespace boost { namespace leaf {

	namespace meta
	{
		struct e_source_location
		{
			char const * const file;
			int const line;
			char const * const function;

			friend std::ostream & operator<<( std::ostream & os, e_source_location const & x )
			{
				return os << "At " << x.file << '(' << x.line << ") in function " << x.function << std::endl;
			}
		};

		struct e_unexpected
		{
			char const * (*first_type)();
			int count;

			friend std::ostream & operator<<( std::ostream & os, e_unexpected const & x )
			{
				assert(x.first_type!=0);
				assert(x.count>0);
				return os << "Detected  " << x.count << " attempt(s) to communicate unexpected error objects, first is of type " << x.first_type() << std::endl;
			}
		};
	}

	namespace leaf_detail
	{
		template <class T, class E = void>
		struct has_data_member_value
		{
			static constexpr bool value=false;
		};

		template <class T>
		struct has_data_member_value<T, decltype(std::declval<T const &>().value, void())>
		{
			static constexpr bool value=std::is_member_object_pointer<decltype(&T::value)>::value;
		};
	}

	template <class T>
	struct is_error_type
	{
		static constexpr bool value = leaf_detail::has_data_member_value<T>::value;
	};

	template <> struct is_error_type<system::error_code>: std::true_type { };
	template <> struct is_error_type<std::error_code>: std::true_type { };
	template <> struct is_error_type<meta::e_unexpected>: std::true_type { };
	template <> struct is_error_type<meta::e_source_location>: std::true_type { };

	////////////////////////////////////////

	class error;

	error next_error_value() noexcept;

	class error
	{
		friend error leaf::next_error_value() noexcept;

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
			int nw = std::snprintf(buf,sizeof(buf),"%X",e.id_);
			assert(nw>=0);
			assert(nw<sizeof(buf));
			os << buf;
			return os;
		}

		static error next_error_value() noexcept
		{
			return error(id_factory::tl_instance().peek());
		}

		template <class... E>
		error propagate( E && ... ) const noexcept;
	};

	inline error next_error_value() noexcept
	{
		return error(error::id_factory::tl_instance().peek());
	}

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
			static_assert(is_error_type<E>::value,"All types passed to leaf::expect must be error types");
		public:
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
				return &p->put( leaf_detail::error_info<E>{std::forward<E>(v),e} ).v;
			else if( leaf_detail::slot<meta::e_unexpected> * p = leaf_detail::tl_slot_ptr<meta::e_unexpected>() )
			{
				if( p->has_value() )
				{
					auto & v = p->value();
					if( v.e==e )
					{
						++v.v.count;
						return 0;
					}
				}
				(void) p->put( leaf_detail::error_info<meta::e_unexpected>{meta::e_unexpected{&type<E>,1},e} );
			}
			return 0;
		}
	} //leaf_detail

	template <class... E>
	error error::propagate( E && ... e ) const noexcept
	{
		{ using _ = void const * [ ]; (void) _ { 0, leaf_detail::put_slot(std::forward<E>(e),*this)... }; }
		return *this;
	}

} }

#endif
