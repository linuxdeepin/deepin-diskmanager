# SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 3.0)
project(basestruct)

# Find the library
find_package(PkgConfig REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS
    Core
    Widgets
    DBus
REQUIRED)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

file(GLOB BASESTRUCT_SOURCES "*.cpp")
file(GLOB BASESTRUCT_HEADERS "*.h")

set(BASESTRUCT_SRCS ${BASESTRUCT_SOURCES} ${BASESTRUCT_HEADERS} )

add_library(${PROJECT_NAME} STATIC ${BASESTRUCT_SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC
    ${Qt6Gui_PRIVATE_INCLUDE_DIRS}
    ${PROJECT_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::DBus
    Threads::Threads
)

# not install for internal use

