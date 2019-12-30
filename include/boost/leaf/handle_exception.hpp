#ifndef LEAF_87F274C4D4BA11E89928D55AC82C3C47
#define LEAF_87F274C4D4BA11E89928D55AC82C3C47

// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/config.hpp>
#ifdef LEAF_NO_EXCEPTIONS
#	error This header requires exception handling
#endif

#include <boost/leaf/capture.hpp>
#include <boost/leaf/handle_error.hpp>
#include <boost/leaf/detail/demangle.hpp>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <class Ex>
		LEAF_CONSTEXPR inline bool check_exception_pack( std::exception const * ex, Ex const * ) noexcept
		{
			return dynamic_cast<Ex const *>(ex)!=0;
		}

		template <class Ex, class... ExRest>
		LEAF_CONSTEXPR inline bool check_exception_pack( std::exception const * ex, Ex const *, ExRest const * ... ex_rest ) noexcept
		{
			return dynamic_cast<Ex const *>(ex)!=0 || check_exception_pack(ex, ex_rest...);
		}

		LEAF_CONSTEXPR inline bool check_exception_pack( std::exception const * )
		{
			return true;
		}
	}

	template <class... Ex>
	class catch_
	{
		std::exception const * const value_;

	public:

		LEAF_CONSTEXPR explicit catch_( std::exception const * value ) noexcept:
			value_(value)
		{
		}

		LEAF_CONSTEXPR bool operator()() const noexcept
		{
			return value_ && leaf_detail::check_exception_pack(value_,static_cast<Ex const *>(0)...);
		}

		LEAF_CONSTEXPR std::exception const & value() const noexcept
		{
			assert(value_!=0);
			return *value_;
		}
	};

	template <class Ex>
	class catch_<Ex>
	{
		Ex const * const value_;

	public:

		LEAF_CONSTEXPR explicit catch_( std::exception const * value ) noexcept:
			value_(dynamic_cast<Ex const *>(value))
		{
		}

		LEAF_CONSTEXPR bool operator()() const noexcept
		{
			return this->value_!=0;
		}

		LEAF_CONSTEXPR Ex const & value() const noexcept
		{
			assert(this->value_!=0);
			return *this->value_;
		}
	};

	namespace leaf_detail
	{
		template <class... Exceptions> struct translate_type_impl<catch_<Exceptions...>> { using type = void; };
		template <class... Exceptions> struct translate_type_impl<catch_<Exceptions...> const>;
		template <class... Exceptions> struct translate_type_impl<catch_<Exceptions...> const *> { static_assert(sizeof(catch_<Exceptions...>)==0, "Handlers should take catch_<> by value, not as catch_<> const *"); };
		template <class... Exceptions> struct translate_type_impl<catch_<Exceptions...> const &> { static_assert(sizeof(catch_<Exceptions...>)==0, "Handlers should take catch_<> by value, not as catch_<> const &"); };

		template <class SlotsTuple, class... Ex>
		struct check_one_argument<SlotsTuple,catch_<Ex...>>
		{
			LEAF_CONSTEXPR static bool check( SlotsTuple const &, error_info const & ei ) noexcept
			{
				if( ei.exception_caught() )
					return catch_<Ex...>(ei.exception())();
				else
					return false;
			}
		};

		template <class... Ex>
		struct get_one_argument<catch_<Ex...>>
		{
			template <class SlotsTuple>
			LEAF_CONSTEXPR static catch_<Ex...> get( SlotsTuple const &, error_info const & ei ) noexcept
			{
				std::exception const * ex = ei.exception();
				assert(ex!=0);
				return catch_<Ex...>(ex);
			}
		};
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class... E>
		template <class R, class... H>
		LEAF_CONSTEXPR inline R context_base<E...>::handle_current_exception_( H && ... h )
		{
			assert(is_active());
			return this->try_catch_(
				[]() -> R { throw; },
				std::forward<H>(h)...);
		}

		template <class... E>
		template <class R, class RemoteH>
		LEAF_CONSTEXPR inline R context_base<E...>::remote_handle_current_exception_( RemoteH && h )
		{
			assert(is_active());
			return this->remote_try_catch_(
				[]() -> R { throw; },
				std::forward<RemoteH>(h));
		}

		template <class... E>
		template <class R, class... H>
		LEAF_CONSTEXPR inline R context_base<E...>::handle_exception_( std::exception_ptr const & ep, H && ... h )
		{
			assert(is_active());
			return this->try_catch_(
				[&]() -> R { std::rethrow_exception(ep); },
				std::forward<H>(h)...);
		}

		template <class... E>
		template <class R, class RemoteH>
		LEAF_CONSTEXPR inline R context_base<E...>::remote_handle_exception_( std::exception_ptr const & ep, RemoteH && h  )
		{
			assert(is_active());
			return this->remote_try_catch_(
				[&]() -> R { std::rethrow_exception(ep); },
				std::forward<RemoteH>(h));
		}

		template <class... E>
		template <class R, class... H>
		LEAF_CONSTEXPR inline R context_base<E...>::handle_current_exception( H && ... h )
		{
			auto active_context = activate_context(*this);
			return handle_current_exception_<R>(std::forward<H>(h)...);
		}

		template <class... E>
		template <class R, class RemoteH>
		LEAF_CONSTEXPR inline R context_base<E...>::remote_handle_current_exception( RemoteH && h )
		{
			auto active_context = activate_context(*this);
			return remote_handle_current_exception_<R>(std::forward<RemoteH>(h));
		}

		template <class... E>
		template <class R, class... H>
		LEAF_CONSTEXPR inline R context_base<E...>::handle_exception( std::exception_ptr const & ep, H && ... h )
		{
			auto active_context = activate_context(*this);
			return handle_exception_<R>(ep, std::forward<H>(h)...);
		}

		template <class... E>
		template <class R, class RemoteH>
		LEAF_CONSTEXPR inline R context_base<E...>::remote_handle_exception( std::exception_ptr const & ep, RemoteH && h  )
		{
			auto active_context = activate_context(*this);
			return remote_handle_exception_<R>(ep, std::forward<RemoteH>(h));
		}

		////////////////////////////////////////

		template <class... E>
		template <class TryBlock, class... H>
		LEAF_CONSTEXPR inline typename std::decay<decltype(std::declval<TryBlock>()().value())>::type catch_context<E...>::try_handle_all( TryBlock && try_block, H && ... h )
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
				return this->handle_all(r, std::forward<H>(h)...);
		}

		template <class... E>
		template <class TryBlock, class RemoteH>
		LEAF_CONSTEXPR inline typename std::decay<decltype(std::declval<TryBlock>()().value())>::type catch_context<E...>::remote_try_handle_all( TryBlock && try_block, RemoteH && h )
		{
			using namespace leaf_detail;
			static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The return type of the try_block passed to a try_handle_all function must be registered with leaf::is_result_type");
			auto active_context = activate_context(*this);
			if(	auto r = this->remote_try_catch_(
					[&]
					{
						return std::forward<TryBlock>(try_block)();
					},
					std::forward<RemoteH>(h)) )
				return r.value();
			else
				return this->remote_handle_all(r, std::forward<RemoteH>(h));
		}

		template <class... E>
		template <class TryBlock, class... H>
		LEAF_CONSTEXPR inline typename std::decay<decltype(std::declval<TryBlock>()())>::type catch_context<E...>::try_handle_some( TryBlock && try_block, H && ... h )
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
				auto rr = this->handle_some(std::move(r), std::forward<H>(h)...);
				if( !rr )
					this->propagate();
				return rr;
			}
		}

		template <class... E>
		template <class TryBlock, class RemoteH>
		LEAF_CONSTEXPR inline typename std::decay<decltype(std::declval<TryBlock>()())>::type catch_context<E...>::remote_try_handle_some( TryBlock && try_block, RemoteH && h )
		{
			using namespace leaf_detail;
			static_assert(is_result_type<decltype(std::declval<TryBlock>()())>::value, "The return type of the try_block passed to a remote_try_handle_some function must be registered with leaf::is_result_type");
			auto active_context = activate_context(*this);
			if( auto r = this->remote_try_catch_(
					[&]
					{
						return std::forward<TryBlock>(try_block)();
					},
					std::forward<RemoteH>(h)) )
				return r;
			else
			{
				auto rr = this->remote_handle_some(std::move(r), std::forward<RemoteH>(h));
				if( !rr )
					this->propagate();
				return rr;
			}
		}

		////////////////////////////////////////

		inline void exception_info_::print( std::ostream & os ) const
		{
			if( ex_ )
			{
				os <<
					"\nException dynamic type: " << leaf_detail::demangle(typeid(*ex_).name()) <<
					"\nstd::exception::what(): " << ex_->what();
			}
			else
				os << "\nUnknown exception type (not a std::exception)";
		}

		LEAF_CONSTEXPR inline exception_info_::exception_info_( std::exception const * ex ) noexcept:
			exception_info_base(ex)
		{
		}

		template <class... E>
		template <class TryBlock, class... H>
		inline decltype(std::declval<TryBlock>()()) context_base<E...>::try_catch_( TryBlock && try_block, H && ... h )
		{
			using namespace leaf_detail;
			assert(is_active());
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
				catch( std::exception const & ex )
				{
					deactivate();
					return leaf_detail::handle_error_<R>(this->tup(), error_info(exception_info_(&ex)), std::forward<H>(h)...,
						[]() -> R { throw; } );
				}
				catch(...)
				{
					deactivate();
					return leaf_detail::handle_error_<R>(this->tup(), error_info(exception_info_(0)), std::forward<H>(h)...,
						[]() -> R { throw; } );
				}
			}
			catch( std::exception const & ex )
			{
				deactivate();
				return leaf_detail::handle_error_<R>(this->tup(), error_info(exception_info_(&ex)), std::forward<H>(h)...,
					[]() -> R { throw; } );
			}
			catch(...)
			{
				deactivate();
				return leaf_detail::handle_error_<R>(this->tup(), error_info(exception_info_(0)), std::forward<H>(h)...,
					[]() -> R { throw; } );
			}
		}

		template <class... E>
		template <class TryBlock, class RemoteH>
		inline decltype(std::declval<TryBlock>()()) context_base<E...>::remote_try_catch_( TryBlock && try_block, RemoteH && h )
		{
			using namespace leaf_detail;
			assert(is_active());
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
				catch( std::exception const & ex )
				{
					deactivate();
					return std::forward<RemoteH>(h)(error_info(exception_info_(&ex), this)).get();
				}
				catch(...)
				{
					deactivate();
					return std::forward<RemoteH>(h)(error_info(exception_info_(0), this)).get();
				}
			}
			catch( std::exception const & ex )
			{
				deactivate();
				return std::forward<RemoteH>(h)(error_info(exception_info_(&ex), this)).get();
			}
			catch(...)
			{
				deactivate();
				return std::forward<RemoteH>(h)(error_info(exception_info_(0), this)).get();
			}
		}
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class R, class... H>
		struct remote_handle_exception_dispatch_impl
		{
			using result_type = handler_result<H...>;

			LEAF_CONSTEXPR static result_type handle( error_info const & err, H && ... h )
			{
				using Ctx = context_type_from_handlers<H...>;
				return { leaf_detail::handle_error_<R>(static_cast<Ctx const *>(err.remote_handling_ctx_)->tup(), err, std::forward<H>(h)...,
					[]() -> R { throw; } ) };
			}
		};

		template <class... H>
		struct remote_handle_exception_dispatch_impl<void, H...>
		{
			using result_type = handler_result_void<H...>;

			LEAF_CONSTEXPR static result_type handle( error_info const & err, H && ... h )
			{
				using Ctx = context_type_from_handlers<H...>;
				leaf_detail::handle_error_<void>(static_cast<Ctx const *>(err.remote_handling_ctx_)->tup(), err, std::forward<H>(h)...,
					[]{ throw; } );
				return { };
			}
		};

		template <class... H>
		using remote_handle_exception_dispatch = remote_handle_exception_dispatch_impl<handler_pack_return<H...>, H...>;
	}

	template <class... H>
	LEAF_CONSTEXPR inline typename leaf_detail::remote_handle_exception_dispatch<H...>::result_type remote_handle_exception( error_info const & err, H && ... h )
	{
		using namespace leaf_detail;
		return remote_handle_exception_dispatch<H...>::handle(err, std::forward<H>(h)...);
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		inline error_id unpack_error_id( std::exception const * ex )
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

		LEAF_CONSTEXPR inline exception_info_base::exception_info_base( std::exception const * ex ) noexcept:
			ex_(ex)
		{
			assert(!dynamic_cast<capturing_exception const *>(ex_));
		}

		inline exception_info_base::~exception_info_base() noexcept
		{
		}
	}

	inline error_info::error_info( leaf_detail::exception_info_ const & xi, void const * remote_handling_ctx ) noexcept:
		remote_handling_ctx_(remote_handling_ctx),
		xi_(&xi),
		err_id_(leaf_detail::unpack_error_id(xi_->ex_))
	{
	}

	////////////////////////////////////////

	template <class TryBlock, class... H>
	LEAF_CONSTEXPR inline decltype(std::declval<TryBlock>()()) try_catch( TryBlock && try_block, H && ... h )
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

	template <class TryBlock, class RemoteH>
	LEAF_CONSTEXPR inline decltype(std::declval<TryBlock>()()) remote_try_catch( TryBlock && try_block, RemoteH && h )
	{
		using namespace leaf_detail;
		context_type_from_remote_handler<RemoteH> ctx;
		auto active_context = activate_context(ctx);
		return ctx.remote_try_catch_(
			[&]
			{
				return std::forward<TryBlock>(try_block)();
			},
			std::forward<RemoteH>(h));
	}

} }

#endif
