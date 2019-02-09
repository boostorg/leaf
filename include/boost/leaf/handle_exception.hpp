#ifndef BOOST_LEAF_87F274C4D4BA11E89928D55AC82C3C47
#define BOOST_LEAF_87F274C4D4BA11E89928D55AC82C3C47

// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/handle_error.hpp>
#include <boost/leaf/capture.hpp>
#include <boost/leaf/detail/demangle.hpp>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		class current_exception_impl: public current_exception
		{
			current_exception_impl( current_exception_impl const & ) = delete;
			current_exception_impl & operator=( current_exception_impl const & ) = delete;

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

			current_exception_impl( std::exception const * ex ) noexcept:
				current_exception(ex)
			{
			}
		};

		template <class TryBlock, class... Handler>
		typename std::decay<decltype(std::declval<TryBlock>()())>::type try_catch_impl( is_result_tag<decltype(std::declval<TryBlock>()()), true>, TryBlock && try_block, Handler && ... handler )
		{
			using namespace leaf_detail;
			using R = typename std::decay<decltype(std::declval<TryBlock>()())>::type;
			context_type_from_handlers<Handler...> ctx;
			context_activator active_context(ctx, false);
			try
			{
				if( auto r = std::forward<TryBlock>(try_block)() )
					return r;
				else
				{
					auto rr = leaf_detail::handle_error_(ctx.tup(), error_info(r.error()), std::forward<Handler>(handler)..., [&r] { return r; });
					if( !rr )
						active_context.set_propagate_errors(true);
					return rr;
				}
			}
			catch( std::exception const & ex )
			{
				return leaf_detail::handle_error_(ctx.tup(), error_info(&ex), std::forward<Handler>(handler)...,
					[ ]() -> R { throw; } );
			}
			catch(...)
			{
				return leaf_detail::handle_error_(ctx.tup(), error_info((std::exception const *)0), std::forward<Handler>(handler)...,
					[ ]() -> R { throw; } );
			}
		}

		template <class TryBlock, class... Handler>
		typename std::decay<decltype(std::declval<TryBlock>()())>::type try_catch_impl( is_result_tag<decltype(std::declval<TryBlock>()()), false>, TryBlock && try_block, Handler && ... handler )
		{
			using namespace leaf_detail;
			using R = typename std::decay<decltype(std::declval<TryBlock>()())>::type;
			context_type_from_handlers<Handler...> ctx;
			context_activator active_context(ctx, false);
			try
			{
				return std::forward<TryBlock>(try_block)();
			}
			catch( std::exception const & ex )
			{
				return leaf_detail::handle_error_(ctx.tup(), error_info(&ex), std::forward<Handler>(handler)...,
					[ ]() -> R { throw; } );
			}
			catch(...)
			{
				return leaf_detail::handle_error_(ctx.tup(), error_info((std::exception const *)0), std::forward<Handler>(handler)...,
					[ ]() -> R { throw; } );
			}
		}
	}

	template <class TryBlock, class... Handler>
	typename std::decay<decltype(std::declval<TryBlock>()())>::type try_catch( TryBlock && try_block, Handler && ... handler )
	{
		using namespace leaf_detail;
		using R = typename std::decay<decltype(std::declval<TryBlock>()())>::type;
		return try_catch_impl( is_result_tag<R>(), std::forward<TryBlock>(try_block), std::forward<Handler>(handler)...);
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class TryBlock, class RemoteHandler>
		typename std::decay<decltype(std::declval<TryBlock>()())>::type remote_try_catch_impl( is_result_tag<decltype(std::declval<TryBlock>()()), true>, TryBlock && try_block, RemoteHandler && handler )
		{
			using namespace leaf_detail;
			context_type_from_remote_handler<RemoteHandler> ctx;
			context_activator active_context(ctx, false);
			try
			{
				if( auto r = std::forward<TryBlock>(try_block)() )
					return r;
				else
				{
					auto rr = std::forward<RemoteHandler>(handler)(error(&ctx, r.error())).get();
					if( !rr )
						active_context.set_propagate_errors(true);
					return rr;
				}
			}
			catch( std::exception const & ex )
			{
				return std::forward<RemoteHandler>(handler)(error(&ctx, error_info(&ex))).get();
			}
			catch(...)
			{
				return std::forward<RemoteHandler>(handler)(error(&ctx, error_info((std::exception const *)0))).get();
			}
		}

		template <class TryBlock, class RemoteHandler>
		typename std::decay<decltype(std::declval<TryBlock>()())>::type remote_try_catch_impl( is_result_tag<decltype(std::declval<TryBlock>()()), false>, TryBlock && try_block, RemoteHandler && handler )
		{
			using namespace leaf_detail;
			context_type_from_remote_handler<RemoteHandler> ctx;
			context_activator active_context(ctx, false);
			try
			{
				return std::forward<TryBlock>(try_block)();
			}
			catch( std::exception const & ex )
			{
				return std::forward<RemoteHandler>(handler)(error(&ctx, &ex)).get();
			}
			catch(...)
			{
				return std::forward<RemoteHandler>(handler)(error(&ctx, (std::exception const *)0)).get();
			}
		}
	}

	template <class TryBlock, class RemoteHandler>
	typename std::decay<decltype(std::declval<TryBlock>()())>::type remote_try_catch( TryBlock && try_block, RemoteHandler && handler )
	{
		using namespace leaf_detail;
		using R = typename std::decay<decltype(std::declval<TryBlock>()())>::type;
		return remote_try_catch_impl( is_result_tag<R>(), std::forward<TryBlock>(try_block), std::forward<RemoteHandler>(handler));
	}

	////////////////////////////////////////

	namespace leaf_detail
	{
		template <class R, class... Handler>
		struct remote_catch_dispatch_impl
		{
			using result_type = handler_result<Handler...>;

			static result_type handle( error const & err, Handler && ... handler )
			{
				using Ctx = context_type_from_handlers<Handler...>;
				return { leaf_detail::handle_error_(reinterpret_cast<Ctx const *>(err.ctx_)->tup(), err, std::forward<Handler>(handler)...,
					[ ]() -> R { throw; } ) };
			}
		};

		template <class... Handler>
		struct remote_catch_dispatch_impl<void, Handler...>
		{
			using result_type = handler_result_void<Handler...>;

			static result_type handle( error const & err, Handler && ... handler )
			{
				using Ctx = context_type_from_handlers<Handler...>;
				leaf_detail::handle_error_(reinterpret_cast<Ctx const *>(err.ctx_)->tup(), err, std::forward<Handler>(handler)...,
					[ ]{ throw; } );
				return { };
			}
		};

		template <class... Handler>
		using remote_catch_dispatch = remote_catch_dispatch_impl<handler_pack_return<Handler...>, Handler...>;
	}

	template <class... Handler>
	typename leaf_detail::remote_catch_dispatch<Handler...>::result_type remote_catch( error const & err, Handler && ... handler )
	{
		using namespace leaf_detail;
		return remote_catch_dispatch<Handler...>::handle( err, std::forward<Handler>(handler)... );
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

		inline int unpack_error_id( std::error_code const * ec )
		{
			if( ec )
				if( is_error_id(*ec) )
					return ec->value();
				else if(*ec)
					return next_id();
				else
					return 0;
			else
				return next_id();
		}

		inline current_error::current_error( std::exception const * ex ) noexcept:
			ex_(unpack_capturing_exception(ex)),
			ec_(unpack_error_code(ex_)),
			err_id_(unpack_error_id(ec_))
		{
		}

		inline current_exception::current_exception( std::exception const * ex ) noexcept:
			current_error(ex)
		{
		}

		inline current_exception::~current_exception() noexcept
		{
		}
	}

	inline error_info::error_info( leaf_detail::current_exception_impl const & ex ) noexcept:
		err_(ex),
		ex_(&ex)
	{
	}

	inline error::error( void const * ctx, leaf_detail::current_exception_impl const & err ) noexcept:
		error_info(err),
		ctx_(ctx)
	{
		assert(ctx_!=0);
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
