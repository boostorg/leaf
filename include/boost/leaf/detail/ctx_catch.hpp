#ifndef BOOST_LEAF_DETAIL_CTX_CATCH_HPP_INCLUDED
#define BOOST_LEAF_DETAIL_CTX_CATCH_HPP_INCLUDED

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

#ifdef BOOST_LEAF_NO_EXCEPTIONS
#	error This header requires exception handling
#endif

#include <boost/leaf/capture.hpp>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <class Tup>
		BOOST_LEAF_CONSTEXPR inline
		std::exception const *
		handler_argument_traits<void>::
		check( Tup const &, error_info const & ei ) noexcept
		{
			return ei.exception();
		}
	}

	template <class... E>
	template <class TryBlock, class... H>
	BOOST_LEAF_CONSTEXPR inline
	typename std::decay<decltype(std::declval<TryBlock>()().value())>::type
	context<E...>::
	try_handle_all( TryBlock && try_block, H && ... h )
	{
		using namespace leaf_detail;
		static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The return type of the try_block passed to a try_handle_all function must be registered with leaf::is_result_type");
		auto active_context = activate_context(*this);
		if(	auto r = this->try_catch_(
				[&]
				{
					return std::forward<TryBlock>(try_block)();
				},
				std::forward<H>(h)...) )
			return r.value();
		else
		{
			error_id id = r.error();
			if( this->is_active() )
				this->deactivate();
			using R = typename std::decay<decltype(std::declval<TryBlock>()().value())>::type;
			return this->template handle_error<R>(std::move(id), std::forward<H>(h)...);
		}
	}

	template <class... E>
	template <class TryBlock, class... H>
	BOOST_LEAF_NODISCARD BOOST_LEAF_CONSTEXPR inline
	typename std::decay<decltype(std::declval<TryBlock>()())>::type
	context<E...>::
	try_handle_some( TryBlock && try_block, H && ... h )
	{
		using namespace leaf_detail;
		static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The return type of the try_block passed to a try_handle_some function must be registered with leaf::is_result_type");
		auto active_context = activate_context(*this);
		if(	auto r = this->try_catch_(
				[&]
				{
					return std::forward<TryBlock>(try_block)();
				},
				std::forward<H>(h)...) )
			return r;
		else
		{
			error_id id = r.error();
			if( this->is_active() )
				this->deactivate();
			using R = typename std::decay<decltype(std::declval<TryBlock>()())>::type;
			auto rr = this->template handle_error<R>(std::move(id), std::forward<H>(h)..., [&r]()->R { return std::move(r); });
			if( !rr )
				this->propagate();
			return rr;
		}
	}

	template <class... E>
	template <class TryBlock, class... H>
	inline
	decltype(std::declval<TryBlock>()())
	context<E...>::
	try_catch_( TryBlock && try_block, H && ... h )
	{
		using namespace leaf_detail;
		BOOST_LEAF_ASSERT(is_active());
		using R = decltype(std::declval<TryBlock>()());
		try
		{
			return std::forward<TryBlock>(try_block)();
		}
		catch( capturing_exception const & cap )
		{
			try
			{
				cap.unload_and_rethrow_original_exception();
			}
			catch( std::exception & ex )
			{
				deactivate();
				return handle_error_<R>(this->tup(), error_info(&ex), std::forward<H>(h)...,
					[]() -> R { throw; } );
			}
			catch(...)
			{
				deactivate();
				return handle_error_<R>(this->tup(), error_info(nullptr), std::forward<H>(h)...,
					[]() -> R { throw; } );
			}
		}
		catch( std::exception & ex )
		{
			deactivate();
			return handle_error_<R>(this->tup(), error_info(&ex), std::forward<H>(h)...,
				[]() -> R { throw; } );
		}
		catch(...)
		{
			deactivate();
			return handle_error_<R>(this->tup(), error_info(nullptr), std::forward<H>(h)...,
				[]() -> R { throw; } );
		}
	}

	////////////////////////////////////////

	template <class TryBlock, class... H>
	BOOST_LEAF_CONSTEXPR inline
	decltype(std::declval<TryBlock>()())
	try_catch( TryBlock && try_block, H && ... h )
	{
		using namespace leaf_detail;
		context_type_from_handlers<H...> ctx;
		auto active_context = activate_context(ctx);
		return ctx.try_catch_(
			[&]
			{
				return std::forward<TryBlock>(try_block)();
			},
			std::forward<H>(h)...);
	}

} }

// Boost Exception Integration

namespace boost { class exception; }
namespace boost { template <class Tag,class T> class error_info; }
namespace boost { namespace exception_detail { template <class ErrorInfo> struct get_info; } }

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <class T>
		struct match_enum_type;

		template <class Tag, class T>
		struct match_enum_type<boost::error_info<Tag, T>>
		{
			using type = T;
		};

		template <class Ex>
		BOOST_LEAF_CONSTEXPR inline Ex * get_exception( error_info const & ei )
		{
			return dynamic_cast<Ex *>(ei.exception());
		}

		template <class, class T>
		struct dependent_type { using type = T; };

		template <class Dep, class T>
		using dependent_type_t = typename dependent_type<Dep, T>::type;

		template <class Tag, class T>
		struct handler_argument_traits<boost::error_info<Tag, T>>
		{
			using error_type = void;
			constexpr static bool always_available = false;

			template <class Tup>
			BOOST_LEAF_CONSTEXPR static T * check( Tup & tup, error_info const & ei ) noexcept
			{
				using boost_exception = dependent_type_t<T, boost::exception>;
				if( auto * be = get_exception<boost_exception>(ei) )
					return exception_detail::get_info<boost::error_info<Tag, T>>::get(*be);
				else
					return 0;
			}

			template <class Tup>
			BOOST_LEAF_CONSTEXPR static boost::error_info<Tag, T> get( Tup const & tup, error_info const & ei ) noexcept
			{
				return boost::error_info<Tag, T>(*check(tup, ei));
			}
		};

		template <class Tag, class T> struct handler_argument_traits<boost::error_info<Tag, T> const &>: handler_argument_traits_require_by_value<boost::error_info<Tag, T>> { };
		template <class Tag, class T> struct handler_argument_traits<boost::error_info<Tag, T> const *>: handler_argument_traits_require_by_value<boost::error_info<Tag, T>> { };
		template <class Tag, class T> struct handler_argument_traits<boost::error_info<Tag, T> &>: handler_argument_traits_require_by_value<boost::error_info<Tag, T>> { };
		template <class Tag, class T> struct handler_argument_traits<boost::error_info<Tag, T> *>: handler_argument_traits_require_by_value<boost::error_info<Tag, T>> { };
	}

} }

#endif
