#!/bin/bash

# Copyright 2022 Peter Dimov
# Distributed under the Boost Software License, Version 1.0.
# https://www.boost.org/LICENSE_1_0.txt

set -ex

DRONE_BUILD_DIR=$(pwd)
export PATH=~/.local/bin:/usr/local/bin:$PATH

BOOST_BRANCH=develop
if [ "$DRONE_BRANCH" = "master" ]; then BOOST_BRANCH=master; fi

cd ..
git clone -b $BOOST_BRANCH --depth 1 https://github.com/boostorg/boost.git boost-root
cd boost-root
git submodule update --init tools/boostdep
cp -r $DRONE_BUILD_DIR/* libs/$LIBRARY
python tools/boostdep/depinst/depinst.py $LIBRARY
./bootstrap.sh
./b2 -d0 headers

echo "Generating single header"
cd libs/$LIBRARY
python gen/generate_single_header.py -i include/boost/leaf/detail/all.hpp -p include -o test/leaf.hpp boost/leaf

echo "Testing"
echo "using $TOOLSET : : $COMPILER ;" > ~/user-config.jam
../../b2 -j3 test $LINKFLAGS toolset=$TOOLSET cxxstd=$CXXSTD ${ADDRMD:+address-model=$ADDRMD} ${UBSAN:+undefined-sanitizer=norecover debug-symbols=on} ${ASAN:+address-sanitizer=norecover debug-symbols=on} ${LINKFLAGS:+linkflags=$LINKFLAGS} link=shared,static variant=debug,release,leaf_debug_capture0,leaf_release_capture0,leaf_debug_diag0,leaf_release_diag0,leaf_debug_embedded,leaf_release_embedded,leaf_debug_leaf_hpp,leaf_release_leaf_hpp
