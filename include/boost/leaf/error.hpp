#ifndef BOOST_LEAF_BA049396D0D411E8B45DF7D4A759E189
#define BOOST_LEAF_BA049396D0D411E8B45DF7D4A759E189

// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/optional.hpp>
#include <boost/leaf/detail/print.hpp>
#include <system_error>
#include <type_traits>
#include <ostream>
#include <sstream>
#include <atomic>
#include <set>

#define LEAF_NEW_ERROR(...) ::boost::leaf::leaf_detail::new_error_at(__FILE__,__LINE__,__FUNCTION__).propagate(__VA_ARGS__)

namespace boost { namespace leaf {

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

		template <>
		struct is_error_type_default<std::exception_ptr>: std::true_type
		{
		};
	}

	template <class T> struct is_e_type: leaf_detail::is_error_type_default<T> { };
	template <class T> struct is_e_type<T const>: is_e_type<T> { };
	template <class T> struct is_e_type<T const &>: is_e_type<T> { };
	template <class T> struct is_e_type<T &>: is_e_type<T> { };
	template <> struct is_e_type<std::error_code>: std::false_type { };

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

	template <>
	struct is_e_type<e_source_location>: std::true_type
	{
	};

	////////////////////////////////////////

	namespace leaf_detail
	{
		class id_factory
		{
			id_factory( id_factory const & ) = delete;
			id_factory & operator=( id_factory const & ) = delete;

			static int new_err_id() noexcept
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
				next_id_(new_err_id()),
				last_id_(next_id_-1)
			{
			}

		public:

			static id_factory & tl_instance() noexcept
			{
				static thread_local id_factory s;
				return s;
			}

			int last_id() noexcept
			{
				return last_id_;
			}

			int new_id() noexcept
			{
				int id = last_id_ = next_id_;
				next_id_ = new_err_id();
				return id;
			}

			int next_id() noexcept
			{
				return next_id_;
			}
		};

		inline int last_id() noexcept
		{
			return id_factory::tl_instance().last_id();
		}

		inline int new_id() noexcept
		{
			return id_factory::tl_instance().new_id();
		}

		inline int next_id() noexcept
		{
			return id_factory::tl_instance().next_id();
		}
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		class slot_base
		{
			slot_base( slot_base const & ) = delete;
			slot_base & operator=( slot_base const & ) = delete;

			virtual bool slot_print( std::ostream &, int err_id ) const = 0;

		public:

			static void print( std::ostream & os, int err_id )
			{
				assert(err_id);
				for( slot_base const * p = first(); p; p=p->next_ )
					if( p->slot_print(os,err_id) )
						os << std::endl;
			}

			static void reassign( int from_err_id, int to_err_id ) noexcept
			{
				assert(from_err_id);
				assert(to_err_id);
				for( slot_base * p = first(); p; p=p->next_ )
					if( p->err_id_==from_err_id )
						p->err_id_ = to_err_id;
			}

		protected:

			static slot_base * & first() noexcept
			{
				static thread_local slot_base * p = 0;
				return p;
			}

			int err_id_;
			slot_base * const next_;

			slot_base() noexcept:
				err_id_(0),
				next_(first())
			{
				first() = this;
			}

			~slot_base() noexcept
			{
				slot_base * & p = first();
				assert(p==this);
				p = next_;
			}
		};
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		class e_unexpected_count
		{
		public:

			char const * (*first_type)();
			int count;

			explicit e_unexpected_count( char const * (*first_type)() ) noexcept:
				first_type(first_type),
				count(1)
			{
			}

			void print( std::ostream & os ) const
			{
				assert(first_type!=0);
				assert(count>0);
				os << "Detected ";
				if( count==1 )
					os << "1 attempt to communicate an E-object";
				else
					os << count << " attempts to communicate unexpected E-objects, the first one";
				os << " of type " << first_type() << std::endl;
			}
		};

		template <>
		struct is_error_type_default<e_unexpected_count>: std::true_type
		{
		};

		template <>
		struct diagnostic<e_unexpected_count,false,false>
		{
			static bool print( std::ostream & os, e_unexpected_count const & ) noexcept
			{
				return false;
			}
		};

		class e_unexpected_info
		{
			std::string s_;
			std::set<char const *(*)()> already_;

		public:

			e_unexpected_info() noexcept
			{
			}

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

			void print( std::ostream & os ) const
			{
				os << s_;
			}
		};

		template <>
		struct is_error_type_default<e_unexpected_info>: std::true_type
		{
		};

		template <>
		struct diagnostic<e_unexpected_info,false,false>
		{
			static bool print( std::ostream & os, e_unexpected_info const & ) noexcept
			{
				return false;
			}
		};
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
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
		class slot;

		template <class E>
		slot<E> * & tl_slot_ptr() noexcept
		{
			static thread_local slot<E> * s;
			return s;
		}

		template <class E>
		class slot:
			slot_base,
			optional<E>
		{
			slot( slot const & ) = delete;
			slot & operator=( slot const & ) = delete;
			typedef optional<E> impl;
			slot<E> * & top_;
			slot<E> * prev_;
			static_assert(is_e_type<E>::value,"Not an error type");

			bool slot_print( std::ostream & os, int err_id ) const
			{
				assert(err_id);
				if( top_==this )
					if( E const * e = has_value(err_id) )
						return diagnostic<decltype(*e)>::print(os,*e);
				return false;
			}

		public:

			slot() noexcept:
				top_(tl_slot_ptr<E>())
			{
				prev_ = top_;
				top_ = this;
			}

			~slot() noexcept;

			E & put( int err_id, E const & e ) noexcept
			{
				assert(err_id);
				E & ret = impl::put(e);
				err_id_ = err_id;
				return ret;
			}

			E & put( int err_id, E && e ) noexcept
			{
				assert(err_id);
				E & ret = impl::put(std::forward<E>(e));
				err_id_ = err_id;
				return ret;
			}

			E const * has_value( int err_id ) const noexcept
			{
				assert(err_id);
				if( err_id == err_id_ )
					if( E const * e = impl::has_value() )
						return e;
				return 0;
			}

			E * has_value( int err_id ) noexcept
			{
				assert(err_id);
				if( err_id == err_id_ )
					if( E * e = impl::has_value() )
						return e;
				return 0;
			}

			optional<E> extract_optional( int err_id ) && noexcept
			{
				assert(err_id);
				if( err_id_ != err_id )
					return optional<E>();
				else
				{
					err_id_ = 0;
					return std::move(*static_cast<optional<E> *>(this));
				}
			}

			using impl::reset;
		};

		template <class E>
		void put_unexpected_count( int err_id, E const & e ) noexcept
		{
			if( slot<e_unexpected_count> * sl = tl_slot_ptr<e_unexpected_count>() )
				if( e_unexpected_count * unx = sl->has_value(err_id) )
					++unx->count;
				else
					sl->put(err_id, e_unexpected_count(&type<E>));
		}

		template <class E>
		void put_unexpected_info( int err_id, E const & e ) noexcept
		{
			if( slot<e_unexpected_info> * sl = tl_slot_ptr<e_unexpected_info>() )
				if( e_unexpected_info * unx = sl->has_value(err_id) )
					unx->add(e);
				else
					sl->put(err_id, e_unexpected_info()).add(e);
		}

		template <class E>
		void no_expect_slot( int err_id, E const & e  ) noexcept
		{
			put_unexpected_count(err_id, e);
			put_unexpected_info(err_id, e);
		}

		template <class E>
		slot<E>::~slot() noexcept
		{
			if( prev_ )
			{
				if( err_id_ )
				{
					optional<E> & p = *prev_;
					p = std::move(*this);
					prev_->err_id_ = err_id_;
				}
			}
			else
			{
				int c = tl_unexpected_enabled_counter();
				assert(c>=0);
				if( c )
					if( E const * e = impl::has_value() )
						no_expect_slot(err_id_, *e);
			}
			top_ = prev_;
		}

		template <class E>
		int put_slot( int err_id, E && e ) noexcept
		{
			using T = typename std::remove_cv<typename std::remove_reference<E>::type>::type;
			assert(err_id);
			if( slot<T> * p = tl_slot_ptr<T>() )
				(void) p->put(err_id, std::forward<E>(e));
			else
			{
				int c = tl_unexpected_enabled_counter();
				assert(c>=0);
				if( c )
					no_expect_slot(err_id, std::forward<E>(e));
			}
			return 0;
		}

		enum class result_variant
		{
			value,
			err_id,
			cap
		};
	} // leaf_detail

	////////////////////////////////////////

	namespace leaf_detail
	{
		struct e_original_ec { std::error_code value; };

		inline std::error_category const & get_error_category() noexcept
		{
			class cat: public std::error_category
			{
				bool equivalent( int,  std::error_condition const & ) const noexcept { return false; }
				bool equivalent( std::error_code const &, int ) const noexcept { return false; }
				char const * name() const noexcept { return "LEAF error, use with leaf::handle_some or leaf::handle_all."; }
				std::string message( int condition ) const { return name(); }
			};
			static cat c;
			return c;
		}
	}

	inline bool is_error_id( std::error_code const & ec ) noexcept
	{
		return &ec.category() == &leaf_detail::get_error_category();
	}

	class error_id: public std::error_code
	{
		template <class ErrorCode>
		static std::error_code import( ErrorCode && ec ) noexcept
		{
			std::error_category const & cat = leaf_detail::get_error_category();
			if( ec && &ec.category()!=&cat )
			{
				int err_id = leaf_detail::new_id();
				leaf_detail::put_slot(err_id, leaf_detail::e_original_ec{ec});
				return std::error_code(err_id, cat);
			}
			else
				return ec;
		}

	public:

		error_id() noexcept = default;

		error_id( std::error_code const & ec ) noexcept:
			error_code(import(ec))
		{
		}

		error_id( std::error_code && ec ) noexcept:
			error_code(import(std::move(ec)))
		{
		}

		error_id const & propagate() const noexcept
		{
			return *this;
		}

		template <class... E>
		error_id const & propagate( E && ... e ) const noexcept
		{
			if( int err_id = value() )
			{
				auto _ = { leaf_detail::put_slot(err_id, std::forward<E>(e))... };
				(void) _;
			}
			return *this;
		}
	};

	namespace leaf_detail
	{
		inline error_id make_error_id( int err_id ) noexcept
		{
			assert(err_id);
			return std::error_code(err_id, get_error_category());
		}
	}

	inline error_id new_error() noexcept
	{
		return leaf_detail::make_error_id(leaf_detail::new_id());
	}

	template <class E1, class... E>
	typename std::enable_if<is_e_type<E1>::value, error_id>::type new_error( E1 && e1, E && ... e ) noexcept
	{
		return leaf_detail::make_error_id(leaf_detail::new_id()).propagate(std::forward<E1>(e1), std::forward<E>(e)...);
	}

	template <class E1, class... E>
	typename std::enable_if<std::is_same<std::error_code, decltype(make_error_code(std::declval<E1>()))>::value, error_id>::type new_error( E1 e1, E && ... e ) noexcept
	{
		return error_id(make_error_code(e1)).propagate(std::forward<E>(e)...);
	}

	inline error_id next_error() noexcept
	{
		return leaf_detail::make_error_id(leaf_detail::next_id());
	}

	namespace leaf_detail
	{
		template <class... E>
		error_id new_error_at( char const * file, int line, char const * function ) noexcept
		{
			assert(file&&*file);
			assert(line>0);
			assert(function&&*function);
			e_source_location sl { file, line, function }; // Temp object MSVC workaround
			return new_error(std::move(sl));
		}
	}

} }

#endif
