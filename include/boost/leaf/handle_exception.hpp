#ifndef BOOST_LEAF_87F274C4D4BA11E89928D55AC82C3C47
#define BOOST_LEAF_87F274C4D4BA11E89928D55AC82C3C47

// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/capture.hpp>
#include <boost/leaf/handle_error.hpp>
#include <boost/leaf/detail/demangle.hpp>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		class exception_info_: public exception_info_base
		{
			exception_info_( exception_info_ const & ) = delete;
			exception_info_ & operator=( exception_info_ const & ) = delete;

			void print( std::ostream & os ) const
			{
				if( ex_ )
				{
					os <<
						"Exception dynamic type: " << leaf_detail::demangle(typeid(*ex_).name()) << std::endl <<
						"std::exception::what(): " << ex_->what() << std::endl;
				}
				else
					os << "Unknown exception type (not a std::exception)" << std::endl;
			}

		public:

			explicit exception_info_( std::exception const * ex ) noexcept:
				exception_info_base(ex)
			{
			}
		};

		template <class Thrower, class... E, class... H>
		decltype(std::declval<Thrower>()()) handle_exceptions( Thrower && thr, context<E...> const & ctx, H && ... h )
		{
			using namespace leaf_detail;
			using R = decltype(std::declval<Thrower>()());
			try
			{
				return std::forward<Thrower>(thr)();
			}
			catch( std::exception const & ex )
			{
				return leaf_detail::handle_error_<R>(ctx.tup(), error_info(ctx, exception_info_(&ex)), std::forward<H>(h)...,
					[ ]() -> R { throw; } );
			}
			catch(...)
			{
				return leaf_detail::handle_error_<R>(ctx.tup(), error_info(ctx, exception_info_(0)), std::forward<H>(h)...,
					[ ]() -> R { throw; } );
			}
		}

		template <class Thrower, class... E, class RemoteH>
		decltype(std::declval<Thrower>()()) remote_handle_exceptions( Thrower && thr, context<E...> const & ctx, RemoteH && h )
		{
			using namespace leaf_detail;
			try
			{
				return std::forward<Thrower>(thr)();
			}
			catch( std::exception const & ex )
			{
				return std::forward<RemoteH>(h)(error_info(ctx, exception_info_(&ex))).get();
			}
			catch(...)
			{
				return std::forward<RemoteH>(h)(error_info(ctx, exception_info_(0))).get();
			}
		}
	}

	template <class... E, class... H>
	leaf_detail::handler_pack_return<H...> handle_current_exception( context<E...> const & ctx, H && ... h ) noexcept
	{
		return leaf_detail::handle_exceptions(
			[ ]{ throw; },
			ctx, std::forward<H>(h)...);
	}

	template <class... E, class... H>
	leaf_detail::handler_pack_return<H...> handle_exception( context<E...> const & ctx, std::exception_ptr const & ep, H && ... h ) noexcept
	{
		return leaf_detail::handle_exceptions(
			[&]{ std::rethrow_exception(ep); },
			ctx, std::forward<H>(h)...);
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class TryBlock, class... H>
		typename std::decay<decltype(std::declval<TryBlock>()())>::type try_catch_impl( is_result_tag<decltype(std::declval<TryBlock>()()), true>, TryBlock && try_block, H && ... h )
		{
			using namespace leaf_detail;
			context_type_from_handlers<H...> ctx;
			context_activator active_context(ctx, context_activator::on_deactivation::propagate_if_current_exception);
			return handle_exceptions(
				[&]
				{
					if( auto r = std::forward<TryBlock>(try_block)() )
						return r;
					else
					{
						auto rr = handle_some(ctx, r, std::forward<H>(h)...);
						if( !rr )
							active_context.set_propagate_errors(context_activator::on_deactivation::propagate);
						return rr;
					}
				},
				ctx, std::forward<H>(h)...);
		}

		template <class TryBlock, class... H>
		typename std::decay<decltype(std::declval<TryBlock>()())>::type try_catch_impl( is_result_tag<decltype(std::declval<TryBlock>()()), false>, TryBlock && try_block, H && ... h )
		{
			using namespace leaf_detail;
			context_type_from_handlers<H...> ctx;
			context_activator active_context(ctx, context_activator::on_deactivation::propagate_if_current_exception);
			return handle_exceptions(
				[&]
				{
					return std::forward<TryBlock>(try_block)();
				},
				ctx, std::forward<H>(h)...);
		}
	}

	template <class TryBlock, class... H>
	typename std::decay<decltype(std::declval<TryBlock>()())>::type try_catch( TryBlock && try_block, H && ... h )
	{
		using namespace leaf_detail;
		using R = typename std::decay<decltype(std::declval<TryBlock>()())>::type;
		return try_catch_impl( is_result_tag<R>(), std::forward<TryBlock>(try_block), std::forward<H>(h)...);
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class TryBlock, class RemoteH>
		typename std::decay<decltype(std::declval<TryBlock>()())>::type remote_try_catch_impl( is_result_tag<decltype(std::declval<TryBlock>()()), true>, TryBlock && try_block, RemoteH && h )
		{
			using namespace leaf_detail;
			context_type_from_remote_handler<RemoteH> ctx;
			context_activator active_context(ctx, context_activator::on_deactivation::propagate_if_current_exception);
			return remote_handle_exceptions(
				[&]
				{
					if( auto r = std::forward<TryBlock>(try_block)() )
						return r;
					else
					{
						auto rr = std::forward<RemoteH>(h)(error_info(ctx, r.error())).get();
						if( !rr )
							active_context.set_propagate_errors(context_activator::on_deactivation::propagate);
						return rr;
					}
				},
				ctx, std::forward<RemoteH>(h));
		}

		template <class TryBlock, class RemoteH>
		typename std::decay<decltype(std::declval<TryBlock>()())>::type remote_try_catch_impl( is_result_tag<decltype(std::declval<TryBlock>()()), false>, TryBlock && try_block, RemoteH && h )
		{
			using namespace leaf_detail;
			context_type_from_remote_handler<RemoteH> ctx;
			context_activator active_context(ctx, context_activator::on_deactivation::propagate_if_current_exception);
			return remote_handle_exceptions(
				[&]
				{
					return std::forward<TryBlock>(try_block)();
				},
				ctx, std::forward<RemoteH>(h));
		}
	}

	template <class TryBlock, class RemoteH>
	typename std::decay<decltype(std::declval<TryBlock>()())>::type remote_try_catch( TryBlock && try_block, RemoteH && h )
	{
		using namespace leaf_detail;
		using R = typename std::decay<decltype(std::declval<TryBlock>()())>::type;
		return remote_try_catch_impl( is_result_tag<R>(), std::forward<TryBlock>(try_block), std::forward<RemoteH>(h));
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class R, class... H>
		struct remote_catch_dispatch_impl
		{
			using result_type = handler_result<H...>;

			static result_type handle( error_info const & err, H && ... h )
			{
				using Ctx = context_type_from_handlers<H...>;
				return { leaf_detail::handle_error_<R>(static_cast<Ctx const &>(err.ctx_).tup(), err, std::forward<H>(h)...,
					[ ]() -> R { throw; } ) };
			}
		};

		template <class... H>
		struct remote_catch_dispatch_impl<void, H...>
		{
			using result_type = handler_result_void<H...>;

			static result_type handle( error_info const & err, H && ... h )
			{
				using Ctx = context_type_from_handlers<H...>;
				leaf_detail::handle_error_<void>(static_cast<Ctx const &>(err.ctx_).tup(), err, std::forward<H>(h)...,
					[ ]{ throw; } );
				return { };
			}
		};

		template <class... H>
		using remote_catch_dispatch = remote_catch_dispatch_impl<handler_pack_return<H...>, H...>;
	}

	template <class... H>
	typename leaf_detail::remote_catch_dispatch<H...>::result_type remote_catch( error_info const & err, H && ... h )
	{
		using namespace leaf_detail;
		return remote_catch_dispatch<H...>::handle(err, std::forward<H>(h)...);
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		inline std::exception const * unpack_capturing_exception( std::exception const * ex )
		{
			if( capturing_exception const * cap = dynamic_cast<capturing_exception const *>(ex) )
				try
				{
					cap->unload_and_rethrow_original_exception();
				}
				catch( std::exception const & ex )
				{
					return &ex;
				}
				catch(...)
				{
					return 0;
				}
			else
				return ex;
		}

		inline std::error_code const * unpack_error_code( std::exception const * ex )
		{
			if( std::system_error const * se = dynamic_cast<std::system_error const *>(ex) )
				return &se->code();
			else if( std::error_code const * ec = dynamic_cast<std::error_code const *>(ex) )
				return ec;
			else
				return 0;
		}

		inline error_id unpack_error_id( std::error_code const * ec )
		{
			if( ec )
				return error_id(*ec);
			else
				return next_error();
		}

		inline exception_info_base::exception_info_base( std::exception const * ex ) noexcept:
			ex_(unpack_capturing_exception(ex))
		{
		}

		inline exception_info_base::~exception_info_base() noexcept
		{
		}
	}

	inline error_info::error_info( polymorphic_context const & ctx, leaf_detail::exception_info_ const & xi ) noexcept:
		ctx_(ctx),
		xi_(&xi),
		ec_(leaf_detail::unpack_error_code(xi_->ex_)),
		err_id_(leaf_detail::unpack_error_id(ec_))
	{
	}

	////////////////////////////////////////

	inline error_id get_error( std::exception const & ex ) noexcept
	{
		if( auto err = dynamic_cast<error_id const *>(&ex) )
			return *err;
		else
			return next_error();
	}

} }

#endif
