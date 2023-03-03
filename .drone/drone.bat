@REM Copyright 2022 Peter Dimov
@REM Distributed under the Boost Software License, Version 1.0.
@REM https://www.boost.org/LICENSE_1_0.txt

@ECHO ON

set LIBRARY=%1
set DRONE_BUILD_DIR=%CD%

set BOOST_BRANCH=develop
if "%DRONE_BRANCH%" == "master" set BOOST_BRANCH=master
cd ..
git clone -b %BOOST_BRANCH% --depth 1 https://github.com/boostorg/boost.git boost-root
cd boost-root
git submodule update --init tools/boostdep
xcopy /s /e /q %DRONE_BUILD_DIR% libs\%LIBRARY%\
python tools/boostdep/depinst/depinst.py %LIBRARY%
cmd /c bootstrap
b2 -d0 headers

echo "Generating single header"
cd libs/%LIBRARY%
python gen/generate_single_header.py -i include/boost/leaf/detail/all.hpp -p include -o test/leaf.hpp boost/leaf

echo "Testing"
if not "%CXXSTD%" == "" set CXXSTD=cxxstd=%CXXSTD%
if not "%ADDRMD%" == "" set ADDRMD=address-model=%ADDRMD%
..\..\b2 -j3 test toolset=%TOOLSET% %CXXSTD% %ADDRMD% embed-manifest-via=linker link=shared,static variant=debug,release,leaf_debug_capture0,leaf_release_capture0,leaf_debug_diag0,leaf_release_diag0,leaf_debug_embedded,leaf_release_embedded,leaf_debug_leaf_hpp,leaf_release_leaf_hpp
