#ifndef LEAF_BA049396D0D411E8B45DF7D4A759E189
#define LEAF_BA049396D0D411E8B45DF7D4A759E189

// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/function_traits.hpp>
#include <boost/leaf/detail/print.hpp>
#include <system_error>
#include <type_traits>
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
		auto && _r = r;\
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

#if LEAF_DIAGNOSTICS

	namespace leaf_detail
	{
		class e_unexpected_count
		{
		public:

			char const * (*first_type)();
			int count;

			LEAF_CONSTEXPR explicit e_unexpected_count( char const * (*first_type)() ) noexcept:
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
					os << "1 attempt to communicate an unexpected error object";
				else
					os << count << " attempts to communicate unexpected error objects, the first one";
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
			LEAF_CONSTEXPR static void print( std::ostream &, e_unexpected_count const & ) noexcept
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
				os << "Unexpected error objects:\n" << s_;
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
			LEAF_CONSTEXPR static void print( std::ostream &, e_unexpected_info const & ) noexcept
			{
			}
		};

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
			optional<E>
		{
			slot( slot const & ) = delete;
			slot & operator=( slot const & ) = delete;

			typedef optional<E> impl;
			slot<E> * * top_;
			slot<E> * prev_;
			static_assert(is_e_type<E>::value,"Not an error type");

		public:

			LEAF_CONSTEXPR slot() noexcept:
				top_(0)
			{
			}

			LEAF_CONSTEXPR slot( slot && x ) noexcept:
				optional<E>(std::move(x)),
				top_(0)
			{
				assert(x.top_==0);
			}

			LEAF_CONSTEXPR void activate() noexcept
			{
				assert(top_==0 || *top_!=this);
				top_ = &tl_slot_ptr<E>();
				prev_ = *top_;
				*top_ = this;
			}


			LEAF_CONSTEXPR void deactivate() noexcept
			{
				assert(top_!=0 && *top_==this);
				*top_ = prev_;
			}

			LEAF_CONSTEXPR void propagate() noexcept;

			using impl::put;
			using impl::has_value;
			using impl::value;
			using impl::print;
		};

#if LEAF_DIAGNOSTICS

		template <class E>
		LEAF_CONSTEXPR inline void load_unexpected_count( int err_id ) noexcept
		{
			if( slot<e_unexpected_count> * sl = tl_slot_ptr<e_unexpected_count>() )
				if( e_unexpected_count * unx = sl->has_value(err_id) )
					++unx->count;
				else
					sl->put(err_id, e_unexpected_count(&type<E>));
		}

		template <class E>
		LEAF_CONSTEXPR inline void load_unexpected_info( int err_id, E && e ) noexcept
		{
			if( slot<e_unexpected_info> * sl = tl_slot_ptr<e_unexpected_info>() )
				if( e_unexpected_info * unx = sl->has_value(err_id) )
					unx->add(e);
				else
					sl->put(err_id, e_unexpected_info()).add(e);
		}

		template <class E>
		LEAF_CONSTEXPR inline void load_unexpected( int err_id, E && e  ) noexcept
		{
			load_unexpected_count<E>(err_id);
			load_unexpected_info(err_id, std::move(e));
		}

#endif

		template <class E>
		LEAF_CONSTEXPR inline void slot<E>::propagate() noexcept
		{
			assert(top_!=0 && (*top_==prev_ || *top_==this));
			if( prev_ )
			{
				impl & this_ = *this;
				impl & that_ = *prev_;
				that_ = std::move(this_);
			}
#if LEAF_DIAGNOSTICS
			else
			{
				int c = tl_unexpected_enabled_counter();
				assert(c>=0);
				if( c )
					if( int err_id = impl::key() )
						load_unexpected(err_id, std::move(*this).value(err_id));
			}
#endif
		}

		template <class E>
		LEAF_CONSTEXPR inline int load_slot( int err_id, E && e ) noexcept
		{
			using T = typename std::decay<E>::type;
			assert((err_id&3)==1);
			if( slot<T> * p = tl_slot_ptr<T>() )
				(void) p->put(err_id, std::forward<E>(e));
#if LEAF_DIAGNOSTICS
			else
			{
				int c = tl_unexpected_enabled_counter();
				assert(c>=0);
				if( c )
					load_unexpected(err_id, std::forward<E>(e));
			}
#endif
			return 0;
		}

		template <class F>
		LEAF_CONSTEXPR inline int accumulate_slot( int err_id, F && f ) noexcept
		{
			static_assert(function_traits<F>::arity==1, "Lambdas passed to accumulate must take a single e-type argument by reference");
			using E = typename std::decay<fn_arg_type<F,0>>::type;
			static_assert(is_e_type<E>::value, "Lambdas passed to accumulate must take a single e-type argument by reference");
			assert((err_id&3)==1);
			if( auto sl = tl_slot_ptr<E>() )
				if( auto v = sl->has_value(err_id) )
					(void) std::forward<F>(f)(*v);
				else
					(void) std::forward<F>(f)(sl->put(err_id,E()));
			return 0;
		}
	} // leaf_detail

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class=void>
		struct id_factory
		{
			static atomic_unsigned_int counter;
			static LEAF_THREAD_LOCAL unsigned current_id;

			LEAF_CONSTEXPR static unsigned generate_next_id() noexcept
			{
				auto id = (counter+=4);
				assert((id&3)==1);
				return id;
			}
		};

		template <class T>
		atomic_unsigned_int id_factory<T>::counter(-3);

		template <class T>
		LEAF_THREAD_LOCAL unsigned id_factory<T>::current_id(0);

		inline int current_id() noexcept
		{
			auto id = id_factory<>::current_id;
			assert(id==0 || (id&3)==1);
			return id;
		}

		inline int new_id() noexcept
		{
			return id_factory<>::current_id = id_factory<>::generate_next_id();
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
			if( int err_id = ec.value() )
			{
				std::error_category const & cat = leaf_detail::get_error_category<>::cat;
				if( &ec.category()==&cat )
				{
					assert((err_id&3)==1);
					return (err_id&~3)|1;
				}
				else
				{
					err_id = leaf_detail::new_id();
					leaf_detail::load_slot(err_id, leaf_detail::e_original_ec{ec});
					return (err_id&~3)|1;
				}
			}
			else
				return 0;
		}

		inline bool is_error_id( std::error_code const & ec ) noexcept
		{
			bool res = (&ec.category() == &leaf_detail::get_error_category<>::cat);
			assert(!res || !ec.value() || ((ec.value()&3)==1));
			return res;
		}
	}

	////////////////////////////////////////

	class error_id;

	namespace leaf_detail
	{
		LEAF_CONSTEXPR error_id make_error_id(int) noexcept;
	}

	class error_id
	{
		friend error_id LEAF_CONSTEXPR leaf_detail::make_error_id(int) noexcept;

		int value_;

		LEAF_CONSTEXPR explicit error_id( int value ) noexcept:
			value_(value)
		{
			assert(value_==0 || ((value_&3)==1));
		}

	public:

		LEAF_CONSTEXPR error_id() noexcept:
			value_(0)
		{
		}

		error_id( std::error_code const & ec ) noexcept:
			value_(leaf_detail::import_error_code(ec))
		{
			assert(!value_ || ((value_&3)==1));
		}

		LEAF_CONSTEXPR error_id load() const noexcept
		{
			return *this;
		}

		template <class... E>
		LEAF_CONSTEXPR error_id load( E && ... e ) const noexcept
		{
			if( int err_id = value() )
			{
				auto _ = { leaf_detail::load_slot(err_id, std::forward<E>(e))... };
				(void) _;
			}
			return *this;
		}

		LEAF_CONSTEXPR error_id accumulate() const noexcept
		{
			return *this;
		}

		template <class... F>
		LEAF_CONSTEXPR error_id accumulate( F && ... f ) const noexcept
		{
			if( int err_id = value() )
			{
				auto _ = { leaf_detail::accumulate_slot(err_id, std::forward<F>(f))... };
				(void) _;
			}
			return *this;
		}

		std::error_code to_error_code() const noexcept
		{
			return std::error_code(value_, leaf_detail::get_error_category<>::cat);
		}

		LEAF_CONSTEXPR int value() const noexcept
		{
			if( int v = value_ )
			{
				assert((v&3)==1);
				return (v&~3)|1;
			}
			else
				return 0;
		}

		LEAF_CONSTEXPR explicit operator bool() const noexcept
		{
			return value_ != 0;
		}

		LEAF_CONSTEXPR friend bool operator==( error_id a, error_id b ) noexcept
		{
			return a.value_ == b.value_;
		}

		LEAF_CONSTEXPR friend bool operator!=( error_id a, error_id b ) noexcept
		{
			return !(a == b);
		}

		LEAF_CONSTEXPR friend bool operator<( error_id a, error_id b ) noexcept
		{
			return a.value_ < b.value_;
		}

		friend std::ostream & operator<<( std::ostream & os, error_id x )
		{
			return os << x.value_;
		}
	};

	namespace leaf_detail
	{
		LEAF_CONSTEXPR inline error_id make_error_id( int err_id ) noexcept
		{
			assert(err_id==0 || (err_id&3)==1);
			return error_id((err_id&~3)|1);
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

	inline error_id current_error() noexcept
	{
		return leaf_detail::make_error_id(leaf_detail::current_id());
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
		virtual error_id propagate_captured_errors() noexcept = 0;
		virtual void activate() noexcept = 0;
		virtual void deactivate() noexcept = 0;
		virtual void propagate() noexcept = 0;
		virtual bool is_active() const noexcept = 0;
		virtual void print( std::ostream & ) const = 0;
		error_id captured_id_;
	};

	using context_ptr = std::shared_ptr<polymorphic_context>;

	////////////////////////////////////////////

	template <class Ctx>
	class context_activator
	{
		context_activator( context_activator const & ) = delete;
		context_activator & operator=( context_activator const & ) = delete;

#if !defined(LEAF_NO_EXCEPTIONS) && LEAF_STD_UNCAUGHT_EXCEPTIONS
		int const uncaught_exceptions_;
#endif
		Ctx * ctx_;

	public:

		explicit LEAF_CONSTEXPR LEAF_ALWAYS_INLINE context_activator(Ctx & ctx) noexcept:
#if !defined(LEAF_NO_EXCEPTIONS) && LEAF_STD_UNCAUGHT_EXCEPTIONS
			uncaught_exceptions_(std::uncaught_exceptions()),
#endif
			ctx_(ctx.is_active() ? 0 : &ctx)
		{
			if( ctx_ )
				ctx_->activate();
		}

		LEAF_CONSTEXPR LEAF_ALWAYS_INLINE context_activator( context_activator && x ) noexcept:
#if !defined(LEAF_NO_EXCEPTIONS) && LEAF_STD_UNCAUGHT_EXCEPTIONS
			uncaught_exceptions_(x.uncaught_exceptions_),
#endif
			ctx_(x.ctx_)
		{
			x.ctx_ = 0;
		}

		LEAF_ALWAYS_INLINE ~context_activator() noexcept
		{
			if( !ctx_ )
				return;
			if( ctx_->is_active() )
				ctx_->deactivate();
#ifndef LEAF_NO_EXCEPTIONS
#	if LEAF_STD_UNCAUGHT_EXCEPTIONS
			if( std::uncaught_exceptions() > uncaught_exceptions_ )
#	else
			if( std::uncaught_exception() )
#	endif
				ctx_->propagate();
			else
				(void) leaf_detail::new_id();
#endif
		}
	};

	template <class Ctx>
	LEAF_CONSTEXPR LEAF_ALWAYS_INLINE context_activator<Ctx> activate_context(Ctx & ctx) noexcept
	{
		return context_activator<Ctx>(ctx);
	}

	////////////////////////////////////////////

	template <class R>
	struct is_result_type: std::false_type
	{
	};

	template <class R>
	struct is_result_type<R const>: is_result_type<R>
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
