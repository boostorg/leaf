#ifndef BOOST_LEAF_BA049396D0D411E8B45DF7D4A759E189
#define BOOST_LEAF_BA049396D0D411E8B45DF7D4A759E189

//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

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
		std::set<char const *(*)()> already;
	};

	namespace leaf_detail
	{
		template <>
		struct diagnostic<e_unexpected_diagnostic_output,false,true>
		{
			static bool print( std::ostream & os, e_unexpected_diagnostic_output const & x ) noexcept
			{
				if( x.value.empty() )
					return false;
				else
				{
					os << x.value;
					return true;
				}
			}
		};

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
		static constexpr bool value = leaf_detail::has_data_member_value<T>::value || std::is_base_of<std::exception,T>::value;
	};

	template <> struct is_error_type<std::exception_ptr>: std::true_type { };
	template <> struct is_error_type<std::error_code>: std::true_type { };
	template <> struct is_error_type<system::error_code>: std::true_type { };
	template <> struct is_error_type<e_unexpected>: std::true_type { };
	template <> struct is_error_type<e_source_location>: std::true_type { };

	////////////////////////////////////////

	class error;

	error next_error_value() noexcept;
	error last_error_value() noexcept;

	class error
	{
		template <class... E>
		friend error new_error( E && ... ) noexcept;
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

		error( error const & ) noexcept = default;
		error( error && ) noexcept = default;

		friend bool operator==( error const & e1, error const & e2 ) noexcept
		{
			return e1.id_==e2.id_;
		}

		friend bool operator!=( error const & e1, error const & e2 ) noexcept
		{
			return e1.id_!=e2.id_;
		}

		error propagate() const noexcept
		{
			return *this;
		}

		friend std::ostream & operator<<( std::ostream & os, error const & e )
		{
			os << e.id_;
			return os;
		}

		template <class... E>
		error propagate( E && ... ) const noexcept;

		void diagnostic_output( std::ostream & os ) const;
	};

	template <class... E>
	error new_error( E && ... e ) noexcept
	{
		return error(error::id_factory::tl_instance().get()).propagate(std::forward<E>(e)...);
	}

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

		class slot_base
		{
			slot_base( slot_base const & ) = delete;
			slot_base & operator=( slot_base const & ) = delete;

			virtual bool slot_diagnostic_output( std::ostream &, error const * e ) const = 0;

		public:

			static void diagnostic_output( std::ostream & os, error const * e )
			{
				for( slot_base const * p = first(); p; p=p->next_ )
					if( p->slot_diagnostic_output(os,e) )
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

		template <class E>
		class slot:
			slot_base,
			optional<error_info<E>>
		{
			slot( slot const & ) = delete;
			slot & operator=( slot const & ) = delete;
			typedef optional<error_info<E>> base;
			slot<E> * prev_;
			static_assert(is_error_type<E>::value,"Not an error type");

			bool slot_diagnostic_output( std::ostream & os, error const * e ) const;

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
						if( p_ev.v.already.insert(&type<E>).second )
						{
							std::string & value = p_ev.v.value;
							value += '\n';
							value += s.str();
							value += " {unexpected}";
						}
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

		template <class E>
		bool slot<E>::slot_diagnostic_output( std::ostream & os, error const * e ) const
		{
			if( tl_slot_ptr<E>()==this )
				if( error_info<E> const * ev = has_value() )
					if( e )
					{
						if( ev->e==*e )
							return diagnostic<decltype(ev->v)>::print(os,ev->v);
					}
					else
					{
						if( diagnostic<decltype(ev->v)>::print(os,ev->v) )
						{
							os << " {" << ev->e << '}';
							return true;
						}
					}
			return false;
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

		enum class result_variant
		{
			value,
			err,
			cap
		};
	} //leaf_detail

	template <class... E>
	error error::propagate( E && ... e ) const noexcept
	{
		auto _ = { leaf_detail::put_slot(std::forward<E>(e),*this)... };
		(void) _;
		return *this;
	}

	inline void error::diagnostic_output( std::ostream & os ) const
	{
		os << "leaf::error serial number: " << *this << std::endl;
		leaf_detail::slot_base::diagnostic_output(os,this);
	}

	inline void global_diagnostic_output( std::ostream & os )
	{
		leaf_detail::slot_base::diagnostic_output(os,0);
	}
} }

#endif
