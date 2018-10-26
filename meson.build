# Copyright (c) 2018 Emil Dotchevski
# Copyright (c) 2018 Second Spectrum, Inc.

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
			language:'cpp' )
	endif
endif

includes = [ include_directories('include') ]

leaf = declare_dependency( include_directories: includes )

tests = [
	'_hpp_all_test',
	'_hpp_common_test',
	'_hpp_error_capture_test',
	'_hpp_error_test',
	'_hpp_exception_capture_test',
	'_hpp_exception_test',
	'_hpp_expect_test',
	'_hpp_result_test',
	'basic_test',
	'diagnostic_print_test',
	'error_capture_test',
	'error_test',
	'exception_capture_test',
	'exception_test',
	'multiple_errors_test',
	'optional_test',
	'preload_test',
	'print_test',
	'result_capture_test',
	'result_test',
	'result_void_capture_test'
]
foreach t : tests
	test(t, executable(t, 'test/'+t+'.cpp', dependencies: [ leaf ] ) )
endforeach

examples = [
	'capture_result',
	'capture_eh',
	'print_file_result',
	'print_file_eh'
]
foreach e : examples
	executable(e, 'example/'+e+'.cpp', dependencies: [ leaf ] )
endforeach
