#!/bin/sh

mkdir -p build
cd build

cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
make

# Run macdeployqt
bundlename='Morphologica.app'
rm -rf "Morphologica.dmg"
macdeployqt "$bundlename" -verbose=2

# Fix it with macdeployqtfix

if [ ! -d macdeployqtfix ]; then
    git clone https://github.com/iltommi/macdeployqtfix.git
fi

# Needs qt 6
echo "$Qt6_DIR qt6 dir, $Qt5_DIR qt5 dir"
env
python3 macdeployqtfix/macdeployqtfix.py "$bundlename/Contents/MacOS/Morphologica" /usr/local/Cellar/qt/6.1.0_1
