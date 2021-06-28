#!/bin/sh

mkdir -p build
cd build

cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
make

# Run macdeployqt
bundlename='Morphologica.app'
macdeployqt "$bundlename" -verbose=2 -dmg
