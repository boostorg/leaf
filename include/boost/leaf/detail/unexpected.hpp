#ifndef BOOST_LEAF_DETAIL_UNEXPECTED_HPP_INCLUDED
#define BOOST_LEAF_DETAIL_UNEXPECTED_HPP_INCLUDED

// Copyright (c) 2018-2020 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if defined(__clang__)
#	pragma clang system_header
#elif (__GNUC__*100+__GNUC_MINOR__>301) && !defined(BOOST_LEAF_ENABLE_WARNINGS)
#	pragma GCC system_header
#elif defined(_MSC_VER) && !defined(BOOST_LEAF_ENABLE_WARNINGS)
#	pragma warning(push,1)
#endif

#include <boost/leaf/config.hpp>
#include <boost/leaf/detail/print.hpp>
#include <sstream>
#include <set>

#ifdef BOOST_LEAF_NO_THREADS
#	define BOOST_LEAF_THREAD_LOCAL
	namespace boost { namespace leaf {
		namespace leaf_detail
		{
			using atomic_unsigned_int = unsigned int;
		}
	} }
#else
#	include <atomic>
#	include <thread>
#	define BOOST_LEAF_THREAD_LOCAL thread_local
	namespace boost { namespace leaf {
		namespace leaf_detail
		{
			using atomic_unsigned_int = std::atomic<unsigned int>;
		}
	} }
#endif

namespace boost { namespace leaf {

#if BOOST_LEAF_DIAGNOSTICS

	namespace leaf_detail
	{
		class e_unexpected_count
		{
		public:

			char const * (*first_type)();
			int count;

			BOOST_LEAF_CONSTEXPR explicit e_unexpected_count(char const * (*first_type)()) noexcept:
				first_type(first_type),
				count(1)
			{
			}

			void print(std::ostream & os) const
			{
				BOOST_LEAF_ASSERT(first_type != 0);
				BOOST_LEAF_ASSERT(count>0);
				os << "Detected ";
				if( count==1 )
					os << "1 attempt to communicate an unexpected error object";
				else
					os << count << " attempts to communicate unexpected error objects, the first one";
				os << " of type " << first_type() << std::endl;
			}
		};

		template <>
		struct diagnostic<e_unexpected_count, false, false>
		{
			static constexpr bool is_invisible = true;
			BOOST_LEAF_CONSTEXPR static void print(std::ostream &, e_unexpected_count const &) noexcept { }
		};

		class e_unexpected_info
		{
			std::string s_;
			std::set<char const *(*)()> already_;

		public:

			e_unexpected_info() noexcept
			{
			}

			template <class E>
			void add(E const & e)
			{
				if( !diagnostic<E>::is_invisible && already_.insert(&type<E>).second  )
				{
					std::stringstream s;
					diagnostic<E>::print(s,e);
					s << std::endl;
					s_ += s.str();
				}
			}

			void print(std::ostream & os) const
			{
				os << "Unexpected error objects:\n" << s_;
			}
		};

		template <>
		struct diagnostic<e_unexpected_info, false, false>
		{
			static constexpr bool is_invisible = true;
			BOOST_LEAF_CONSTEXPR static void print(std::ostream &, e_unexpected_info const &) noexcept { }
		};

		template <class=void>
		struct tl_unexpected_enabled
		{
			static BOOST_LEAF_THREAD_LOCAL int counter;
		};

		template <class T>
		BOOST_LEAF_THREAD_LOCAL int tl_unexpected_enabled<T>::counter;
	}

#else

	namespace leaf_detail
	{
		using e_unexpected_count = void;
		using e_unexpected_info = void;
	}

#endif

} }

#endif
