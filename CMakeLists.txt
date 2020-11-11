cmake_minimum_required(VERSION 3.0)
project(deepin-diskmanager)


option(DOTEST "option for test" OFF)

# 是否开启单元测试编译
set(DOTEST OFF)

option(DOAPP "option for test app" OFF)
set(DOAPP OFF)

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
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -O2 -Wl,--gc-sections")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2 -Wl,--gc-sections")
endif()
ADD_SUBDIRECTORY(authenticateProxy)
ADD_SUBDIRECTORY(basestruct)
ADD_SUBDIRECTORY(log)
ADD_SUBDIRECTORY(application)
ADD_SUBDIRECTORY(service)

# 开启单元测试
if(DOTEST)
    if(DOAPP)
        # 添加gocv覆盖率文件的输出
        set(CMAKE_CXX_FLAGS "-g -fprofile-arcs -ftest-coverage")
        # 设置测试模块
        set(QT Core Gui Widgets Network DBus Sql LinguistTools Svg Multimedia Test)
        find_package(Qt5 COMPONENTS ${QT} REQUIRED)
        # 添加子模块apptest
        add_subdirectory(apptest)
    else()
    # 添加gocv覆盖率文件的输出
    set(CMAKE_CXX_FLAGS "-g -fprofile-arcs -ftest-coverage")
    # 设置测试模块
    set(QT Core Gui Widgets Network DBus Sql LinguistTools Svg Multimedia Test)
    find_package(Qt5 COMPONENTS ${QT} REQUIRED)
    # 添加子模块test
    add_subdirectory(test)
    endif()
endif()

