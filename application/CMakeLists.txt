cmake_minimum_required(VERSION 3.13)
project(deepin-diskmanager)

if (NOT DEFINED VERSION)
    message(WARNING "Not defined version ,about dialog version set 1.1.0.1")
    set(VERSION 1.3.14)
endif ()


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)#moc 预编译包含QObject文件，生成xx_moc.h cpp
set(CMAKE_AUTORCC ON)#开启rcc 编译资源文件qrc
set(CMAKE_AUTOUIC ON)#开启uic 编译转换*.ui文件生存设计界面对应的.h .cpp文件

# Find the library
find_package(PkgConfig REQUIRED)
find_package(Dtk${DTK_VERSION_MAJOR}Widget REQUIRED)
find_package(Dtk${DTK_VERSION_MAJOR}Gui REQUIRED)
#find_package(DtkCMake REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS
    Core
    Widgets
    DBus
    LinguistTools
    REQUIRED)

set(LINK_LIBS
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::DBus
    Qt${QT_VERSION_MAJOR}::Widgets
    Dtk${DTK_VERSION_MAJOR}::Core
    Dtk${DTK_VERSION_MAJOR}::Gui
    Dtk${DTK_VERSION_MAJOR}::Widget
)

# Qt6 has WidgetsPrivate module, Qt5 does not (use private target differently)
if (QT_VERSION_MAJOR MATCHES 6)
    list(APPEND LINK_LIBS Qt${QT_VERSION_MAJOR}::WidgetsPrivate)
endif()

#common resource names
set(APP_RES_DIR "assets")
set(APP_DESKTOP "deepin-diskmanager.desktop")
set(APP_QRC "${APP_RES_DIR}/appicons.qrc")
set(APP_ICONPATH "${APP_RES_DIR}/icons/deepin-diskmanager.svg")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie")

configure_file(${APP_RES_DIR}/environments.h.in environments.h @ONLY)
#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/translations")
#set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1 "${CMAKE_CURRENT_SOURCE_DIR}/translations")
#set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/translations PROPERTY CLEAN_NO_CUSTOM  1)
set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1 )
#lupdate start
set(TS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/translations")
set(TS_FILES
    "${TS_DIR}/${PROJECT_NAME}_en_US.ts"    
)
find_program(LUPDATE_EXECUTABLE lupdate)
foreach(_ts_file ${TS_FILES})
    execute_process(
        COMMAND ${LUPDATE_EXECUTABLE} -recursive ${CMAKE_SOURCE_DIR} -ts ${_ts_file})
endforeach()
#lupdate end

file(GLOB ALL_SOURCES "*.cpp" "./partedproxy/*.cpp" "./widgets/*.cpp" "./widgets/customcontrol/*.cpp" "./widgets/accessible/*.cpp")
file(GLOB ALL_HEADERS "*.h" "./partedproxy/*.h" "./widgets/*.h" "./widgets/customcontrol/*.h" "./widgets/accessible/*.h")
file (GLOB DTNG_TS_FILES translations/*.ts)
if(Qt6_FOUND)
    qt_create_translation(DTNG_QM_FILES ${CMAKE_SOURCE_DIR} ${DTNG_TS_FILES})
else()
    qt5_create_translation(DTNG_QM_FILES ${CMAKE_SOURCE_DIR} ${DTNG_TS_FILES})
endif()

include_directories(../basestruct)
#link_directories(${PROJECT_BINARY_DIR})
set(ALL_SRCS   ${ALL_SOURCES} ${ALL_HEADERS})

# 生成执行文件
add_executable(${PROJECT_NAME} ${ALL_SRCS} ${APP_QRC}  ${DTNG_QM_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${DtkWidget_INCLUDE_DIRS}
    Qt${QT_VERSION_MAJOR}::GuiPrivate
    ${PROJECT_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME} ${LINK_LIBS} basestruct)

set(CMAKE_INSTALL_PREFIX /usr)

find_program(DESKTOP_TS_CONV deepin-desktop-ts-convert REQUIRED)
add_custom_target(APP_DESKTOP
    ALL
    COMMAND ${DESKTOP_TS_CONV} ts2desktop ${CMAKE_CURRENT_SOURCE_DIR}/assets/${APP_DESKTOP} ${CMAKE_CURRENT_SOURCE_DIR}/translations/desktop ${APP_DESKTOP}.tmp
    COMMAND mv ${APP_DESKTOP}.tmp ${APP_DESKTOP}
)

# Install files
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
#file(GLOB APP_QM_FILES "translations/*.qm")
#install(FILES ${APP_QM_FILES} DESTINATION share/${PROJECT_NAME}/translations)
install(FILES ${DTNG_QM_FILES} DESTINATION share/${PROJECT_NAME}/translations)
install(FILES ${APP_ICONPATH} DESTINATION share/icons/hicolor/scalable/apps)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${APP_DESKTOP} DESTINATION share/applications)
install(DIRECTORY ${APP_RES_DIR}/deepin-diskmanager DESTINATION /usr/share/deepin-manual/manual-assets/application)




