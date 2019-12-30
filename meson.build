# Copyright (c) 2018-2019 Emil Dotchevski and Reverge Studios, Inc.

# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

project('leaf', 'cpp', default_options : ['cpp_std=c++17','b_pch=false'], license : 'boost')

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
	elif compiler_id=='gcc'
		add_global_arguments(
			'-Wno-non-virtual-dtor',
			'-Wno-dangling-else',
			'-Wno-parentheses',
			language:'cpp' )
	endif
endif

if get_option('leaf_all_hpp')
	prog_python = import('python').find_installation('python3')
	custom_target(
		'all_hpp',
		command:
		[
			prog_python,
			files('gen/generate_single_header.py'),
			'-i', files('include/boost/leaf/detail/all.hpp'),
			'-p', join_paths(meson.source_root(),'include'),
			'-o', join_paths(meson.source_root(),'include/boost/leaf/all.hpp'),
			'boost/leaf'
		],
		output: 'all.hpp',
		build_always_stale: true,
		build_by_default: true)
endif

boost_headers = declare_dependency(include_directories: '../..')

exceptions = (get_option('cpp_eh')!='none')
diagnostics = get_option('leaf_diagnostics')

defines = [ '-DLEAF_DIAGNOSTICS='+diagnostics.to_string() ]

mp11 = [ ]
if get_option('leaf_use_boost_mp11')
	mp11 = [ boost_headers ]
	defines += [ '-DLEAF_USE_BOOST_MP11' ]
endif

thread_dep = dependency('threads')

includes = [ include_directories('include') ]

leaf = declare_dependency( include_directories: includes, compile_args: defines )

tests = [
	'accumulate_basic_test',
	'accumulate_nested_error_exception_test',
	'accumulate_nested_error_result_test',
	'accumulate_nested_new_error_exception_test',
	'accumulate_nested_new_error_result_test',
	'accumulate_nested_success_exception_test',
	'accumulate_nested_success_result_test',
	'capture_exception_async_test',
	'capture_exception_result_async_test',
	'capture_exception_state_test',
	'capture_exception_unload_test',
	'capture_result_async_test',
	'capture_result_state_test',
	'context_activator_test',
	'context_deduction_test',
	'capture_result_unload_test',
	'ctx_handle_all_test',
	'ctx_handle_exception_test',
	'ctx_handle_some_test',
	'ctx_remote_handle_all_test',
	'ctx_remote_handle_exception_test',
	'ctx_remote_handle_some_test',
	'defer_basic_test',
	'defer_nested_error_exception_test',
	'defer_nested_error_result_test',
	'defer_nested_new_error_exception_test',
	'defer_nested_new_error_result_test',
	'defer_nested_success_exception_test',
	'defer_nested_success_result_test',
	'diagnostic_info_test',
	'error_code_test',
	'error_id_test',
	'exception_test',
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
	'result_load_accumulate_test',
	'result_state_test',
	'try_catch_error_id_test',
	'try_catch_test',
	'try_exception_and_result_test',
	'_hpp_capture_test',
	'_hpp_common_test',
	'_hpp_context_test',
	'_hpp_error_test',
	'_hpp_exception_test',
	'_hpp_handle_error_test',
	'_hpp_handle_exception_test',
	'_hpp_preload_test',
	'_hpp_result_test',
	'_hpp_all_test',
]
ubsan_args = [ '-fsanitize=undefined', '-fno-sanitize-recover=undefined' ]
foreach t : tests
	test(t, executable(t, 'test/'+t+'.cpp', dependencies: [leaf,thread_dep]+mp11, cpp_args: ubsan_args, link_args: ubsan_args) )
endforeach

if exceptions
	examples = [
		'capture_in_exception',
		'capture_in_result',
		'error_log',
		'error_trace',
		'exception_to_result',
		'print_file_eh',
		'print_file_result',
		'print_half'
	]
	foreach e : examples
		executable(e, 'examples/'+e+'.cpp', dependencies: [leaf,thread_dep]+mp11 )
	endforeach

	if get_option('leaf_boost_examples')
		executable('asio_beast_leaf_rpc', 'examples/asio_beast_leaf_rpc.cpp', dependencies: [leaf,thread_dep,boost_headers], override_options: ['cpp_std=c++17'] )
		executable('print_file_outcome_result', 'examples/print_file_outcome_result.cpp', dependencies: [leaf,boost_headers], override_options: ['cpp_std=c++17'] )
	endif

	if get_option('leaf_lua_examples')
		lua=subproject('lua').get_variable('all')
		executable('lua_callback_result', 'examples/lua_callback_result.cpp', dependencies: [leaf,lua]+mp11 )
		executable('lua_callback_eh', 'examples/lua_callback_eh.cpp', dependencies: [leaf,lua]+mp11 )
	endif
endif

if not exceptions
	if diagnostics == 0
		executable('deep_stack_leaf', 'benchmark/deep_stack_leaf.cpp', dependencies: [leaf], override_options: ['cpp_std=c++17'])
	endif
	if get_option('leaf_boost_examples')
		executable('deep_stack_tl', 'benchmark/deep_stack_other.cpp', override_options: ['cpp_std=c++17'], cpp_args: '-DBENCHMARK_WHAT=0' )
		executable('deep_stack_result', 'benchmark/deep_stack_other.cpp', dependencies: [boost_headers], override_options: ['cpp_std=c++17'], cpp_args: '-DBENCHMARK_WHAT=1' )
		executable('deep_stack_outcome', 'benchmark/deep_stack_other.cpp', dependencies: [boost_headers], override_options: ['cpp_std=c++17'], cpp_args: '-DBENCHMARK_WHAT=2' )
	endif
endif
