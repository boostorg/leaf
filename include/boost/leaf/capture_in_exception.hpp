#ifndef BOOST_LEAF_BC24FB98B2DE11E884419CF5AD35F1A2
#define BOOST_LEAF_BC24FB98B2DE11E884419CF5AD35F1A2

// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/capturing_exception.hpp>
#include <boost/leaf/detail/dynamic_store_impl.hpp>
#include <boost/leaf/throw_exception.hpp>
#include <memory>

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		class capturing_exception_impl:
			public capturing_exception,
			public error_id
		{
			std::exception_ptr ex_;
			std::shared_ptr<dynamic_store> ds_;
			bool had_error_;
			void (*print_captured_types_)(std::ostream &);

		public:

			capturing_exception_impl( std::exception_ptr && ex, std::shared_ptr<dynamic_store> && ds, bool had_error, void (*print_captured_types)(std::ostream &) ) noexcept:
				error_id(leaf_detail::make_error_id(ds->err_id())),
				ex_(std::move(ex)),
				ds_(std::move(ds)),
				had_error_(had_error),
				print_captured_types_(print_captured_types)
			{
				assert(ex_);
				assert(print_captured_types_!=0);
			}

			[[noreturn]] void unload_and_rethrow_original_exception()
			{
				std::shared_ptr<dynamic_store> ds; ds.swap(ds_);
				assert(ds);
				if( had_error_ )
				{
					int err_id = ds->unload();
					assert(err_id==value());
					(void) err_id;
				}
				else
					ds->unload(leaf_detail::next_id());
				std::rethrow_exception(ex_);
			}

			void print( std::ostream & os ) const
			{
				print_captured_types_(os);
			}
		};

		////////////////////////////////////////

		template <class... T>
		struct print_types;

		template <class Car, class... Cdr>
		struct print_types<Car,Cdr...>
		{
			static void print( std::ostream & os )
			{
				os << '\t' << type<Car>() << std::endl;
				print_types<Cdr...>::print(os);
			}
		};

		template <>
		struct print_types<>
		{
			static void print( std::ostream & )
			{
			}
		};

		////////////////////////////////////////

		template <class... E>
		[[noreturn]] static std::shared_ptr<dynamic_store> capture_in_exception_throw(int err_id, static_store<E...> && ss, bool had_error)
		{
			assert(err_id!=0);
			throw_exception(capturing_exception_impl( std::current_exception(), std::make_shared<dynamic_store_impl<E...>>(err_id,std::move(ss)), had_error, &print_types<E...>::print ));
		}

		template <class Alloc, class... E>
		[[noreturn]] static std::shared_ptr<dynamic_store> capture_in_exception_throw(Alloc alloc, int err_id, static_store<E...> && ss, bool had_error)
		{
			assert(err_id!=0);
			throw_exception(capturing_exception_impl( std::current_exception(), std::allocate_shared<dynamic_store_impl<E...>>(alloc, err_id, std::move(ss)), had_error, &print_types<E...>::print));
		}

		template <class R, class... E, class F, class... A>
		R capture_in_exception_impl( result_tag<R, false>, static_store<E...> && ss, F && f, A... a )
		{
			ss.set_reset(true);
			try
			{
				return std::forward<F>(f)(std::forward<A>(a)...);
			}
			catch( capturing_exception const & )
			{
				throw;
			}
			catch( error_id const & err )
			{
				capture_in_exception_throw(err.value(), std::move(ss), true);
			}
			catch(...)
			{
				capture_in_exception_throw(leaf_detail::new_id(), std::move(ss), false);
			}
		}

		template <class R, class... E, class F, class... A>
		R capture_in_exception_impl( result_tag<R, true>, static_store<E...> && ss, F && f, A... a )
		{
			ss.set_reset(true);
			try
			{
				if( auto r = std::forward<F>(f)(std::forward<A>(a)...) )
					return r;
				else
					return R( std::make_shared<dynamic_store_impl<E...>>(r.error().value(),std::move(ss)) );
			}
			catch( capturing_exception const & )
			{
				throw;
			}
			catch( error_id const & err )
			{
				capture_in_exception_throw(err.value(), std::move(ss), true);
			}
			catch(...)
			{
				capture_in_exception_throw(leaf_detail::new_id(), std::move(ss), false);
			}
		}

		template <class Alloc, class R, class... E, class F, class... A>
		R capture_in_exception_impl( Alloc alloc, result_tag<R, false>, static_store<E...> && ss, F && f, A... a )
		{
			ss.set_reset(true);
			try
			{
				return std::forward<F>(f)(std::forward<A>(a)...);
			}
			catch( capturing_exception const & )
			{
				throw;
			}
			catch( error_id const & err )
			{
				capture_in_exception_throw(alloc, err.value(), std::move(ss), true);
			}
			catch(...)
			{
				capture_in_exception_throw(alloc, leaf_detail::new_id(), std::move(ss), false);
			}
		}

		template <class Alloc, class R, class... E, class F, class... A>
		R capture_in_exception_impl( Alloc alloc, result_tag<R, true>, static_store<E...> && ss, F && f, A... a )
		{
			ss.set_reset(true);
			try
			{
				if( auto r = std::forward<F>(f)(std::forward<A>(a)...) )
					return r;
				else
					return R( std::allocate_shared<dynamic_store_impl<E...>>(alloc, r.error().value(),std::move(ss)) );
			}
			catch( capturing_exception const & )
			{
				throw;
			}
			catch( error_id const & err )
			{
				capture_in_exception_throw(alloc, err.value(), std::move(ss), true);
			}
			catch(...)
			{
				capture_in_exception_throw(alloc, leaf_detail::new_id(), std::move(ss), false);
			}
		}

	} // leaf_detail

	template <class... E, class F, class... A>
	decltype(std::declval<F>()(std::forward<A>(std::declval<A>())...)) capture_in_exception_explicit(F && f, A... a)
	{
		using namespace leaf_detail;
		using R = decltype(std::declval<F>()(std::forward<A>(a)...));
		using StaticStore = deduce_static_store<error_type_set<e_original_ec, E...>>;
		StaticStore ss;
		return capture_in_exception_impl( result_tag<R>(), std::move(ss), std::forward<F>(f), std::forward<A>(a)...);
	}

	template <class... E, class Alloc, class F, class... A>
	decltype(std::declval<F>()(std::forward<A>(std::declval<A>())...)) capture_in_exception_explicit(Alloc alloc, F && f, A... a)
	{
		using namespace leaf_detail;
		using R = decltype(std::declval<F>()(std::forward<A>(a)...));
		using StaticStore = deduce_static_store<error_type_set<e_original_ec, E...>>;
		StaticStore ss;
		return capture_in_exception_impl( alloc, result_tag<R>(), std::move(ss), std::forward<F>(f), std::forward<A>(a)...);
	}

	template <class Handler, class F, class... A>
	decltype(std::declval<F>()(std::forward<A>(std::declval<A>())...)) capture_in_exception(F && f, A... a)
	{
		using namespace leaf_detail;
		using R = decltype(std::declval<F>()(std::forward<A>(a)...));
		using StaticStore = deduce_static_store<leaf_detail::handler_args_list<fn_return_type<Handler>>>;
		StaticStore ss;
		return capture_in_exception_impl( result_tag<R>(), std::move(ss), std::forward<F>(f), std::forward<A>(a)...);
	}

	template <class Handler, class Alloc, class F, class... A>
	decltype(std::declval<F>()(std::forward<A>(std::declval<A>())...)) capture_in_exception(Alloc alloc, F && f, A... a)
	{
		using namespace leaf_detail;
		using R = decltype(std::declval<F>()(std::forward<A>(a)...));
		using StaticStore = deduce_static_store<leaf_detail::handler_args_list<fn_return_type<Handler>>>;
		StaticStore ss;
		return capture_in_exception_impl( alloc, result_tag<R>(), std::move(ss), std::forward<F>(f), std::forward<A>(a)...);
	}

} }

#endif
