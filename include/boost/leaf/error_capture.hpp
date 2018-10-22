//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_C86E4C4ED0F011E8BB777EB8A659E189
#define UUID_C86E4C4ED0F011E8BB777EB8A659E189

#include <boost/leaf/error.hpp>
#include <boost/leaf/throw_exception.hpp>
#include <boost/leaf/detail/print.hpp>
#include <tuple>

namespace
boost
	{
	namespace
	leaf
		{
		class error_capture;

		template <class P>
		decltype(P::value) const * peek( error_capture const & );
		////////////////////////////////////////
		namespace
		leaf_detail
			{
			template <int I,class Tuple>
			struct
			tuple_dynamic_bind
				{
				static
				void const *
				bind( Tuple const & tup, char const * (*type_id)() ) noexcept
					{
					assert(type_id!=0);
					typedef typename std::tuple_element<I-1,Tuple>::type ith_type;
					if( &type<typename ith_type::value_type> == type_id )
						return &std::get<I-1>(tup);
					return tuple_dynamic_bind<I-1,Tuple>::bind(tup,type_id);
					}
				};
			template <class Tuple>
			struct
			tuple_dynamic_bind<0,Tuple>
				{
				static void const * bind( Tuple const &, char const * (*)() ) noexcept { return 0; }
				};
			////////////////////////////////////////
			template <class... List>
			struct all_available;
			template <class Car,class... Cdr>
			struct
			all_available<Car,Cdr...>
				{
				static
				bool
				check( error_capture const & cap ) noexcept
					{
					return peek<Car>(cap) && all_available<Cdr...>::check(cap);
					}
				};
			template <>
			struct
			all_available<>
				{
				static bool check( error_capture const & ) noexcept { return true; }
				};
			////////////////////////////////////////
			template <int I,class Tuple>
			struct
			tuple_print
				{
				static
				void
				print( std::ostream & os, Tuple const & tup )
					{
					typedef typename std::tuple_element<I-1,Tuple>::type ith_type;
					tuple_print<I-1,Tuple>::print(os,tup);
					auto & opt = std::get<I-1>(tup);
					if( opt.has_value() && diagnostic<typename ith_type::value_type>::print(os,opt.value()) )
						os << std::endl;
					}
				};
			template <class Tuple>
			struct
			tuple_print<0,Tuple>
				{
				static void print( std::ostream &, Tuple const & ) { }
				};
			////////////////////////////////////////
			template <int I,class Tuple>
			struct
			tuple_propagate
				{
				static
				void
				propagate( error const & e, Tuple & tup ) noexcept
					{
					tuple_propagate<I-1,Tuple>::propagate(e,tup);
					auto & opt = std::get<I-1>(tup);
					if( opt.has_value() )
						e.propagate(opt.extract_value());
					}
				};
			template <class Tuple>
			struct
			tuple_propagate<0,Tuple>
				{
				static void propagate( error const &, Tuple & ) noexcept { }
				};
			}
		////////////////////////////////////////
		class
		error_capture
			{
			template <class P>
			friend decltype(P::value) const * leaf::peek( error_capture const & );

			class
			dynamic_store
				{
				mutable std::atomic<int> refcount_;
				virtual void const * bind_( char const * (*)() ) const noexcept = 0;
				protected:
				dynamic_store() noexcept:
					refcount_(0)
					{
					}
				public:
				virtual
				~dynamic_store() noexcept
					{
					}
				void
				addref() const noexcept
					{
					++refcount_;
					}
				void
				release() const noexcept
					{
					if( !--refcount_ )
						delete this;
					}
				template <class T>
				leaf_detail::optional<T> const *
				bind() const noexcept
					{
					if( void const * p = bind_(&type<T>) )
						return static_cast<leaf_detail::optional<T> const *>(p);
					else
						return 0;
					}
				virtual void diagnostic_print( std::ostream & ) const = 0;
				virtual void propagate( error const & ) noexcept = 0;
				};
			template <class... T>
			class
			dynamic_store_impl:
				public dynamic_store
				{
				std::tuple<leaf_detail::optional<T>...> s_;
				public:
				explicit
				dynamic_store_impl( std::tuple<leaf_detail::optional<T>...> && s ) noexcept:
					s_(std::move(s))
					{
					}
				void const *
				bind_( char const * (*type_id)() ) const noexcept
					{
					using namespace leaf_detail;
					assert(type_id!=0);
					return tuple_dynamic_bind<sizeof...(T),std::tuple<optional<T>...>>::bind(s_,type_id);
					}
				void
				diagnostic_print( std::ostream & os ) const
					{
					leaf_detail::tuple_print<sizeof...(T),decltype(s_)>::print(os,s_);
					}
				void
				propagate( error const & e ) noexcept
					{
					leaf_detail::tuple_propagate<sizeof...(T),decltype(s_)>::propagate(e,s_);
					}
				};
			void
			free() noexcept
				{
				if( s_ )
					{
					s_->release();
					s_=0;
					}
				}
			template <class F,class... MatchTypes>
			int
			unwrap( leaf_detail::match_impl<F,MatchTypes...> const & m, bool & matched ) const noexcept
				{
				if( !matched && (matched=leaf_detail::all_available<MatchTypes...>::check(*this)) )
					(void) m.f( *peek<MatchTypes>(*this)... );
				return 42;
				}
			error e_;
			dynamic_store * s_;
			public:			
			error_capture() noexcept:
				s_(0)
				{
				}
			template <class... E>
			error_capture( error const & e, std::tuple<leaf_detail::optional<E>...> && s ) noexcept:
				e_(e),
				s_(new dynamic_store_impl<E...>(std::move(s)))
				{
				s_->addref();
				}
			~error_capture() noexcept
				{
				free();
				}
			error_capture( error_capture const & x ) noexcept:
				e_(x.e_),
				s_(x.s_)
				{
				if( s_ )
					s_->addref();
				}
			error_capture( error_capture && x ) noexcept:
				e_(std::move(x.e_)),
				s_(x.s_)
				{
				x.s_ = 0;
				}
			error_capture &
			operator=( error_capture const & x ) noexcept
				{
				e_ = x.e_;
				s_ = x.s_;
				s_->addref();
				return *this;
				}
			error_capture &
			operator=( error_capture && x ) noexcept
				{
				e_ = x.e_;
				s_ = x.s_;
				x.s_ = 0;
				return *this;
				}
			explicit
			operator bool() const noexcept
				{
				return s_!=0;
				}
			error
			propagate() noexcept
				{
				if( s_ )
					{
					s_->propagate(e_);
					free();
					}
				return e_;
				}
			template <class... M>
			friend
			void
			handle_error( error_capture & e, M && ... m )
				{
				if( e )
					{
					bool matched = false;
					{ using _ = int[ ]; (void) _ { 42, e.unwrap(m,matched)... }; }
					if( !matched )
						throw_exception(mismatch_error());
					e.free();
					}
				}
			friend
			void
			diagnostic_print( std::ostream & os, error_capture const & e )
				{
				if( e )
					e.s_->diagnostic_print(os);
				}
			};
		////////////////////////////////////////
		template <class P>
		decltype(P::value) const *
		peek( error_capture const & e )
			{
			if( e )
				if( auto * opt = e.s_->bind<P>() )
					if( opt->has_value() )
						return &opt->value().value;
			return 0;
			}
		}
	}

#endif
