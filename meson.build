# Copyright 2018-2025 Emil Dotchevski and Reverge Studios, Inc.

# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

project('leaf', 'cpp', default_options : ['cpp_std=c++17', 'b_pch=false'], license : 'boost')

option_single_header = get_option('leaf_single_header')
option_boost_available = get_option('leaf_boost_available')
option_diagnostics = get_option('leaf_diagnostics')
option_capture = get_option('leaf_capture')
option_exceptions = (get_option('cpp_eh')!='none')
option_enable_unit_tests = get_option('leaf_enable_unit_tests')
option_enable_examples = get_option('leaf_enable_examples')
option_embedded = get_option('leaf_embedded')

if target_machine.system() == 'windows'
    add_global_arguments('-D_CRT_SECURE_NO_WARNINGS', language: 'cpp')
endif

compiler = meson.get_compiler('cpp')
compiler_id = compiler.get_id()
if not meson.is_subproject()
    if option_boost_available
        add_global_arguments(
            '-DBOOST_LEAF_BOOST_AVAILABLE',
            language:'cpp' )
    endif
    if compiler_id=='msvc'
        add_global_arguments(
            '/FC',
            language:'cpp' )
    elif compiler_id=='clang'
        if get_option('buildtype')!='debug'
            add_global_arguments(
                '-Wno-unused-variable',
                language:'cpp' )
        endif
        add_global_arguments(
            '-fdiagnostics-absolute-paths',
            '-Wno-dangling-else',
            '-Wno-non-virtual-dtor',
            '-Wno-delete-non-abstract-non-virtual-dtor',
#            '-Wshadow-all',
#            '-Wsign-conversion',
#            '-Wextra',
#            '-Werror',
            language:'cpp' )
    elif compiler_id=='gcc'
        if get_option('buildtype')!='debug'
            add_global_arguments(
                '-Wno-unused-variable',
                language:'cpp' )
        endif
        add_global_arguments(
            '-Wno-dangling-else',
            '-Wno-non-virtual-dtor',
            '-Wno-misleading-indentation',
            language:'cpp' )
    elif host_machine.system()=='emscripten'
        add_global_arguments(
            '-s', 'WASM=1',
            '-s', 'USE_PTHREADS=1',
            '-s', 'EXIT_RUNTIME=1',
            '-s', 'PROXY_TO_PTHREAD=1',
            '-s', 'DISABLE_EXCEPTION_CATCHING=0',
                language:'cpp' )
        add_global_link_arguments(
            '-s', 'EXPORT_ALL=1',
            '-s', 'WASM=1',
            '-s', 'USE_PTHREADS=1',
            '-s', 'EXIT_RUNTIME=1',
            '-s', 'PROXY_TO_PTHREAD=1',
            '-s', 'DISABLE_EXCEPTION_CATCHING=0',
            '-s', 'INITIAL_MEMORY=268435456',
                language:'cpp' )
    endif
endif

dep_boost = [ ]
if option_boost_available # Requires that LEAF resides under boost_root/libs/leaf.
    dep_boost = declare_dependency(include_directories: '../..')
endif

dep_lua = [ ]
if option_enable_examples
    dep_lua = subproject('lua').get_variable('all')
endif

defines = [
    '-DBOOST_LEAF_CFG_DIAGNOSTICS=' + option_diagnostics.to_string(),
    '-DBOOST_LEAF_CFG_CAPTURE=' + option_capture.to_string()
]

if option_embedded
    defines += '-DBOOST_LEAF_EMBEDDED'
endif

dep_thread = dependency('threads')

leaf = declare_dependency( include_directories: 'include', compile_args: defines )

#################################

if option_enable_unit_tests

    tests = [
        'BOOST_LEAF_ASSIGN_test',
        'BOOST_LEAF_AUTO_test',
        'BOOST_LEAF_CHECK_test',
        'capture_exception_async_test',
        'capture_exception_result_async_test',
        'capture_exception_result_unload_test',
        'capture_exception_state_test',
        'capture_exception_unload_test',
        'capture_result_async_test',
        'capture_result_state_test',
        'capture_result_unload_test',
        'context_activator_test',
        'context_deduction_test',
        'ctx_handle_all_test',
        'ctx_handle_some_test',
        'ctx_remote_handle_all_test',
        'ctx_remote_handle_some_test',
        'diagnostics_test1',
        'diagnostics_test2',
        'diagnostics_test3',
        'diagnostics_test4',
        'diagnostics_test5',
        'diagnostics_test6',
        'e_errno_test',
        'error_code_test',
        'error_id_test',
        'exception_test',
        'exception_to_result_test',
        'function_traits_test',
        'github_issue53_test',
        'github_issue53x_test',
        'handle_all_other_result_test',
        'handle_all_test',
        'handle_basic_test',
        'handle_some_other_result_test',
        'handle_some_test',
        'match_member_test',
        'match_test',
        'match_value_test',
        'multiple_errors_test',
        'nlohmann_json_encoder_test',
        'on_error_accumulate_basic_test',
        'on_error_accumulate_nested_error_exception_test',
        'on_error_accumulate_nested_error_result_test',
        'on_error_accumulate_nested_new_error_exception_test',
        'on_error_accumulate_nested_new_error_result_test',
        'on_error_accumulate_nested_success_exception_test',
        'on_error_accumulate_nested_success_result_test',
        'on_error_alloc_fail_test',
        'on_error_defer_basic_test',
        'on_error_defer_nested_error_exception_test',
        'on_error_defer_nested_error_result_test',
        'on_error_defer_nested_new_error_exception_test',
        'on_error_defer_nested_new_error_result_test',
        'on_error_defer_nested_success_exception_test',
        'on_error_defer_nested_success_result_test',
        'on_error_dynamic_reserve_test1',
        'on_error_dynamic_reserve_test2',
        'on_error_dynamic_reserve_test3',
        'on_error_preload_basic_test',
        'on_error_preload_exception_test',
        'on_error_preload_nested_error_exception_test',
        'on_error_preload_nested_error_result_test',
        'on_error_preload_nested_new_error_exception_test',
        'on_error_preload_nested_new_error_result_test',
        'on_error_preload_nested_success_exception_test',
        'on_error_preload_nested_success_result_test',
        'optional_test',
        'print_test',
        'result_bad_result_test',
        'result_implicit_conversion_test',
        'result_load_test',
        'result_print_test',
        'result_ref_test',
        'result_state_test',
        'tls_array_alloc_test1',
        'tls_array_alloc_test2',
        'tls_array_alloc_test3',
        'tls_array_test',
        'to_variant_test',
        'try_capture_all_test',
        'try_catch_error_id_test',
        'try_catch_system_error_test',
        'try_catch_test',
        'try_exception_and_result_test',
        'type_name_test',
    ]
    if option_boost_available and option_exceptions
        tests += [
            'boost_exception_test'
        ]
    endif

    dep_test_single_header = []
    if option_single_header
        dep_test_single_header = declare_dependency(compile_args: ['-DBOOST_LEAF_TEST_SINGLE_HEADER'])
    endif

    foreach t : tests
        test(t, executable(t, 'test/'+t+'.cpp', dependencies: [leaf, dep_thread, dep_boost, dep_test_single_header]) )
    endforeach

    if target_machine.system() == 'windows'
        dep_e_LastError = declare_dependency(compile_args: ['-DBOOST_LEAF_CFG_WIN32=1'])
        test('e_LastError_test', executable('e_LastError_test', 'test/e_LastError_test.cpp', dependencies: [leaf, dep_thread, dep_boost, dep_test_single_header, dep_e_LastError]) )
    endif

    header_tests = [
        '_hpp_common_test',
        '_hpp_config_test',
        '_hpp_context_test',
        '_hpp_diagnostics_test',
        '_hpp_error_test',
        '_hpp_exception_test',
        '_hpp_handle_errors_test',
        '_hpp_leaf_test',
        '_hpp_on_error_test',
        '_hpp_pred_test',
        '_hpp_result_test',
        '_hpp_serialization_nlohmann_json_encoder_test',
        '_hpp_to_variant_test',
    ]
    foreach t : header_tests
        test(t, executable(t, 'test/'+t+'.cpp', dependencies: [leaf, dep_thread, dep_boost]) )
    endforeach

    # Shared library test
    dep_so_dll = []
    if target_machine.system() == 'windows'
        dep_so_dll = declare_dependency(compile_args: ['-DBOOST_LEAF_CFG_WIN32=2'])
    endif
    so_dll_test_lib1 = shared_library('so_dll_test_lib1', 'test/so_dll_lib1.cpp', dependencies: [leaf, dep_so_dll], gnu_symbol_visibility: 'hidden')
    so_dll_test_lib2 = shared_library('so_dll_test_lib2', 'test/so_dll_lib2.cpp', dependencies: [leaf, dep_so_dll], gnu_symbol_visibility: 'hidden')
    test('so_dll_test', executable('so_dll_test', 'test/so_dll_test.cpp', dependencies: [leaf, dep_so_dll], link_with: [so_dll_test_lib1, so_dll_test_lib2]))

    # Test that the DLL test works when linked statically as well.
    # This is to verify that we don't get multiply defined symbol link errors when tls_win32.hpp is included in multiple compilation units..
    dep_so_dll_static = declare_dependency(compile_args: ['-DBOOST_LEAF_SO_DLL_TEST_STATIC'])
    so_dll_static_lib1 = static_library('so_dll_static_lib1', 'test/so_dll_lib1.cpp', dependencies: [leaf, dep_so_dll, dep_so_dll_static])
    so_dll_static_lib2 = static_library('so_dll_static_lib2', 'test/so_dll_lib2.cpp', dependencies: [leaf, dep_so_dll, dep_so_dll_static])
    test('so_dll_static_test', executable('so_dll_static_test', 'test/so_dll_test.cpp', dependencies: [leaf, dep_so_dll, dep_so_dll_static], link_with: [so_dll_static_lib1, so_dll_static_lib2]))

endif

#################################

if option_enable_examples

    executable('print_file_leaf_result', 'example/print_file/print_file_leaf_result.cpp', dependencies: [leaf] )
    if option_exceptions
        executable('print_file_exceptions', 'example/print_file/print_file_exceptions.cpp', dependencies: [leaf] )
    endif
    if option_boost_available
        executable('print_file_system_result', 'example/print_file/print_file_system_result.cpp', dependencies: [leaf, dep_boost] )
    endif

    executable('error_log', 'example/error_log.cpp', dependencies: [leaf] )
    executable('error_trace', 'example/error_trace.cpp', dependencies: [leaf] )
    executable('print_half', 'example/print_half.cpp', dependencies: [leaf] )
    executable('try_capture_all_result', 'example/try_capture_all_result.cpp', dependencies: [leaf] )
    if option_exceptions
        executable('try_capture_all_exceptions', 'example/try_capture_all_exceptions.cpp', dependencies: [leaf] )
        executable('exception_to_result', 'example/exception_to_result.cpp', dependencies: [leaf] )
    endif

    if option_enable_examples
        if option_exceptions
            executable('lua_callback_exceptions', 'example/lua_callback_exceptions.cpp', dependencies: [leaf, dep_lua] )
        endif
        executable('lua_callback_result', 'example/lua_callback_result.cpp', dependencies: [leaf, dep_lua] )
    endif

endif
