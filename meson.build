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
	'optional_test',
	'tl_slot_test',
	'diagnostic_print_test',
	'diagnostic_information_test',
	'expect_result_test',
	'expect_eh_test',
	'capture_result_test',
	'capture_result_void_test',
	'capture_eh_test',
	'hpp_capture_exception_test',
	'hpp_capture_result_test',
	'hpp_common_test',
	'hpp_current_exception_diagnostic_information_test',
	'hpp_diagnostic_information_test',
	'hpp_expect_test',
	'hpp_put_test'
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
