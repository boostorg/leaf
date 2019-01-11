#ifndef BOOST_LEAF_BC24FB98B2DE11E884419CF5AD35F1A2
#define BOOST_LEAF_BC24FB98B2DE11E884419CF5AD35F1A2

//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/dynamic_store_impl.hpp>
#include <boost/leaf/detail/captured_exception.hpp>
#include <typeinfo>
#include <memory>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		class captured_exception_impl:
			public captured_exception,
			public leaf::error
		{
			std::exception_ptr ex_;
			std::shared_ptr<dynamic_store> ds_;
			bool had_error_;

		public:

			captured_exception_impl( std::exception_ptr && ex, std::shared_ptr<dynamic_store> && ds, bool had_error ) noexcept:
				error(ds->get_error()),
				ex_(std::move(ex)),
				ds_(std::move(ds)),
				had_error_(had_error)
			{
				assert(ex_);
			}

			[[noreturn]] void unload_and_rethrow_original_exception()
			{
				std::shared_ptr<dynamic_store> ds; ds.swap(ds_);
				assert(ds);
				if( had_error_ )
				{
					error err = ds->unload();
					assert(err==*this);
					(void) err;
				}
				else
					ds->unload(next_error_value());
				std::rethrow_exception(ex_);
			}

			void print( std::ostream & os ) const
			{
				assert(ds_);
				ds_->print(os);
			}
		};

		////////////////////////////////////////

		template <class F, class... E>
		class exception_trap
		{
			F f_;

		public:

			explicit exception_trap( F && f ) noexcept:
				f_(std::move(f))
			{
			}

			template <class... A>
			decltype(std::declval<F>()(std::declval<A>()...)) operator()( A && ... a ) const
			{
				static_store<E...> ss;
				ss.set_reset(true);
				try
				{
					return f_(std::forward<A>(a)...);
				}
				catch( captured_exception const & )
				{
					throw;
				}
				catch( error const & e )
				{
					throw captured_exception_impl( std::current_exception(), std::make_shared<dynamic_store_impl<E...>>(e,std::move(ss)), true );
				}
				catch(...)
				{
					throw captured_exception_impl( std::current_exception(), std::make_shared<dynamic_store_impl<E...>>(new_error(),std::move(ss)), false );
				}
			}
		};
	} //leaf_detail

	template <class... E, class F>
	leaf_detail::exception_trap<F,E...> capture_exception( F && f ) noexcept
	{
		return leaf_detail::exception_trap<F,E...>(std::move(f));
	}

} }

#endif
