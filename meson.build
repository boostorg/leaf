# Copyright (c) 2018-2019 Emil Dotchevski
# Copyright (c) 2018-2019 Second Spectrum, Inc.

# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

project('leaf', 'cpp', default_options : ['cpp_std=c++11','b_pch=false'], license : 'boost')

compiler = meson.get_compiler('cpp')
compiler_id = compiler.get_id()
if not meson.is_subproject()
	if compiler_id=='clang'
		add_global_arguments(
			'-Wno-unused-variable',
			'-Wno-non-virtual-dtor',
			'-Wno-dangling-else',
			'-Wno-delete-non-virtual-dtor',
			language:'cpp' )
	endif
endif

thread_dep = dependency('threads')

includes = [ include_directories('include') ]

leaf = declare_dependency( include_directories: includes )

tests = [
	'_hpp_all_test',
	'_hpp_capture_exception_test',
	'_hpp_capture_result_test',
	'_hpp_common_test',
	'_hpp_error_test',
	'_hpp_exception_test',
	'_hpp_exception_to_result_test',
	'_hpp_handle_test',
	'_hpp_preload_test',
	'_hpp_result_test',
	'_hpp_try_test',
	'capture_exception_async_test',
	'capture_exception_state_test',
	'capture_exception_unload_test',
	'capture_result_async_test',
	'capture_result_state_test',
	'capture_result_unload_test',
	'defer_basic_test',
	'defer_nested_error_exception_test',
	'defer_nested_error_result_test',
	'defer_nested_new_error_exception_test',
	'defer_nested_new_error_result_test',
	'defer_nested_success_exception_test',
	'defer_nested_success_result_test',
	'diagnostic_info_test',
	'exception_to_result_test',
	'function_traits_test',
	'handle_all_other_result_test',
	'handle_all_test',
	'handle_basic_test',
	'handle_some_other_result_test',
	'handle_some_test',
	'is_error_type_test',
	'multiple_errors_test',
	'optional_test',
	'preload_basic_test',
	'preload_nested_error_exception_test',
	'preload_nested_error_result_test',
	'preload_nested_new_error_exception_test',
	'preload_nested_new_error_result_test',
	'preload_nested_success_exception_test',
	'preload_nested_success_result_test',
	'print_test',
	'result_bad_result_test',
	'result_state_test',
	'static_store_deduction_test',
	'try_exception_and_result_test',
	'try_exception_test',
	'try_test'
]
foreach t : tests
	test(t, executable(t, 'test/'+t+'.cpp', dependencies: [leaf,thread_dep] ) )
endforeach

examples = [
	'capture_result',
	'capture_eh',
	'exception_to_result',
	'print_file_result',
	'print_file_eh',
	'print_half'
]
foreach e : examples
	executable(e, 'example/'+e+'.cpp', dependencies: [leaf,thread_dep] )
endforeach

if get_option('build_lua_examples')
	lua=subproject('lua').get_variable('all')
	executable('lua_callback_result', 'example/lua_callback_result.cpp', dependencies: [ leaf, lua ] )
	executable('lua_callback_eh', 'example/lua_callback_eh.cpp', dependencies: [ leaf, lua ] )
endif
