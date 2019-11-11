#ifndef LEAF_BA049396D0D411E8B45DF7D4A759E189
#define LEAF_BA049396D0D411E8B45DF7D4A759E189

// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/function_traits.hpp>
#include <boost/leaf/detail/optional.hpp>
#include <boost/leaf/detail/print.hpp>
#include <system_error>
#include <type_traits>
#include <ostream>
#include <sstream>
#include <memory>
#include <set>

#ifdef LEAF_NO_THREADS
#	define LEAF_THREAD_LOCAL
	namespace boost { namespace leaf {
		namespace leaf_detail
		{
			using atomic_unsigned_int = unsigned int;
		}
	} }
#else
#	include <atomic>
#	include <thread>
#	define LEAF_THREAD_LOCAL thread_local
	namespace boost { namespace leaf {
		namespace leaf_detail
		{
			using atomic_unsigned_int = std::atomic<unsigned int>;
		}
	} }
#endif

#define LEAF_NEW_ERROR(...) ::boost::leaf::leaf_detail::new_error_at(__FILE__,__LINE__,__FUNCTION__).load(__VA_ARGS__)

#define LEAF_AUTO(v,r)\
	static_assert(::boost::leaf::is_result_type<typename std::decay<decltype(r)>::type>::value, "LEAF_AUTO requires a result type");\
	auto && _r_##v = r;\
	if( !_r_##v )\
		return _r_##v.error();\
	auto && v = _r_##v.value()

#define LEAF_CHECK(r)\
	{\
		static_assert(::boost::leaf::is_result_type<typename std::decay<decltype(r)>::type>::value, "LEAF_CHECK requires a result type");\
		auto const & _r = r;\
		if( !_r )\
			return _r.error();\
	}

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template<class T> using has_value_impl = decltype( std::declval<T>().value );

		template <class T> using has_value = leaf_detail_mp11::mp_valid<has_value_impl, T>;

		template <class T>
		struct is_error_type_default
		{
			static constexpr bool value = has_value<T>::value || std::is_base_of<std::exception,T>::value;
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
		class slot_base
		{
			slot_base & operator=( slot_base const & ) = delete;
			slot_base( slot_base const & ) = delete;

#ifndef LEAF_NO_DIAGNOSTIC_INFO
			virtual bool slot_print( std::ostream &, int err_id ) const = 0;
#endif

		public:

#ifndef LEAF_NO_DIAGNOSTIC_INFO
			static void print_all( std::ostream & os, int err_id )
			{
				for( slot_base const * p = first(); p; p=p->next_ )
					if( p->slot_print(os,err_id) )
						os << std::endl;
			}
#endif

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
				static LEAF_THREAD_LOCAL slot_base * p = 0;
				return p;
			}

			int err_id_;
			slot_base * next_;

			slot_base() noexcept:
				err_id_(0),
				next_(0)
			{
			}

			slot_base( slot_base && x ) noexcept:
				err_id_(std::move(x.err_id_)),
				next_(0)
			{
				assert(x.next_==0);
			}

			~slot_base() noexcept
			{
				assert(next_ == 0);
			}

			void activate() noexcept
			{
				assert(next_ == 0);
				slot_base * * f = &first();
				next_ = *f;
				*f = this;
			}

			void deactivate() noexcept
			{
				slot_base * * f = &first();
				assert(*f == this);
				*f = next_;
				next_ = 0;
			}
		};
	}

	////////////////////////////////////////

#ifndef LEAF_DISCARD_UNEXPECTED

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
			static constexpr bool is_invisible = true;
			static void print( std::ostream &, e_unexpected_count const & ) noexcept
			{
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
				if( !leaf_detail::diagnostic<E>::is_invisible )
				{
					leaf_detail::diagnostic<E>::print(s,e);
					if( already_.insert(&type<E>).second  )
					{
						s << std::endl;
						s_ += s.str();
					}
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
			static constexpr bool is_invisible = true;
			static void print( std::ostream &, e_unexpected_info const & ) noexcept
			{
			}
		};
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		inline int & tl_unexpected_enabled_counter() noexcept
		{
			static LEAF_THREAD_LOCAL int c;
			return c;
		}
	}

#endif

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class E>
		class slot;

		template <class E>
		inline slot<E> * & tl_slot_ptr() noexcept
		{
			static LEAF_THREAD_LOCAL slot<E> * s;
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
			slot<E> * * top_;
			slot<E> * prev_;
			static_assert(is_e_type<E>::value,"Not an error type");

#ifndef LEAF_NO_DIAGNOSTIC_INFO
			bool slot_print( std::ostream & os, int err_id ) const final override
			{
				if( !diagnostic<E>::is_invisible && *top_==this )
					if( E const * e = has_value(err_id) )
					{
						assert(err_id);
						diagnostic<decltype(*e)>::print(os, *e);
						return true;
					}
				return false;
			}
#endif

		public:

			slot() noexcept:
				top_(0)
			{
			}

			slot( slot && x ) noexcept:
				slot_base(std::move(x)),
				optional<E>(std::move(x)),
				top_(0)
			{
				assert(x.top_==0);
			}

			~slot() noexcept
			{
				assert(top_==0);
			}

			void activate() noexcept
			{
				assert(top_==0);
				slot_base::activate();
				top_ = &tl_slot_ptr<E>();
				prev_ = *top_;
				*top_ = this;
			}

			void deactivate( bool propagate_errors ) noexcept;

			E & load( int err_id, E const & e ) noexcept
			{
				assert(err_id);
				E & ret = impl::put(e);
				err_id_ = err_id;
				return ret;
			}

			E & load( int err_id, E && e ) noexcept
			{
				assert(err_id);
				E & ret = impl::put(std::forward<E>(e));
				err_id_ = err_id;
				return ret;
			}

			E const * has_value( int err_id ) const noexcept
			{
				if( err_id == err_id_ )
				{
					assert(err_id);
					if( E const * e = impl::has_value() )
						return e;
				}
				return 0;
			}

			E * has_value( int err_id ) noexcept
			{
				if( err_id == err_id_ )
				{
					assert(err_id);
					if( E * e = impl::has_value() )
						return e;
				}
				return 0;
			}

			E extract( int err_id ) noexcept
			{
				assert(has_value(err_id));
				err_id_ = 0;
				optional<E> & opt(*this);
				return std::move(opt).value();
			}

			bool print( std::ostream & os ) const
			{
				os << '[' << err_id_ << "]: ";
				if( E const * e = impl::has_value() )
				{
					diagnostic<decltype(*e)>::print(os, *e);
					os << std::endl;
				}
				else
					os << type<E>() << ": {Empty}" << std::endl;
				return true;
			}
		};

#ifndef LEAF_DISCARD_UNEXPECTED

		template <class E>
		inline void load_unexpected_count( int err_id, E const & e ) noexcept
		{
			if( slot<e_unexpected_count> * sl = tl_slot_ptr<e_unexpected_count>() )
				if( e_unexpected_count * unx = sl->has_value(err_id) )
					++unx->count;
				else
					sl->load(err_id, e_unexpected_count(&type<E>));
		}

		template <class E>
		inline void load_unexpected_info( int err_id, E const & e ) noexcept
		{
			if( slot<e_unexpected_info> * sl = tl_slot_ptr<e_unexpected_info>() )
				if( e_unexpected_info * unx = sl->has_value(err_id) )
					unx->add(e);
				else
					sl->load(err_id, e_unexpected_info()).add(e);
		}

		template <class E>
		inline void no_expect_slot( int err_id, E const & e  ) noexcept
		{
			load_unexpected_count(err_id, e);
			load_unexpected_info(err_id, e);
		}

#endif

		template <class E>
		inline void slot<E>::deactivate( bool propagate_errors ) noexcept
		{
			assert(top_!=0);
			if( propagate_errors )
				if( prev_ )
				{
					if( err_id_ )
					{
						optional<E> & p = *prev_;
						p = std::move(*this);
						prev_->err_id_ = err_id_;
					}
				}
#ifndef LEAF_DISCARD_UNEXPECTED
				else
				{
					int c = tl_unexpected_enabled_counter();
					assert(c>=0);
					if( c )
						if( E const * e = impl::has_value() )
							no_expect_slot(err_id_, *e);
				}
#endif
			*top_ = prev_;
			top_ = 0;
			slot_base::deactivate();
		}

		template <class E>
		inline int load_slot( int err_id, E && e ) noexcept
		{
			using T = typename std::decay<E>::type;
			assert(err_id);
			if( slot<T> * p = tl_slot_ptr<T>() )
				(void) p->load(err_id, std::forward<E>(e));
#ifndef LEAF_DISCARD_UNEXPECTED
			else
			{
				int c = tl_unexpected_enabled_counter();
				assert(c>=0);
				if( c )
					no_expect_slot(err_id, std::forward<E>(e));
			}
#endif
			return 0;
		}

		template <class F>
		inline int accumulate_slot( int err_id, F && f ) noexcept
		{
			static_assert(function_traits<F>::arity==1, "Lambdas passed to accumulate must take a single e-type argument by reference");
			using E = typename std::decay<fn_arg_type<F,0>>::type;
			static_assert(is_e_type<E>::value, "Lambdas passed to accumulate must take a single e-type argument by reference");
			assert(err_id);
			if( auto sl = tl_slot_ptr<E>() )
				if( auto v = sl->has_value(err_id) )
					(void) std::forward<F>(f)(*v);
				else
					(void) std::forward<F>(f)(sl->load(err_id,E()));
			return 0;
		}
	} // leaf_detail

	////////////////////////////////////////

	namespace leaf_detail
	{
		//Starting with first_id, all error IDs are odd numbers, in order to ensure
		//that the sequence will not include 0, which is used to indicate no error.
		constexpr int first_id = 1;
		static_assert(first_id&1, "first_id must be an odd number");

		template <class=void>
		struct id_factory
		{
			static atomic_unsigned_int counter;
			static LEAF_THREAD_LOCAL int last_id;
			static LEAF_THREAD_LOCAL int next_id;

			static int generate_next_id() noexcept
			{
				unsigned id = (counter+=2u);
				assert(id&1u);
				return int(id);
			}
		};

		template <class T>
		atomic_unsigned_int id_factory<T>::counter(first_id-2u);

		template <class T>
		LEAF_THREAD_LOCAL int id_factory<T>::last_id(0);

		template <class T>
		LEAF_THREAD_LOCAL int id_factory<T>::next_id(0);

		inline int last_id() noexcept
		{
			int id = id_factory<>::last_id;
			assert(id==0 || (id&1));
			return id;
		}

		inline int next_id() noexcept
		{
			if( int id = id_factory<>::next_id )
			{
				assert(id&1);
				return id;
			}
			else
				return id_factory<>::next_id = id_factory<>::generate_next_id();
		}

		inline int new_id() noexcept
		{
			int id = next_id();
			assert(id&1);
			int next = id_factory<>::generate_next_id();
			assert(next&1);
			id_factory<>::last_id = id;
			id_factory<>::next_id = next;
			return id;
		}
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		struct e_original_ec { std::error_code value; };

		class leaf_category: public std::error_category
		{
			bool equivalent( int,  std::error_condition const & ) const noexcept final override { return false; }
			bool equivalent( std::error_code const &, int ) const noexcept final override { return false; }
			char const * name() const noexcept final override { return "LEAF error"; }
			std::string message( int condition ) const final override { return name(); }
		public:
			~leaf_category() noexcept final override { }
		};

		template <class=void>
		struct get_error_category
		{
			static leaf_category cat;
		};

		template <class T>
		leaf_category get_error_category<T>::cat;

		inline int import_error_code( std::error_code const & ec ) noexcept
		{
			int err_id = ec.value();
			if( err_id )
			{
				std::error_category const & cat = leaf_detail::get_error_category<>::cat;
				if( &ec.category()!=&cat )
				{
					err_id = leaf_detail::new_id();
					leaf_detail::load_slot(err_id, leaf_detail::e_original_ec{ec});
					return err_id;
				}
			}
			return err_id;
		}

		inline bool is_error_id( std::error_code const & ec ) noexcept
		{
			bool res = (&ec.category() == &leaf_detail::get_error_category<>::cat);
			assert(!res || !ec.value() || (ec.value()&1));
			return res;
		}
}

	////////////////////////////////////////

	class error_id;

	namespace leaf_detail
	{
		error_id make_error_id(int) noexcept;
	}

	class error_id
	{
		friend error_id leaf_detail::make_error_id(int ) noexcept;

		int value_;

		explicit error_id( int value ) noexcept:
			value_(value)
		{
			assert(value_==0 || (value_&1));
		}

	public:

		error_id() noexcept:
			value_(0)
		{
		}

		error_id( std::error_code const & ec ) noexcept:
			value_(leaf_detail::import_error_code(ec))
		{
			assert(!value_ || (value_&1));
		}

		error_id load() const noexcept
		{
			return *this;
		}

		template <class... E>
		error_id load( E && ... e ) const noexcept
		{
			if( int err_id = value() )
			{
				auto _ = { leaf_detail::load_slot(err_id, std::forward<E>(e))... };
				(void) _;
			}
			return *this;
		}

		error_id accumulate() const noexcept
		{
			return *this;
		}

		template <class... F>
		error_id accumulate( F && ... f ) const noexcept
		{
			if( int err_id = value() )
			{
				auto _ = { leaf_detail::accumulate_slot(err_id, std::forward<F>(f))... };
				(void) _;
			}
			return *this;
		}

		template<class T, class E = typename std::enable_if<
			std::is_nothrow_constructible<T, std::error_code>::value &&
			std::is_convertible<std::error_code, T>::value
			>::type>
		operator T() const noexcept
		{
			return std::error_code(value(), leaf_detail::get_error_category<>::cat);
		}

		int value() const noexcept
		{
			assert(!value_ || (value_&1));
			return value_;
		}

		explicit operator bool() const noexcept
		{
			return value()!=0;
		}

		friend bool operator==( error_id a, error_id b ) noexcept
		{
			return a.value()==b.value();
		}

		friend bool operator!=( error_id a, error_id b ) noexcept
		{
			return !(a==b);
		}

		friend bool operator<( error_id a, error_id b ) noexcept
		{
			return a.value()<b.value();
		}

		friend std::ostream & operator<<( std::ostream & os, error_id x )
		{
			return os<<x.value();
		}
	};

	namespace leaf_detail
	{
		inline error_id make_error_id( int err_id ) noexcept
		{
			return error_id(err_id);
		}
	}

	inline error_id new_error() noexcept
	{
		return leaf_detail::make_error_id(leaf_detail::new_id());
	}

	template <class E1, class... E>
	inline typename std::enable_if<is_e_type<E1>::value, error_id>::type new_error( E1 && e1, E && ... e ) noexcept
	{
		return leaf_detail::make_error_id(leaf_detail::new_id()).load(std::forward<E1>(e1), std::forward<E>(e)...);
	}

	template <class E1, class... E>
	inline typename std::enable_if<std::is_same<std::error_code, decltype(make_error_code(std::declval<E1>()))>::value, error_id>::type new_error( E1 const & e1, E && ... e ) noexcept
	{
		return error_id(make_error_code(e1)).load(std::forward<E>(e)...);
	}

	inline error_id next_error() noexcept
	{
		return leaf_detail::make_error_id(leaf_detail::next_id());
	}

	namespace leaf_detail
	{
		template <class... E>
		inline error_id new_error_at( char const * file, int line, char const * function ) noexcept
		{
			assert(file&&*file);
			assert(line>0);
			assert(function&&*function);
			e_source_location sl { file, line, function }; // Temp object MSVC workaround
			return new_error(std::move(sl));
		}
	}

	////////////////////////////////////////////

	class polymorphic_context
	{
	protected:
		polymorphic_context() noexcept = default;
		~polymorphic_context() noexcept = default;
	public:
		virtual int propagate_captured_errors() noexcept = 0;
		virtual void activate() noexcept = 0;
		virtual void deactivate( bool propagate_errors ) noexcept = 0;
		virtual bool is_active() const noexcept = 0;
		virtual void print( std::ostream & ) const = 0;
		error_id captured_id_;
	};

	using context_ptr = std::shared_ptr<polymorphic_context>;

	////////////////////////////////////////////

	enum class on_deactivation
	{
		propagate,
		do_not_propagate,
		propagate_if_uncaught_exception
	};

	class context_activator
	{
		context_activator( context_activator const & ) = delete;
		context_activator & operator=( context_activator const & ) = delete;

		void (* const deactivate_)(context_activator *, bool) noexcept;
		void * const ctx_;
		bool const ctx_was_active_;
		on_deactivation on_deactivate_;

		template <class Ctx>
		static void deactivate_fwd(context_activator * this_, bool propagate_errors) noexcept
		{
			assert(this_->deactivate_!=0);
			assert(this_->ctx_!=0);
			static_cast<Ctx *>(this_->ctx_)->deactivate(propagate_errors);
		}

	public:

		template <class Ctx>
		context_activator(Ctx & ctx, on_deactivation on_deactivate) noexcept:
			deactivate_(&deactivate_fwd<Ctx>),
			ctx_(&ctx),
			ctx_was_active_(ctx.is_active()),
			on_deactivate_(on_deactivate)
		{
			if( !ctx_was_active_ )
				ctx.activate();
		}

		~context_activator() noexcept
		{
			assert(
				on_deactivate_ == on_deactivation::propagate ||
				on_deactivate_ == on_deactivation::do_not_propagate ||
				on_deactivate_ == on_deactivation::propagate_if_uncaught_exception);
			if( !ctx_was_active_ )
				if( on_deactivate_ == on_deactivation::propagate_if_uncaught_exception )
				{
#ifdef LEAF_NO_EXCEPTIONS
					deactivate_(this, false);
#else
					bool has_exception = std::uncaught_exception();
					deactivate_(this, has_exception);
					if( !has_exception )
						(void) leaf_detail::new_id();
#endif
				}
				else
					deactivate_(this, on_deactivate_ == on_deactivation::propagate);
		}

		void set_on_deactivate( on_deactivation on_deactivate ) noexcept
		{
			on_deactivate_ = on_deactivate;
		}
	};

	////////////////////////////////////////////

	template <class R>
	struct is_result_type: std::false_type
	{
	};

	namespace leaf_detail
	{
		template <class R, bool IsResult = is_result_type<R>::value>
		struct is_result_tag;

		template <class R>
		struct is_result_tag<R, false>
		{
		};

		template <class R>
		struct is_result_tag<R, true>
		{
		};
	}

} }

#undef LEAF_THREAD_LOCAL

#endif
