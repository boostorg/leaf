//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_AFBBD676B2FF11E8984C7976AE35F1A2
#define UUID_AFBBD676B2FF11E8984C7976AE35F1A2

#include <boost/leaf/detail/tl_slot.hpp>
#include <boost/leaf/has_current_error.hpp>

namespace
boost
	{
	namespace
	leaf
	 	{
		struct mismatch_error: virtual std::exception { };
		template <class... Match>
		void
		unwrap( Match && ... m )
			{
			using namespace leaf_detail;
			bool matched = false;
			{ using _ = int[ ]; (void) _ { 42, m.unwrap_(matched)... }; }
			if( !matched )
				throw mismatch_error();
			}
		class
		available
			{
			available( available const & ) = delete;
			available & operator=( available const & ) = delete;
			template <class F,class... T>
			class
			match_
				{
				friend class available;
				match_( match_ && ) = default;
				match_( match_ const & ) = delete;
				match_ & operator=( match_ const & ) = delete;
				F f_;
				explicit
				match_( F && f ) noexcept:
					f_(std::move(f))
					{
					}
				public:
				int
				unwrap_( bool & matched ) const noexcept
					{
					using namespace leaf_detail;
					if( !matched )
						{
						bool const available[ ] = { tl_slot<T>::tl_instance().has_value()... };
						for( auto i : available )
							if( !i )
								return 42;
						(void) f_(tl_slot<T>::tl_instance().value().value...);
						matched=true;
						}
					return 42;
					}
				};
			bool reset_all_;
			protected:
			public:
			available() noexcept:
				reset_all_(true)
				{
				}
			~available() noexcept
				{
				if( reset_all_ )
					leaf_detail::tl_slot_base::reset_all();
				}
			void
			set_to_propagate() noexcept
				{
				reset_all_ = false;
				}
			[[noreturn]]
			void
			rethrow_with_current_info()
				{
				set_to_propagate();
				throw;
				}
			template <class... T,class F>
			match_<F,T...>
			match( F && f ) noexcept
				{
				return match_<F,T...>(std::move(f));
				}
			};
		namespace
		leaf_detail
			{
#ifdef _MSC_VER
			template <int I,class Tuple>
			struct
			msvc_workaround_open
				{
				static
				void
				open() noexcept
					{
					typedef typename std::tuple_element<I-1,Tuple>::type ith_type;
					tl_slot<ith_type>::tl_instance().open();
					msvc_workaround_open<I-1,Tuple>::open();
					}
				};
			template <class Tuple>
			struct
			msvc_workaround_open<0,Tuple>
				{
				static void open() noexcept { }
				};
			template <int I,class Tuple>
			struct
			msvc_workaround_close
				{
				static
				void
				close() noexcept
					{
					typedef typename std::tuple_element<I-1,Tuple>::type ith_type;
					tl_slot<ith_type>::tl_instance().close();
					msvc_workaround_close<I-1,Tuple>::close();
					}
				};
			template <class Tuple>
			struct
			msvc_workaround_close<0,Tuple>
				{
				static void close() noexcept { }
				};
			template <class... T>
			void
			open_slots() noexcept
				{
				msvc_workaround_open<sizeof...(T),std::tuple<T...>>::open();
				}
			template <class... T>
			void
			close_slots() noexcept
				{
				msvc_workaround_close<sizeof...(T),std::tuple<T...>>::close();
				}
#else
			template <class... T>
			void
			open_slots() noexcept
				{
				{ using _ = int[ ]; (void) _ { 42, tl_slot<T>::tl_instance().open()... }; }
				}
			template <class... T>
			void
			close_slots() noexcept
				{
				{ using _ = int[ ]; (void) _ { 42, tl_slot<T>::tl_instance().close()... }; }
				}
#endif
			}
		template <class... A>
		class
		expect:
			public available
			{
			expect( expect const & ) = delete;
			expect & operator=( expect const & ) = delete;
			public:
			expect() noexcept
				{
				leaf_detail::tl_slot_base::reset_all();
				leaf_detail::open_slots<A...>();
				}
			~expect() noexcept
				{
				leaf_detail::close_slots<A...>();
				if( has_current_error() )
					set_to_propagate();
				}
			};
		}
	}

#endif
