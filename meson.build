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
	'tl_slot_test',
	'diagnostic_print_test',
	'diagnostic_information_test',
	'expect_test',
	'transport_test',
	'hpp_capture_test',
	'hpp_common_test',
	'hpp_current_exception_diagnostic_information_test',
	'hpp_diagnostic_information_test',
	'hpp_expect_test',
	'hpp_has_current_error_test',
	'hpp_put_test',
	'hpp_transport_test'
]
foreach t : tests
	test(t, executable(t, 'test/'+t+'.cpp', dependencies: [ leaf ] ) )
endforeach

examples = [
	'print_file_ec',
	'print_file_eh',
	'transport_ec',
	'transport_eh'
]
foreach e : examples
	executable(e, 'example/'+e+'.cpp', dependencies: [ leaf ] )
endforeach
