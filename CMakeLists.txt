cmake_minimum_required(VERSION 3.13)
project(deepin-diskmanager)

find_package(Threads REQUIRED)

option(DOTEST "option for test" OFF)

# 是否开启单元测试编译
set(DOTEST OFF)

option(DOAPP "option for test app" OFF)
set(DOAPP ON)

#compile flags
if (CMAKE_BUILD_TYPE MATCHES Debug)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wextra")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wextra")
  # Enable Qt builtin debug mode
  add_definitions("-DQT_MESSAGELOGCONTEXT")
else()
  # -Wl, -O2 Enable linker optimizations
  # -Wl, --gc-sections Remove unused code resulting from -fdsta-sections and
  # -ffunction-sections
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O2")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
endif()

# 添加位置无关代码和安全编译标志
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstack-protector-all -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector-all -fPIE")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z relro -z now -z noexecstack -pie")

# Auto-detect Qt version (tries Qt6 first, falls back to Qt5)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
message(STATUS "Found Qt version: ${QT_VERSION_MAJOR}")

# Map to DTK version (Qt6→DTK6, Qt5→DTK5)
if (QT_VERSION_MAJOR MATCHES 6)
    set(DTK_VERSION_MAJOR 6)
else()
    set(DTK_VERSION_MAJOR "")
endif()
message(STATUS "Build with DTK: ${DTK_VERSION_MAJOR}")

# For Qt5, add DBUS caller auth check
if (QT_VERSION_MAJOR MATCHES 5)
    add_compile_definitions(NO_DBUS_CALLER_AUTH_CHECK)
    message("NO_DBUS_CALLER_AUTH_CHECK is on.")
endif()

# 检查内存泄漏代码
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fsanitize=address -O2")
#set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address -O2")
#set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g -fsanitize=address -O2")
#set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} -g -fsanitize=address -O2")
#set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -g -fsanitize=address -O2")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=address -O2")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -O2")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -fsanitize=address -O2")
#set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -g -fsanitize=address -O2")
#set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}  -g -fsanitize=address -O2")

# Skip build rpath for reproducible release build
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_SKIP_BUILD_RPATH TRUE)
endif()

ADD_SUBDIRECTORY(basestruct)
ADD_SUBDIRECTORY(log)
ADD_SUBDIRECTORY(application)
ADD_SUBDIRECTORY(service)

# 开启单元测试
if(DOTEST)
    if(DOAPP)
        # 添加gocv覆盖率文件的输出
#        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fprofile-arcs -ftest-coverage")
        # 设置测试模块
        set(QT Core Gui Widgets Network DBus Sql LinguistTools Svg Test)
        find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT} REQUIRED)
        # 添加子模块apptest
        add_subdirectory(tests)
    else()
    # 添加gocv覆盖率文件的输出
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fprofile-arcs -ftest-coverage")
    # 设置测试模块
    set(QT Core Gui Widgets Network DBus Sql LinguistTools Svg Multimedia Test)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${QT} REQUIRED)
    # 添加子模块test
    add_subdirectory(test)
    endif()
endif()

