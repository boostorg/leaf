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

		template <class F, class mp_args, class... E>
		class exception_trap;

		template <class F, template<class...> class L, class... A, class... E>
		class exception_trap<F, L<A...>,E...>
		{
			F f_;

			using R = decltype(std::declval<F>()(std::declval<A>()...));

			R call_f( result_tag<R, false>, static_store<E...> &&, A... a ) const
			{
				return f_(std::forward<A>(a)...);
			}

			R call_f( result_tag<R, true>, static_store<E...> && ss, A... a ) const
			{
				if( auto r = f_(std::forward<A>(a)...) )
					return r;
				else
					return decltype(r)( std::make_shared<dynamic_store_impl<E...>>(r.error().value(),std::move(ss)) );
			}

		public:

			explicit exception_trap( F && f ) noexcept:
				f_(std::forward<F>(f))
			{
			}

			R operator()( A... a ) const
			{
				typename deduce_static_store<typename error_type_set<E...>::type>::type ss;
				ss.set_reset(true);
				try
				{
					return call_f( result_tag<R>(), std::move(ss), std::forward<A>(a)...); //Note, ss will not be actually moved if exception thrown.
				}
				catch( capturing_exception const & )
				{
					throw;
				}
				catch( error_id const & err )
				{
					throw_exception(capturing_exception_impl( std::current_exception(), std::make_shared<dynamic_store_impl<E...>>(err.value(),std::move(ss)), true, &print_types<E...>::print ));
				}
				catch(...)
				{
					throw_exception(capturing_exception_impl( std::current_exception(), std::make_shared<dynamic_store_impl<E...>>(leaf_detail::new_id(),std::move(ss)), false, &print_types<E...>::print ));
				}
			}
		};

	} // leaf_detail

	template <class... E, class F>
	leaf_detail::exception_trap<F,typename leaf_detail::function_traits<F>::mp_args,E...> capture_in_exception( F && f ) noexcept
	{
		using namespace leaf_detail;
		return exception_trap<F,typename function_traits<F>::mp_args,E...>(std::move(f));
	}

} }

#endif
