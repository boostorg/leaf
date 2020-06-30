#ifndef BOOST_LEAF_HANDLE_EXCEPTION_HPP_INCLUDED
#define BOOST_LEAF_HANDLE_EXCEPTION_HPP_INCLUDED

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
#ifdef BOOST_LEAF_NO_EXCEPTIONS
#	error This header requires exception handling
#endif

#include <boost/leaf/handle_error.hpp>
#include <boost/leaf/capture.hpp>
#include <boost/leaf/detail/demangle.hpp>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <class... E>
		template <class TryBlock, class... H>
		BOOST_LEAF_CONSTEXPR inline
		typename std::decay<decltype(std::declval<TryBlock>()().value())>::type
		catch_context<E...>::
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
		catch_context<E...>::
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

		template <class Ex>
		BOOST_LEAF_CONSTEXPR inline bool check_exception_pack( std::exception const & ex, Ex const * ) noexcept
		{
			return dynamic_cast<Ex const *>(&ex)!=0;
		}

		template <class Ex, class... ExRest>
		BOOST_LEAF_CONSTEXPR inline bool check_exception_pack( std::exception const & ex, Ex const *, ExRest const * ... ex_rest ) noexcept
		{
			return dynamic_cast<Ex const *>(&ex)!=0 || check_exception_pack(ex, ex_rest...);
		}

		BOOST_LEAF_CONSTEXPR inline bool check_exception_pack( std::exception const & )
		{
			return true;
		}
	}

	////////////////////////////////////////

	template <class... Ex>
	class catch_
	{
		std::exception const & ex_;

	public:

		BOOST_LEAF_CONSTEXPR explicit catch_( std::exception const & ex ) noexcept:
			ex_(ex)
		{
		}

		BOOST_LEAF_CONSTEXPR explicit operator bool() const noexcept
		{
			return leaf_detail::check_exception_pack(ex_, static_cast<Ex const *>(0)...);
		}

		BOOST_LEAF_CONSTEXPR std::exception const & matched() const noexcept
		{
			return ex_;
		}
	};

	template <class Ex>
	class catch_<Ex>
	{
		std::exception const & ex_;

	public:

		BOOST_LEAF_CONSTEXPR explicit catch_( std::exception const & ex ) noexcept:
			ex_(ex)
		{
		}

		BOOST_LEAF_CONSTEXPR explicit operator bool() const noexcept
		{
			return dynamic_cast<Ex const *>(&ex_) != 0;
		}

		BOOST_LEAF_CONSTEXPR Ex const & matched() const noexcept
		{
			Ex const * ex = dynamic_cast<Ex const *>(&ex_);
			BOOST_LEAF_ASSERT(ex != 0);
			return *ex;
		}
	};

	////////////////////////////////////////

	namespace leaf_detail
	{
		inline void exception_info_::print( std::ostream & os ) const
		{
			if( ex_ )
			{
				os <<
					"\nException dynamic type: " << demangle(typeid(*ex_).name()) <<
					"\nstd::exception::what(): " << ex_->what();
			}
			else
				os << "\nUnknown exception type (not a std::exception)";
		}

		BOOST_LEAF_CONSTEXPR inline exception_info_::exception_info_( std::exception * ex ) noexcept:
			exception_info_base(ex)
		{
		}

		template <class... E>
		template <class TryBlock, class... H>
		inline
		decltype(std::declval<TryBlock>()())
		context_base<E...>::
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
					return handle_error_<R>(this->tup(), error_info(exception_info_(&ex)), std::forward<H>(h)...,
						[]() -> R { throw; } );
				}
				catch(...)
				{
					deactivate();
					return handle_error_<R>(this->tup(), error_info(exception_info_(0)), std::forward<H>(h)...,
						[]() -> R { throw; } );
				}
			}
			catch( std::exception & ex )
			{
				deactivate();
				return handle_error_<R>(this->tup(), error_info(exception_info_(&ex)), std::forward<H>(h)...,
					[]() -> R { throw; } );
			}
			catch(...)
			{
				deactivate();
				return handle_error_<R>(this->tup(), error_info(exception_info_(0)), std::forward<H>(h)...,
					[]() -> R { throw; } );
			}
		}
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		inline error_id unpack_error_id( std::exception const * ex ) noexcept
		{
			if( std::system_error const * se = dynamic_cast<std::system_error const *>(ex) )
				return error_id(se->code());
			else if( std::error_code const * ec = dynamic_cast<std::error_code const *>(ex) )
				return error_id(*ec);
			else if( error_id const * err_id = dynamic_cast<error_id const *>(ex) )
				return *err_id;
			else
				return current_error();
		}

		BOOST_LEAF_CONSTEXPR inline exception_info_base::exception_info_base( std::exception * ex ) noexcept:
			ex_(ex)
		{
			BOOST_LEAF_ASSERT(!dynamic_cast<capturing_exception const *>(ex_));
		}

		inline exception_info_base::~exception_info_base() noexcept
		{
		}
	}

	inline error_info::error_info( leaf_detail::exception_info_ const & xi ) noexcept:
		xi_(&xi),
		err_id_(leaf_detail::unpack_error_id(xi_->ex_))
	{
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

#endif
