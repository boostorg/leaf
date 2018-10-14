//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UUID_E95BDA9ACEAA11E894618A98A659E189
#define UUID_E95BDA9ACEAA11E894618A98A659E189

#include <boost/leaf/result.hpp>
#include <boost/leaf/detail/captured_info.hpp>

namespace
boost
	{
	namespace
	leaf
		{
		template <class T>
		class
		captured_result
			{
			friend class result<T>;
			result<T> r_;
			std::shared_ptr<leaf_detail::captured_info> cap_;
			explicit
			captured_result( result<T> && r ) noexcept:
				r_(std::move(r))
				{
				assert(r_);
				}
			explicit
			captured_result( std::shared_ptr<leaf_detail::captured_info> && cap ) noexcept:
				r_(leaf_detail::error_tag()),
				cap_(std::move(cap))
				{
				}
			public:
#ifdef _MSC_VER
			captured_result() noexcept
				{
				}
#else
			captured_result( captured_result const & ) = delete;
			captured_result & operator=( captured_result const & ) = delete;
			captured_result( captured_result && ) noexcept = default;
#endif
			result<T>
			get()
				{
				if( cap_ )
					{
					cap_->unpack();
					cap_.reset();
					}
				return r_;
				}
			};
		template <class T>
		template <class... ExpectErrorInfo>
		captured_result<T>
		result<T>::
		capture( expect<ExpectErrorInfo...> & exp )
			{
			using namespace leaf_detail;
			if( *this )
				return captured_result<T>(std::move(*this));
			else
				return captured_result<T>(std::shared_ptr<captured_info>(new captured_info_impl<ExpectErrorInfo...>(extract(exp))));
			}
		template <class... ExpectErrorInfo>
		captured_result<void>
		result<void>::
		capture( expect<ExpectErrorInfo...> & exp )
			{
			using namespace leaf_detail;
			if( *this )
				return captured_result<void>(std::move(*this));
			else
				return captured_result<void>(std::shared_ptr<captured_info>(new captured_info_impl<ExpectErrorInfo...>(extract(exp))));
			}
		}
	}

#endif
