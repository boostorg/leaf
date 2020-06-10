#ifndef LEAF_75F38740D98D11E881DDB244C82C3C47
#define LEAF_75F38740D98D11E881DDB244C82C3C47

// Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if defined(__clang__)
#	pragma clang system_header
#elif (__GNUC__*100+__GNUC_MINOR__>301) && !defined(LEAF_ENABLE_WARNINGS)
#	pragma GCC system_header
#elif defined(_MSC_VER) && !defined(LEAF_ENABLE_WARNINGS)
#	pragma warning(push,1)
#endif

#include <boost/leaf/error.hpp>
#include <exception>

#define LEAF_EXCEPTION(...) ::boost::leaf::exception(__VA_ARGS__).at(__FILE__,__LINE__,__FUNCTION__)
#define LEAF_THROW(...) ::boost::leaf::throw_exception(LEAF_EXCEPTION(__VA_ARGS__))

#ifdef LEAF_NO_EXCEPTIONS

namespace boost
{
	[[noreturn]] void throw_exception( std::exception const & ); // user defined
}

namespace boost { namespace leaf {

	template <class T>
	[[noreturn]] void throw_exception( T const & e )
	{
		::boost::throw_exception(e);
	}

} }

#else

namespace boost { namespace leaf {

	template <class T>
	[[noreturn]] void throw_exception( T const & e )
	{
		throw e;
	}

} }

#endif

////////////////////////////////////////

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		inline void enforce_std_exception( std::exception const & ) noexcept { }

		class exception_base
		{
			std::shared_ptr<void const> auto_id_bump_;
		public:

			virtual error_id get_error_id() const noexcept = 0;

		protected:

			exception_base():
				auto_id_bump_(0, [](void const *) { (void) new_id(); })
			{
			}

			~exception_base() noexcept { }
		};

		template <class Ex>
		class exception:
			public Ex,
			public exception_base,
			public error_id
		{
			error_id get_error_id() const noexcept final override
			{
				return *this;
			}

		public:

			exception( exception const & ) = default;
			exception( exception && ) = default;

			LEAF_CONSTEXPR exception( error_id id, Ex && ex ) noexcept:
				Ex(std::move(ex)),
				error_id(id)
			{
				leaf_detail::enforce_std_exception(*this);
			}

			explicit LEAF_CONSTEXPR exception( error_id id ) noexcept:
				error_id(id)
			{
				leaf_detail::enforce_std_exception(*this);
			}

			LEAF_CONSTEXPR exception & at( char const * file, int line, char const * function ) noexcept
			{
				BOOST_LEAF_ASSERT(file&&*file);
				BOOST_LEAF_ASSERT(line>0);
				BOOST_LEAF_ASSERT(function&&*function);
				this->load(e_source_location {file,line,function});
				return *this;
			}
		};
	}

	template <class... Tag, class Ex, class... E>
	LEAF_CONSTEXPR inline typename std::enable_if<std::is_base_of<std::exception,Ex>::value, leaf_detail::exception<Ex>>::type exception( Ex && ex, E && ... e ) noexcept
	{
		auto id = leaf::new_error<Tag...>(std::forward<E>(e)...);
		return leaf_detail::exception<Ex>(id, std::forward<Ex>(ex));
	}

	template <class... Tag, class E1, class... E>
	LEAF_CONSTEXPR inline typename std::enable_if<!std::is_base_of<std::exception,E1>::value, leaf_detail::exception<std::exception>>::type exception( E1 && car, E && ... cdr ) noexcept
	{
		auto id = leaf::new_error<Tag...>(std::forward<E1>(car), std::forward<E>(cdr)...);
		return leaf_detail::exception<std::exception>(id);
	}

	inline leaf_detail::exception<std::exception> exception() noexcept
	{
		return leaf_detail::exception<std::exception>(leaf::new_error());
	}

	template <class... Tag, class Ex, class... E>
	LEAF_CONSTEXPR inline typename std::enable_if<std::is_base_of<std::exception,Ex>::value, leaf_detail::exception<Ex>>::type exception( error_id id, Ex && ex, E && ... e ) noexcept
	{
		return leaf_detail::exception<Ex>(id.load<Tag...>(std::forward<E>(e)...), std::forward<Ex>(ex));
	}

	template <class... Tag, class E1, class... E>
	LEAF_CONSTEXPR inline typename std::enable_if<!std::is_base_of<std::exception,E1>::value, leaf_detail::exception<std::exception>>::type exception( error_id id, E1 && car, E && ... cdr ) noexcept
	{
		return leaf_detail::exception<std::exception>(id.load<Tag...>(std::forward<E1>(car), std::forward<E>(cdr)...));
	}

	inline leaf_detail::exception<std::exception> exception(error_id id) noexcept
	{
		return leaf_detail::exception<std::exception>(id);
	}

} }

#endif
