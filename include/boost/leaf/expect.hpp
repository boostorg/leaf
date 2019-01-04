#ifndef BOOST_LEAF_AFBBD676B2FF11E8984C7976AE35F1A2
#define BOOST_LEAF_AFBBD676B2FF11E8984C7976AE35F1A2

//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/handle_error.hpp>
#include <boost/leaf/error.hpp>
#include <tuple>

namespace boost { namespace leaf {

	class error_capture;

	template <class T>
	class result;

	namespace leaf_detail
	{
		template <class E>
		class expect_slot:
			public slot<E>
		{
		};

		class expect_slot_enable_unexpected
		{
		protected:
			expect_slot_enable_unexpected() noexcept
			{
				++tl_unexpected_enabled_counter();
			}

			~expect_slot_enable_unexpected() noexcept
			{
				--tl_unexpected_enabled_counter();
			}
		};

		template <>
		class expect_slot<e_unexpected>:
			public slot<e_unexpected>,
			expect_slot_enable_unexpected
		{
		};

		template <>
		class expect_slot<e_unexpected_diagnostic_output>:
			public slot<e_unexpected_diagnostic_output>,
			expect_slot_enable_unexpected
		{
		};

		////////////////////////////////////////

		template <class T, class... List>
		struct type_index;

		template <class T, class... Cdr>
		struct type_index<T, T, Cdr...>
		{
			static const int value = 0;
		};

		template <class T, class Car, class... Cdr>
		struct type_index<T, Car, Cdr...>
		{
			static const int value = 1 + type_index<T,Cdr...>::value;
		};

		template <class T, class Tuple>
		struct tuple_type_index;

		template <class T, class... TupleTypes>
		struct tuple_type_index<T,std::tuple<TupleTypes...>>
		{
			static const int value = type_index<T,TupleTypes...>::value;
		};

		////////////////////////////////////////

		template <class SlotsTuple, class... List>
		struct slots_subset;

		template <class SlotsTuple, class Car, class... Cdr>
		struct slots_subset<SlotsTuple, Car, Cdr...>
		{
			static bool have_values( SlotsTuple const & tup, error const & e ) noexcept
			{
				auto & sl = std::get<tuple_type_index<Car,SlotsTuple>::value>(tup);
				return sl.has_value() && sl.value().e==e && slots_subset<SlotsTuple,Cdr...>::have_values(tup,e);
			}
		};

		template <class SlotsTuple>
		struct slots_subset<SlotsTuple>
		{
			static constexpr bool have_values( SlotsTuple const &, error const & ) noexcept { return true; }
		};

		////////////////////////////////////////

		template <class T>
		optional<T> convert_optional( expect_slot<T> && x, error const & e ) noexcept
		{
			if( x.has_value() && x.value().e==e )
				return optional<T>(std::move(x).value().v);
			else
				return optional<T>();
		}

		template <class>
		struct dependent_type
		{
			typedef leaf::error_capture error_capture;
		};
	} //leaf_detail

	template <class... E>
	class expect;

	template <class... E>
	typename leaf_detail::dependent_type<expect<E...>>::error_capture capture( expect<E...> &, error const & );

	template <class... E>
	class expect
	{
		friend class error;

		template <class... E_>
		friend typename leaf_detail::dependent_type<expect<E_...>>::error_capture leaf::capture( expect<E_...> &, error const & );

		expect( expect const & ) = delete;
		expect & operator=( expect const & ) = delete;

		std::tuple<leaf_detail::expect_slot<E>...>  s_;

		template <class F,class... T>
		std::pair<bool, typename leaf_detail::handler_wrapper<F>::return_type> check_handler_( error const & e, F && f, leaf_detail::mp_list<T...> ) const
		{
			using namespace leaf_detail;
			typedef typename handler_wrapper<F>::return_type return_type;
			if( slots_subset<decltype(s_),expect_slot<typename std::remove_cv<typename std::remove_reference<T>::type>::type>...>::have_values(s_,e) )
				return std::make_pair(true, handler_wrapper<F>(std::forward<F>(f))( *peek<typename std::remove_cv<typename std::remove_reference<T>::type>::type>(e)... ));
			else
				return std::make_pair(false, uhnandled_error<return_type>::value(e));
		}

		template <class F>
		std::pair<bool, typename leaf_detail::handler_wrapper<F>::return_type>  find_handler_( error const & e, F && f ) const
		{
			return check_handler_( e, std::forward<F>(f), typename leaf_detail::function_traits<F>::mp_args{ } );
		}

		template <class CarF,class... CdrF>
		std::pair<bool, typename leaf_detail::handler_wrapper<CarF>::return_type>  find_handler_( error const & e, CarF && car_f, CdrF && ... cdr_f ) const
		{
			using namespace leaf_detail;
			auto r = check_handler_( e, std::forward<CarF>(car_f), typename leaf_detail::function_traits<CarF>::mp_args{ } );
			return r.first ? r : find_handler_(e,std::forward<CdrF>(cdr_f)...);
		}

	public:

		constexpr expect() noexcept
		{
		}

		//////////////////////////////////////////////

		template <class... F>
		typename leaf_detail::handler_pack_return_type<F...>::return_type handle_error( error const & e, F && ... f ) const noexcept
		{
			return this->find_handler_( e, std::forward<F>(f)... ).second;
		}

		template <class P>
		P const * peek( error const & e ) const noexcept
		{
			auto & opt = std::get<leaf_detail::type_index<P,E...>::value>(s_);
			if( opt.has_value() )
			{
				auto & x = opt.value();
				if( x.e==e )
					return &x.v;
			}
			return 0;
		}

		//////////////////////////////////////////////

		template <class T, class... F>
		typename leaf_detail::handler_pack_return_type<F...>::return_type  handle_error( result<T> const & r, F && ... f ) const noexcept
		{
			assert(!r);
			if( r.which_ == leaf_detail::result_variant::err )
				return this->handle_error(r.err_,f...);
			else
			{
				assert(r.which_==leaf_detail::result_variant::cap);
				return r.cap_.handle_error(f...);
			}
		}

		template <class P, class T>
		P const * peek( result<T> const & r ) const noexcept
		{
			assert(!r);
			if( r.which_==leaf_detail::result_variant::err )
				return this->template peek<P>(r.err_);
			else
			{
				assert(r.which_==leaf_detail::result_variant::cap);
				return r.cap_.template peek<P>();
			}
		}

		//////////////////////////////////////////////

		template <class P>
		P const * peek( std::exception const & ) const noexcept;

		template <class... F>
		void handle_exception( std::exception const &, F && ... ) const;
	};

	////////////////////////////////////////

	template <class... E>
	typename leaf_detail::dependent_type<expect<E...>>::error_capture capture( expect<E...> & exp, error const & e )
	{
		using namespace leaf_detail;
		typename leaf_detail::dependent_type<expect<E...>>::error_capture cap(
			e,
			std::make_tuple(
				convert_optional(
					std::move(std::get<tuple_type_index<expect_slot<E>,decltype(exp.s_)>::value>(exp.s_)),e)...));
		return cap;
	}

} }

#endif
