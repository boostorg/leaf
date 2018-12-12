#ifndef BOOST_LEAF_BA049396D0D411E8B45DF7D4A759E189
#define BOOST_LEAF_BA049396D0D411E8B45DF7D4A759E189

//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/optional.hpp>
#include <boost/leaf/detail/print.hpp>
#include <atomic>
#include <cstdio>
#include <climits>
#include <ostream>
#include <type_traits>
#include <system_error>
#include <sstream>

#define LEAF_ERROR(...) ::boost::leaf::leaf_detail::make_error(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)

namespace boost { namespace system { class error_code; } }

namespace boost { namespace leaf {

	struct e_source_location
	{
		char const * const file;
		int const line;
		char const * const function;

		friend std::ostream & operator<<( std::ostream & os, e_source_location const & x )
		{
			return os << leaf::type<e_source_location>() << ": " << x.file << '(' << x.line << ") in function " << x.function;
		}
	};

	struct e_unexpected
	{
		char const * (*first_type)() noexcept;
		int count;

		friend std::ostream & operator<<( std::ostream & os, e_unexpected const & x )
		{
			assert(x.first_type!=0);
			assert(x.count>0);
			if( x.count==1 )
				os << "Detected 1 attempt to communicate an unexpected error object of type ";
			else
				os << "Detected " << x.count << " attempts to communicate unexpected error objects, the first one of type ";
			return os << x.first_type();
		}
	};

	struct e_unexpected_diagnostic_output
	{
		std::string value;

		friend std::ostream & operator<<( std::ostream & os, e_unexpected_diagnostic_output const & x )
		{
			return os << x.value;
		}
	};

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
	template <> struct is_error_type<e_unexpected>: std::true_type { };
	template <> struct is_error_type<e_source_location>: std::true_type { };

	////////////////////////////////////////

	class error;

	error next_error_value() noexcept;
	error last_error_value() noexcept;

	class error
	{
		friend error leaf::next_error_value() noexcept;
		friend error leaf::last_error_value() noexcept;

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
			unsigned last_id_;

			id_factory() noexcept:
				next_id_(new_error_id()),
				last_id_(next_id_-1)
			{
			}

		public:

			static id_factory & tl_instance() noexcept
			{
				static thread_local id_factory s;
				return s;
			}

			unsigned next_id() noexcept
			{
				return next_id_;
			}

			unsigned last_id() noexcept
			{
				return last_id_;
			}

			unsigned get() noexcept
			{
				unsigned id = last_id_ = next_id_;
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

		template <class... E>
		error propagate( E && ... ) const noexcept;
	};

	inline error next_error_value() noexcept
	{
		return error(error::id_factory::tl_instance().next_id());
	}

	inline error last_error_value() noexcept
	{
		return error(error::id_factory::tl_instance().last_id());
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

		inline int & tl_unexpected_enabled_counter() noexcept
		{
			static thread_local int c;
			return c;
		}

		template <class E>
		class slot:
			optional<error_info<E>>
		{
			slot( slot const & ) = delete;
			slot & operator=( slot const & ) = delete;
			typedef optional<error_info<E>> base;
			slot<E> * prev_;
		protected:
			slot() noexcept;
			~slot() noexcept;
		public:
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
		void put_unexpected( error_info<E> const & ev ) noexcept
		{
			if( slot<e_unexpected> * p = tl_slot_ptr<e_unexpected>() )
			{
				if( p->has_value() )
				{
					auto & p_ev = p->value();
					if( p_ev.e==ev.e )
					{
						++p_ev.v.count;
						return;
					}
				}
				(void) p->put( error_info<e_unexpected>{e_unexpected{&type<E>,1},ev.e} );
			}
		}

		template <class E>
		void put_unexpected_diagnostic_output( error_info<E> const & ev ) noexcept
		{
			if( slot<e_unexpected_diagnostic_output> * p = tl_slot_ptr<e_unexpected_diagnostic_output>() )
			{
				std::stringstream s;
				if( !diagnostic<decltype(ev.v)>::print(s,ev.v) )
					return;
				if( p->has_value() )
				{
					auto & p_ev = p->value();
					if( p_ev.e==ev.e )
					{
						std::string & value = p_ev.v.value;
						value += '\n';
						value += s.str();
						value += " {unexpected}";
						return;
					}
				}
				(void) p->put( error_info<e_unexpected_diagnostic_output>{e_unexpected_diagnostic_output{s.str()+" {unexpected}"},ev.e} );
			}
		}

		template <class E>
		void no_expect_slot( error_info<E> const & ev ) noexcept
		{
			put_unexpected(ev);
			put_unexpected_diagnostic_output(ev);
		}

		template <class E>
		int put_slot( E && v, error const & e ) noexcept
		{
			using T = typename std::remove_cv<typename std::remove_reference<E>::type>::type;
			if( slot<T> * p = tl_slot_ptr<T>() )
				(void) p->put( error_info<T>{std::forward<E>(v),e} );
			else
			{
				int c = tl_unexpected_enabled_counter();
				assert(c>=0);
				if( c )
					no_expect_slot( error_info<T>{std::forward<E>(v),e} );
			}
			return 0;
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
			else
			{
				int c = tl_unexpected_enabled_counter();
				assert(c>=0);
				if( c && has_value() )
					no_expect_slot(value());
			}
			tl_slot_ptr<E>() = prev_;
		}

		template <class... E>
		error make_error( char const * file, int line, char const * function, E && ... e )
		{
			assert(file&&*file);
			assert(line>0);
			assert(function&&*function);
			e_source_location sl { file, line, function }; //Temp object MSVC workaround
			return error( std::move(sl), std::forward<E>(e)... );
		}
	} //leaf_detail

	template <class... E>
	error error::propagate( E && ... e ) const noexcept
	{
		{ using _ = int const [ ]; (void) _ { 0, leaf_detail::put_slot(std::forward<E>(e),*this)... }; }
		return *this;
	}

} }

#endif
