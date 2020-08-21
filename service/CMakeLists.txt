cmake_minimum_required(VERSION 3.0)
project(deepin-diskmanager-service)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


set(APP_RES_DIR "assets")
set(APP_SERVICE "${APP_RES_DIR}/data/com.deepin.diskmanager.service")
set(APP_CONFIG "${APP_RES_DIR}/data/com.deepin.diskmanager.conf")

# Find the library
find_package(PkgConfig REQUIRED)
find_package(DtkGui REQUIRED)
find_package(Qt5 COMPONENTS
    Core
    Widgets
    DBus
REQUIRED)

set(LINK_LIBS
    Qt5::Core
    Qt5::DBus
    ${DtkCore_LIBRARIES}
    parted
    parted-fs-resize
)

file(GLOB ALL_SOURCES "*.cpp" "./diskoperation/*.cpp" "./diskoperation/filesystems/*.cpp")
file(GLOB ALL_HEADERS "*.h" "./diskoperation/*.h" "./diskoperation/filesystems/*.h")

include_directories(../basestruct)
include_directories(../log)
link_directories(${PROJECT_BINARY_DIR})

add_executable(${PROJECT_NAME} ${ALL_SOURCES} ${ALL_HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${PROJECT_BINARY_DIR}
)
#link_libraries(../basestruct/basestruct)
target_link_libraries(${PROJECT_NAME} ${LINK_LIBS} basestruct)
target_link_libraries(${PROJECT_NAME} ${LINK_LIBS} ddmlog)


set(CMAKE_INSTALL_PREFIX /usr)

# Install files
install(TARGETS ${PROJECT_NAME} DESTINATION lib/deepin-daemon/)
install(FILES ${APP_SERVICE} DESTINATION share/dbus-1/system-services/)
install(FILES ${APP_CONFIG} DESTINATION share/dbus-1/system.d/)
