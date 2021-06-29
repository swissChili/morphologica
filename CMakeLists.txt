cmake_minimum_required(VERSION 3.5)

project(Morphologica LANGUAGES CXX)

set(PROJECT_COMPANY "swissChili")
set(PROJECT_COPYRIGHT "Copyright (c) 2021 swissChili. All rights reserved.")
set(PROJECT_DOMAIN "sh.swisschili.morphologica")
set(PROJECT_VERSION 1.0)
set(PROjECT_BUNDLE_NAME Morphologica)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(APP_ICON_MACOSX ${CMAKE_CURRENT_SOURCE_DIR}/res/AppIcon.icns)

set_source_files_properties(
    ${APP_ICON_MACOSX}
    PROPERTIES MACOSX_PACKAGE_LOCATION "Resources"
)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

add_custom_target(MacOS SOURCES cmake/MacOSXBundleInfo.plist.in build_macosx.sh)
add_custom_target(CI SOURCES .github/workflows/build-macos.yml)

set(OS_BUNDLE "")
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(OS_BUNDLE "MACOSX_BUNDLE")
    set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_BUNDLE_NAME})
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_COPYRIGHT ${PROJECT_COPYRIGHT})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER ${PROJECT_DOMAIN})
    set(MACOSX_BUNDLE_ICON_FILE AppIcon.icns)
else(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_BUILD_TYPE Debug)
endif()

add_executable(Morphologica
    ${OS_BUNDLE}
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    dataentry.cpp
    dataentry.h
    graphicsview.cpp
    graphicsview.h
    dataentrylistmodel.cpp
    dataentrylistmodel.h
    res/resources.qrc
    ${APP_ICON_MACOSX}
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_target_properties(Morphologica PROPERTIES
        MACOSX_BUNDLE TRUE
        OUTPUT_NAME "${PROJECT_BUNDLE_NAME}"
    )
endif()

target_link_libraries(Morphologica PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
