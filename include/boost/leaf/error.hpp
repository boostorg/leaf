#ifndef BOOST_LEAF_BA049396D0D411E8B45DF7D4A759E189
#define BOOST_LEAF_BA049396D0D411E8B45DF7D4A759E189

// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/optional.hpp>
#include <boost/leaf/detail/print.hpp>
#include <atomic>
#include <ostream>
#include <type_traits>
#include <system_error>
#include <sstream>
#include <set>

#define LEAF_ERROR(...) ::boost::leaf::leaf_detail::make_error(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)

namespace boost { namespace system { class error_code; } }

namespace boost { namespace leaf {

	class error_id
	{
		template <class... E>
		friend error_id new_error( E && ... ) noexcept;
		friend error_id next_error() noexcept;
		friend error_id last_error() noexcept;
		friend error_id success() noexcept;
		friend error_id get_error_id( std::error_code ) noexcept;
		friend std::error_code get_error_code( error_id ) noexcept;

		int id_;

		explicit error_id( int id ) noexcept:
			id_(id)
		{
		}

		class id_factory
		{
			id_factory( id_factory const & ) = delete;
			id_factory & operator=( id_factory const & ) = delete;

			static int new_error_id() noexcept
			{
				static std::atomic<unsigned> c;
				if( unsigned id = ++c )
					return int(id);
				else
					return int(++c);
			}

			int next_id_;
			int last_id_;

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

			int next_id() noexcept
			{
				return next_id_;
			}

			int last_id() noexcept
			{
				return last_id_;
			}

			int new_id() noexcept
			{
				int id = last_id_ = next_id_;
				next_id_ = new_error_id();
				return id;
			}
		};

	public:

		operator bool() const noexcept
		{
			return id_!=0;
		}

		friend bool operator==( error_id e1, error_id e2 ) noexcept
		{
			return e1.id_==e2.id_;
		}

		friend bool operator!=( error_id e1, error_id e2 ) noexcept
		{
			return e1.id_!=e2.id_;
		}

		error_id propagate() const noexcept
		{
			return *this;
		}

		friend std::ostream & operator<<( std::ostream & os, error_id id )
		{
			os << id.id_;
			return os;
		}

		template <class... E>
		error_id propagate( E && ... ) const noexcept;
	};

	template <class... E>
	error_id new_error( E && ... e ) noexcept
	{
		return error_id(error_id::id_factory::tl_instance().new_id()).propagate(std::forward<E>(e)...);
	}

	inline error_id next_error() noexcept
	{
		return error_id(error_id::id_factory::tl_instance().next_id());
	}

	inline error_id last_error() noexcept
	{
		return error_id(error_id::id_factory::tl_instance().last_id());
	}

	inline error_id success() noexcept
	{
		return error_id(0);
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		class slot_base
		{
			slot_base( slot_base const & ) = delete;
			slot_base & operator=( slot_base const & ) = delete;

			virtual bool slot_print( std::ostream &, error_id ) const = 0;

		public:

			static void print( std::ostream & os, error_id id )
			{
				for( slot_base const * p = first(); p; p=p->next_ )
					if( p->slot_print(os,id) )
						os << std::endl;
			}

		protected:

			static slot_base const * & first() noexcept
			{
				static thread_local slot_base const * p = 0;
				return p;
			}

			slot_base const * const next_;

			slot_base() noexcept:
				next_(first())
			{
				first() = this;
			}

			~slot_base() noexcept
			{
				slot_base const * & p = first();
				assert(p==this);
				p = next_;
			}
		};
	}

	////////////////////////////////////////

	namespace leaf_detail { class captured_exception; }

	class error_info
	{
		error_info( error_info const & ) = delete;
		error_info & operator=( error_info const & ) = delete;

		error_id const id_;
		std::exception const * const ex_;
		leaf_detail::captured_exception const * const cap_;
		void (* const print_ex_)( std::ostream &, std::exception const *, leaf_detail::captured_exception const * );

	public:

		explicit error_info( error_id id ) noexcept:
			id_(id),
			ex_(0),
			cap_(0),
			print_ex_(0)
			{
			}

		error_info( error_id id, std::exception const * ex, void (*print_ex)(std::ostream &, std::exception const *, leaf_detail::captured_exception const *) ) noexcept:
			id_(id),
			ex_(ex),
			cap_(0),
			print_ex_(print_ex)
		{
			assert(print_ex_!=0);
		}

		error_info( error_id id, std::exception const * ex, leaf_detail::captured_exception const * cap, void (*print_ex)(std::ostream &, std::exception const *, leaf_detail::captured_exception const *) ) noexcept:
			id_(id),
			ex_(ex),
			cap_(cap),
			print_ex_(print_ex)
		{
			assert(print_ex_!=0);
		}

		error_id error() const noexcept
		{
			return id_;
		}

		bool exception_caught() const noexcept
		{
			return print_ex_!=0;
		}

		std::exception const * exception() const noexcept
		{
			assert(exception_caught());
			return ex_;
		}

		friend std::ostream & operator<<( std::ostream & os, error_info const & x )
		{
			os << "leaf::error_id: " << x.id_ << std::endl;
			if( x.print_ex_ )
				x.print_ex_(os,x.ex_,x.cap_);
			leaf_detail::slot_base::print(os,x.id_);
			return os;
		}
	};

	////////////////////////////////////////

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

		template <class T>
		struct is_error_type_default
		{
			static constexpr bool value = has_data_member_value<T>::value || std::is_base_of<std::exception,T>::value;
		};

		template <> struct is_error_type_default<std::exception_ptr>: std::true_type { };
		template <> struct is_error_type_default<std::error_code>: std::true_type { };
		template <> struct is_error_type_default<system::error_code>: std::true_type { };
	}

	template <class T>
	struct is_error_type: leaf_detail::is_error_type_default<T>
	{
	};

	////////////////////////////////////////

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

	namespace leaf_detail
	{
		template <> struct is_error_type_default<e_source_location>: std::true_type { };
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		class monitor_base
		{
			monitor_base( monitor_base const & ) = delete;
			monitor_base & operator=( monitor_base const & ) = delete;

			mutable error_info const * ei_;

		protected:

			monitor_base() noexcept:
				ei_(0)
			{
			}

			monitor_base( monitor_base && x ) noexcept:
				ei_(0)
			{
				x.ei_ = 0;
			}

			void set_error_info( error_info const & ei ) const noexcept
			{
				ei_ = &ei;
			}

			error_info const & get_error_info() const noexcept
			{
				assert(ei_!=0);
				return *ei_;
			}

		public:

			error_id error() const noexcept
			{
				return ei_->error();
			}

			bool exception_caught() const noexcept
			{
				return ei_->exception_caught();
			}

			std::exception const * exception() const noexcept
			{
				return ei_->exception();
			}
		};
	}

	class diagnostic_info: leaf_detail::monitor_base
	{
	public:

		char const * (*first_type)();
		int count;

		explicit diagnostic_info( char const * (*first_type)() ) noexcept:
			first_type(first_type),
			count(1)
		{
		}

		using monitor_base::set_error_info;

		friend std::ostream & operator<<( std::ostream & os, diagnostic_info const & x )
		{
			assert(x.first_type!=0);
			assert(x.count>0);
			os << x.get_error_info() << "diagnostic_info: Detected ";
			if( x.count==1 )
				os << "1 attempt to communicate an E-object";
			else
				os << x.count << " attempts to communicate unexpected E-objects, the first one";
			return os << " of type " << x.first_type() << std::endl;
		}
	};

	namespace leaf_detail
	{
		template <> struct is_error_type_default<diagnostic_info>: std::true_type { };

		template <>
		struct diagnostic<diagnostic_info,true,false>
		{
			static bool print( std::ostream & os, diagnostic_info const & ) noexcept
			{
				return false;
			}
		};
	}

	class verbose_diagnostic_info: leaf_detail::monitor_base
	{
		std::string s_;
		std::set<char const *(*)()> already_;

	public:

		verbose_diagnostic_info() noexcept
		{
		}

		using monitor_base::set_error_info;

		void reset() noexcept
		{
			s_.clear();
			already_.clear();
		}

		template <class E>
		void add( E const & e )
		{
			std::stringstream s;
			if( leaf_detail::diagnostic<E>::print(s,e) && already_.insert(&type<E>).second  )
			{
				s << std::endl;
				s_ += s.str();
			}
		}

		friend std::ostream & operator<<( std::ostream & os, verbose_diagnostic_info const & x )
		{
			os <<
				x.get_error_info() <<
				"verbose_diagnostic_info:" << std::endl <<
				x.s_;
			return os;
		}
	};

	namespace leaf_detail
	{
		template <> struct is_error_type_default<verbose_diagnostic_info>: std::true_type { };

		template <>
		struct diagnostic<verbose_diagnostic_info,true,false>
		{
			static bool print( std::ostream & os, verbose_diagnostic_info const & ) noexcept
			{
				return false;
			}
		};
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class E>
		struct id_e_pair
		{
			error_id id;
			E e;

			explicit id_e_pair( error_id id ) noexcept:
				id(id)
			{
			}

			id_e_pair( error_id id, E const & e ):
				id(id),
				e(e)
			{
			}

			id_e_pair( error_id id, E && e ) noexcept:
				id(id),
				e(std::forward<E>(e))
			{
			}
		};

		inline int & tl_unexpected_enabled_counter() noexcept
		{
			static thread_local int c;
			return c;
		}
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class E>
		class slot:
			slot_base,
			optional<id_e_pair<E>>
		{
			slot( slot const & ) = delete;
			slot & operator=( slot const & ) = delete;
			typedef optional<id_e_pair<E>> base;
			slot<E> * prev_;
			static_assert(is_error_type<E>::value,"Not an error type");

			bool slot_print( std::ostream &, error_id ) const;

		protected:

			slot() noexcept;
			~slot() noexcept;

		public:

			using base::put;
			using base::has_value;
			using base::value;
			using base::reset;
			using base::emplace;
		};

		template <class E>
		slot<E> * & tl_slot_ptr() noexcept
		{
			static thread_local slot<E> * s;
			return s;
		}

		template <class E>
		void put_unexpected( id_e_pair<E> const & id_e ) noexcept
		{
			if( slot<diagnostic_info> * p = tl_slot_ptr<diagnostic_info>() )
			{
				if( p->has_value() )
				{
					auto & p_id_e = p->value();
					if( p_id_e.id==id_e.id )
					{
						++p_id_e.e.count;
						return;
					}
				}
				(void) p->put( id_e_pair<diagnostic_info>(id_e.id,diagnostic_info(&type<E>)) );
			}
		}

		template <class E>
		void put_verbose_diagnostic_info( id_e_pair<E> const & id_e ) noexcept
		{
			if( slot<verbose_diagnostic_info> * sl = tl_slot_ptr<verbose_diagnostic_info>() )
			{
				if( auto * pv = sl->has_value() )
				{
					if( pv->id!=id_e.id )
					{
						pv->id = id_e.id;
						pv->e.reset();
					}
					pv->e.add(id_e.e);
				}
				else
					sl->emplace(id_e.id).e.add(id_e.e);
			}
		}

		template <class E>
		void no_expect_slot( id_e_pair<E> const & id_e ) noexcept
		{
			put_unexpected(id_e);
			put_verbose_diagnostic_info(id_e);
		}

		template <class E>
		int put_slot( E && e, error_id id ) noexcept
		{
			using T = typename std::remove_cv<typename std::remove_reference<E>::type>::type;
			if( slot<T> * p = tl_slot_ptr<T>() )
				(void) p->put( id_e_pair<T>(id,std::forward<E>(e)) );
			else
			{
				int c = tl_unexpected_enabled_counter();
				assert(c>=0);
				if( c )
					no_expect_slot( id_e_pair<T>(id,std::forward<E>(e)) );
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
				optional<id_e_pair<E>> & p = *prev_;
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

		template <class E>
		bool slot<E>::slot_print( std::ostream & os, error_id id ) const
		{
			if( tl_slot_ptr<E>()==this )
				if( id_e_pair<E> const * id_e = has_value() )
					if( id_e->id==id )
						return diagnostic<decltype(id_e->e)>::print(os,id_e->e);
			return false;
		}

		template <class... E>
		error_id make_error( char const * file, int line, char const * function, E && ... e ) noexcept
		{
			assert(file&&*file);
			assert(line>0);
			assert(function&&*function);
			e_source_location sl { file, line, function }; // Temp object MSVC workaround
			return error_id( std::move(sl), std::forward<E>(e)... );
		}

		enum class result_variant
		{
			value,
			err,
			cap
		};
	} // leaf_detail

	template <class... E>
	error_id error_id::propagate( E && ... e ) const noexcept
	{
		auto _ = { leaf_detail::put_slot(std::forward<E>(e),*this)... };
		(void) _;
		return *this;
	}

	inline error_id get_error_id( error_id id ) noexcept
	{
		return id;
	}

} }

#endif
