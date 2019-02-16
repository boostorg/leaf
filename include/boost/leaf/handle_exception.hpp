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

	template <class TryBlock, class... H>
	decltype(std::declval<TryBlock>()()) try_catch( TryBlock && try_block, H && ... h )
	{
		using namespace leaf_detail;
		context_type_from_handlers<H...> ctx;
		context_activator active_context(ctx, context_activator::on_deactivation::propagate_if_uncaught_exception);
		return ctx.try_catch_(
			[&]
			{
				return std::forward<TryBlock>(try_block)();
			},
			std::forward<H>(h)...);
	}

	template <class TryBlock, class RemoteH>
	decltype(std::declval<TryBlock>()()) remote_try_catch( TryBlock && try_block, RemoteH && h )
	{
		using namespace leaf_detail;
		context_type_from_remote_handler<RemoteH> ctx;
		context_activator active_context(ctx, context_activator::on_deactivation::propagate_if_uncaught_exception);
		return ctx.remote_try_catch_(
			[&]
			{
				return std::forward<TryBlock>(try_block)();
			},
			std::forward<RemoteH>(h));
	}

	namespace leaf_detail
	{
		template <class... E>
		template <class TryBlock, class... H>
		typename std::decay<decltype(std::declval<TryBlock>()().value())>::type catch_context<E...>::try_handle_all( TryBlock && try_block, H && ... h ) noexcept
		{
			context_activator active_context(*this, context_activator::on_deactivation::do_not_propagate);
			return this->try_catch_(
				[&]
				{
					return this->try_handle_all_( std::forward<TryBlock>(try_block), std::forward<H>(h)... );
				},
				std::forward<H>(h)...);
		}

		template <class... E>
		template <class TryBlock, class RemoteH>
		typename std::decay<decltype(std::declval<TryBlock>()().value())>::type catch_context<E...>::remote_try_handle_all( TryBlock && try_block, RemoteH && h ) noexcept
		{
			context_activator active_context(*this, context_activator::on_deactivation::do_not_propagate);
			return this->try_catch_(
				[&]
				{
					return this->remote_try_handle_all_( std::forward<TryBlock>(try_block), std::forward<RemoteH>(h) );
				},
				std::forward<RemoteH>(h));
		}

		template <class... E>
		template <class TryBlock, class... H>
		typename std::decay<decltype(std::declval<TryBlock>()())>::type catch_context<E...>::try_handle_some( TryBlock && try_block, H && ... h )
		{
			context_activator active_context(*this, context_activator::on_deactivation::propagate_if_uncaught_exception);
			return this->try_catch_(
				[&]
				{
					return this->try_handle_some_( active_context, std::forward<TryBlock>(try_block), std::forward<H>(h)... );
				},
				std::forward<H>(h)...);
		}

		template <class... E>
		template <class TryBlock, class RemoteH>
		typename std::decay<decltype(std::declval<TryBlock>()())>::type catch_context<E...>::remote_try_handle_some( TryBlock && try_block, RemoteH && h )
		{
			context_activator active_context(*this, context_activator::on_deactivation::propagate_if_uncaught_exception);
			return this->try_catch_(
				[&]
				{
					return this->remote_try_handle_some_( active_context, std::forward<TryBlock>(try_block), std::forward<RemoteH>(h) );
				},
				std::forward<RemoteH>(h));
		}

		////////////////////////////////////////

		template <class... E>
		template <class R, class... H>
		R catch_context<E...>::handle_current_exception( H && ... h ) const
		{
			return this->try_catch_(
				[ ]{ throw; },
				std::forward<H>(h)...);
		}

		template <class... E>
		template <class R, class RemoteH>
		R catch_context<E...>::remote_handle_current_exception( RemoteH && h ) const
		{
			return this->remote_try_catch_(
				[ ]() -> R { throw; },
				std::forward<RemoteH>(h));
		}

			template <class... E>
			template <class R, class... H>
			R catch_context<E...>::handle_exception( std::exception_ptr const & ep, H && ... h ) const
			{
				return this->try_catch_(
					[&]{ std::rethrow_exception(ep); },
					std::forward<H>(h)...);
			}

		template <class... E>
		template <class R, class RemoteH>
		R catch_context<E...>::remote_handle_exception( std::exception_ptr const & ep, RemoteH && h  ) const
		{
			return this->remote_try_catch_(
				[&]() -> R { std::rethrow_exception(ep); },
				std::forward<RemoteH>(h));
		}

		////////////////////////////////////////

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

		template <class... E>
		template <class TryBlock, class... H>
		decltype(std::declval<TryBlock>()()) context_base<E...>::try_catch_( TryBlock && try_block, H && ... h ) const
		{
			using namespace leaf_detail;
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
					return leaf_detail::handle_error_<R>(this->tup(), error_info(*this, exception_info_(&ex)), std::forward<H>(h)...,
						[ ]() -> R { throw; } );
				}
				catch(...)
				{
					return leaf_detail::handle_error_<R>(this->tup(), error_info(*this, exception_info_(0)), std::forward<H>(h)...,
						[ ]() -> R { throw; } );
				}
			}
			catch( std::exception const & ex )
			{
				return leaf_detail::handle_error_<R>(this->tup(), error_info(*this, exception_info_(&ex)), std::forward<H>(h)...,
					[ ]() -> R { throw; } );
			}
			catch(...)
			{
				return leaf_detail::handle_error_<R>(this->tup(), error_info(*this, exception_info_(0)), std::forward<H>(h)...,
					[ ]() -> R { throw; } );
			}
		}

		template <class... E>
		template <class TryBlock, class RemoteH>
		decltype(std::declval<TryBlock>()()) context_base<E...>::remote_try_catch_( TryBlock && try_block, RemoteH && h ) const
		{
			using namespace leaf_detail;
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
					return std::forward<RemoteH>(h)(error_info(*this, exception_info_(&ex))).get();
				}
				catch(...)
				{
					return std::forward<RemoteH>(h)(error_info(*this, exception_info_(0))).get();
				}
			}
			catch( std::exception const & ex )
			{
				return std::forward<RemoteH>(h)(error_info(*this, exception_info_(&ex))).get();
			}
			catch(...)
			{
				return std::forward<RemoteH>(h)(error_info(*this, exception_info_(0))).get();
			}
		}
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
		inline std::error_code const * unpack_error_code( std::exception const * ex )
		{
			if( std::system_error const * se = dynamic_cast<std::system_error const *>(ex) )
				return &se->code();
			else if( error_id const * err_id = dynamic_cast<error_id const *>(ex) )
				return err_id;
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
			ex_(ex)
		{
			assert(!dynamic_cast<capturing_exception const *>(ex_));
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

} }

#endif
