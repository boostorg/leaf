//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_2CD8E6B8CA8D11E8BD3B80D66CE5B91B
#define UUID_2CD8E6B8CA8D11E8BD3B80D66CE5B91B

#include <boost/leaf/expect.hpp>
#include <boost/leaf/put.hpp>

#define LEAF_ERROR ::boost::leaf::error(ei_SOURCE_LOCATION)
#define LEAF_CHECK(v,r) auto _r_##v = r; if( !_r_##v ) return _r_##v.error(); auto & v = *_r_##v
#define LEAF_CHECK_(r) {auto _r_##v = r; if( !_r_##v ) return _r_##v.error();}

namespace
boost
	{
	namespace
	leaf
		{
		namespace
		leaf_detail
			{
			struct error_tag { };
			}
		template <class T>
		class
		result:
			leaf_detail::optional<T>
			{
			typedef leaf_detail::optional<T> base;
			public:
			using base::has_value;
			result():
				base(T())
				{
				}
			result( T const & v ):
				base(v)
				{
				}
			result( T && v ) noexcept:
				base(std::move(v))
				{
				}
			result( leaf_detail::error_tag ) noexcept
				{
				}
			explicit
			operator bool() const noexcept
				{
				return has_value();
				}
			T const &
			value() const noexcept
				{
				if( has_value() )
					return base::value();
				else
					throw bad_result();
				}
			T &
			value() noexcept
				{
				if( has_value() )
					return base::value();
				else
					throw bad_result();
				}
			T const &
			operator*() const noexcept
				{
				return value();
				}
			T &
			operator*() noexcept
				{
				return value();
				}
			template <class... ErrorInfo>
			leaf_detail::error_tag
			error( ErrorInfo && ... a ) noexcept
				{
				assert(has_current_error());
				put(std::forward<ErrorInfo>(a)...);
				return leaf_detail::error_tag();
				}				
			};
		template <>
		class
		result<void>
			{
			bool success_;
			public:
			result( result && ) = default;
			result( result const & ) = default;
			result & operator=( result const & ) = default;
			result():
				success_(true)
				{
				}
			result( leaf_detail::error_tag ) noexcept:
				success_(false)
				{
				}
			explicit
			operator bool() const noexcept
				{
				return success_;
				}
			template <class... ErrorInfo>
			leaf_detail::error_tag
			error( ErrorInfo && ... a ) noexcept
				{
				assert(leaf_detail::current_error_flag());
				put(std::forward<ErrorInfo>(a)...);
				return leaf_detail::error_tag();
				}				
			};
		template <class ErrorInfo,class... ExpectErrorInfo,class T>
		decltype(std::declval<ErrorInfo>().value) const *
		peek( expect<ExpectErrorInfo...> const &, result<T> const & r )
			{
			assert(!r);
			auto & x = leaf_detail::tl_slot<ErrorInfo>::tl_instance();
			if( x.has_value() )
				return &x.value().value;
			else
				return 0;
			}
		template <class... Match,class... ExpectErrorInfo,class T>
		void
		handle_error( expect<ExpectErrorInfo...> & e, result<T> const & r, Match && ... m )
			{
			assert(!r);
			int count = sizeof...(Match);
			bool & still_has_error = leaf_detail::current_error_flag();
			still_has_error = true;
			{ using _ = int[ ]; (void) _ { 42, m.unwrap(still_has_error,count)... }; }
			}
		template <class... ErrorInfo>
		leaf_detail::error_tag
		error( ErrorInfo && ... a ) noexcept
			{
			leaf_detail::current_error_flag() = true;
			leaf_detail::tl_slot_base::bump_current_seq_id();
			put(std::forward<ErrorInfo>(a)...);
			return leaf_detail::error_tag();
			}
		}
	}

#endif
