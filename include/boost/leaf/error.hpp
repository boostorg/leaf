#ifndef BOOST_LEAF_BA049396D0D411E8B45DF7D4A759E189
#define BOOST_LEAF_BA049396D0D411E8B45DF7D4A759E189

// Copyright (c) 2018 Emil Dotchevski
// Copyright (c) 2018 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/teleport.hpp>
#include <system_error>

#define LEAF_ERROR(...) ::boost::leaf::leaf_detail::new_error_at(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)

namespace boost { namespace leaf {

	namespace leaf_detail
	{
		template <>
		struct is_error_type_default<std::error_code>: std::true_type
		{
		};

		class error_category: public std::error_category
		{
			bool equivalent( int,  std::error_condition const & ) const noexcept
			{
				return false;
			}

			bool equivalent( std::error_code const &, int ) const noexcept
			{
				return false;
			}

			char const * name() const noexcept
			{
				return "LEAF error, use with leaf::handle_some or leaf::handle_all.";
			}

			std::string message( int condition ) const
			{
				return name();
			}
		};

		inline error_category const & get_error_category() noexcept
		{
			static error_category cat;
			return cat;
		}
	}

	inline bool is_error_id( std::error_code const & ec )
	{
		return &ec.category() == &leaf_detail::get_error_category();
	}

	////////////////////////////////////////

	struct e_original_ec { std::error_code value; };

	class error_id: public std::error_code
	{
		template <class ErrorCode>
		static std::error_code import( ErrorCode && ec ) noexcept
		{
			std::error_category const & cat = leaf_detail::get_error_category();
			if( ec && &ec.category()!=&cat )
			{
				int err_id = leaf_detail::new_id();
				leaf_detail::put_slot(err_id, e_original_ec{ec});
				return std::error_code(err_id, cat);
			}
			else
				return ec;
		}

	public:

		error_id() noexcept = default;

		template <class... E>
		error_id( std::error_code const & ec, E && ... e ) noexcept:
			error_code(import(ec))
		{
			(void) propagate(std::forward<E>(e)...);
		}

		template <class... E>
		error_id( std::error_code && ec, E && ... e ) noexcept:
			error_code(import(std::move(ec)))
		{
			(void) propagate(std::forward<E>(e)...);
		}

		error_id propagate() const noexcept
		{
			assert(is_error_id(*this));
			return *this;
		}

		template <class... E>
		error_id propagate( E && ... e ) const noexcept
		{
			assert(is_error_id(*this));
			auto _ = { leaf_detail::put_slot(value(), std::forward<E>(e))... };
			(void) _;
			return *this;
		}
	};

	namespace leaf_detail
	{
		inline error_id make_error( int err_id )
		{
			assert(err_id);
			return error_id(std::error_code(err_id, get_error_category()));
		}
	}

	template <class... E>
	error_id new_error( E && ... e ) noexcept
	{
		return leaf_detail::make_error(leaf_detail::new_id()).propagate(std::forward<E>(e)...);
	}

	inline error_id next_error() noexcept
	{
		return leaf_detail::make_error(leaf_detail::next_id());
	}

	inline error_id error_info::error() const noexcept
	{
		assert(has_error());
		return leaf_detail::make_error(err_id_);
	}

	////////////////////////////////////////

	struct e_source_location
	{
		char const * const file;
		int const line;
		char const * const function;

		friend std::ostream & operator<<( std::ostream & os, e_source_location const & x )
		{
			return os << leaf::type<e_source_location>() << ": " << x.file << '(' << x.line << ") in function " << x.function;
		}
	};

	namespace leaf_detail
	{
		template <> struct is_error_type_default<e_source_location>: std::true_type { };

		template <class... E>
		error_id new_error_at( char const * file, int line, char const * function, E && ... e ) noexcept
		{
			assert(file&&*file);
			assert(line>0);
			assert(function&&*function);
			e_source_location sl { file, line, function }; // Temp object MSVC workaround
			return new_error( std::move(sl), std::forward<E>(e)... );
		}
	}

} }

#endif
