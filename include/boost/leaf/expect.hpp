//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_AFBBD676B2FF11E8984C7976AE35F1A2
#define UUID_AFBBD676B2FF11E8984C7976AE35F1A2

#include <boost/leaf/detail/tl_slot.hpp>
#include <exception>

namespace
boost
	{
	namespace
	leaf
	 	{
		struct mismatch_error: virtual std::exception { };

		template <class... ExpectErrorInfo>
		class expect;

		template <class T>
		class result;

		class bad_result: public std::exception { };

		namespace
		leaf_detail
			{
			template <class F,class R,class... ExpectErrorInfo> class capture_wrapper;
			template <class F,class... MatchErrorInfo>
			class
			match_impl
				{
				match_impl( match_impl const & ) = delete;
				match_impl & operator=( match_impl const & ) = delete;
				F f_;
				public:
				match_impl( match_impl && ) = default;
				explicit
				match_impl( F && f ) noexcept:
					f_(std::move(f))
					{
					}
				template <class... ExpectErrorInfo>
				int
				unwrap( bool & still_has_error, int & count ) const noexcept
					{
					using namespace leaf_detail;
					bool const last = (--count==0);
					assert(count>=0);
					if( still_has_error )
						{
						bool const available[ ] = { tl_slot<MatchErrorInfo>::tl_instance().has_value()... };
						for( auto i : available )
							if( !i )
								if( last )
									throw mismatch_error();
								else
									return 42;
						(void) f_(tl_slot<MatchErrorInfo>::tl_instance().value().value...);
						still_has_error = false;
						}
					return 42;
					}
				};
			template <class T>
			int
			slot_open_reset()
				{
				auto & info = tl_slot<T>::tl_instance();
				(void) info.open();
				info.reset();
				return 42;
				}
			template <class T>
			int
			slot_close()
				{
				tl_slot<T>::tl_instance().close();
				return 42;
				}
#ifdef _MSC_VER
			template <int I,class Tuple>
			struct
			msvc_workaround_open_reset
				{
				static
				void
				open_reset() noexcept
					{
					typedef typename std::tuple_element<I-1,Tuple>::type ith_type;
					(void) open_reset<ith_type>();
					msvc_workaround_open<I-1,Tuple>::open_reset();
					}
				};
			template <class Tuple>
			struct
			msvc_workaround_open_reset<0,Tuple>
				{
				static void open_reset() noexcept { }
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
					(void) slot_close<ith_type>();
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
			slots_open_reset() noexcept
				{
				msvc_workaround_open_reset<sizeof...(T),std::tuple<T...>>::open_reset();
				}
			template <class... T>
			void
			slots_close() noexcept
				{
				msvc_workaround_close<sizeof...(T),std::tuple<T...>>::check_close();
				}
#else
			template <class... T>
			void
			slots_open_reset() noexcept
				{
				{ using _ = int[ ]; (void) _ { 42, slot_open_reset<T>()... }; }
				}
			template <class... T>
			void
			slots_close() noexcept
				{
				{ using _ = int[ ]; (void) _ { 42, slot_close<T>()... }; }
				}
#endif
			}
		template <class... ExpectErrorInfo>
		class
		expect
			{
			expect( expect const & ) = delete;
			expect & operator=( expect const & ) = delete;

			template <class F,class... MatchErrorInfo>
			friend class leaf_detail::match_impl;

			public:
			expect() noexcept
				{
				using namespace leaf_detail;
				tl_slot_base::bump_current_seq_id();
				current_error_flag() = false;
				slots_open_reset<ei_source_location<in_file>,ei_source_location<at_line>,ei_source_location<in_function>,ExpectErrorInfo...>();
				}
			~expect() noexcept
				{
				using namespace leaf_detail;
				if( !has_current_error() )
					tl_slot_base::bump_current_seq_id();
				slots_close<ei_source_location<in_file>,ei_source_location<at_line>,ei_source_location<in_function>,ExpectErrorInfo...>();
				}
			void print_diagnostic_information( std::ostream & ) const;
			void print_current_exception_diagnostic_information( std::ostream & ) const;
			};
		template <class ErrorInfo,class... ExpectErrorInfo>
		decltype(std::declval<ErrorInfo>().value) const *
		peek( expect<ExpectErrorInfo...> const &, std::exception const & )
			{
			auto & x = leaf_detail::tl_slot<ErrorInfo>::tl_instance();
			if( x.has_value() )
				return &x.value().value;
			else
				return 0;
			}
		template <class... Match,class... ExpectErrorInfo>
		void
		handle_error( expect<ExpectErrorInfo...> & e, std::exception const &, Match && ... m )
			{
			int count = sizeof...(Match);
			bool & still_has_error = leaf_detail::current_error_flag();
			still_has_error = true;
			{ using _ = int[ ]; (void) _ { 42, m.unwrap(still_has_error,count)... }; }
			}
		template <class... MatchErrorInfo,class F>
		leaf_detail::match_impl<F,MatchErrorInfo...>
		match( F && f ) noexcept
			{
			return leaf_detail::match_impl<F,MatchErrorInfo...>(std::move(f));
			}
		}
	}

#endif
