cmake_minimum_required(VERSION 3.0)
project(deepin-diskmanager)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)#moc 预编译包含QObject文件，生成xx_moc.h cpp
set(CMAKE_AUTORCC ON)#开启rcc 编译资源文件qrc
set(CMAKE_AUTOUIC ON)#开启uic 编译转换*.ui文件生存设计界面对应的.h .cpp文件

# Find the library
find_package(PkgConfig REQUIRED)
find_package(DtkWidget REQUIRED)
find_package(DtkGui REQUIRED)
find_package(DtkCMake REQUIRED)
find_package(Qt5 COMPONENTS
    Core
    Widgets
    DBus
REQUIRED)

set(LINK_LIBS
    Qt5::Core
    Qt5::DBus
    Qt5::Widgets
    Qt5::WidgetsPrivate

    ${DtkWidget_LIBRARIES}
    ${DtkCore_LIBRARIES}
    ${DtkGUI_LIBRARIES}
)

#common resource names
set(APP_RES_DIR "assets")
set(APP_TRANS_SCRIPT "${APP_RES_DIR}/translate_generation.sh")
set(APP_DESKTOP "${APP_RES_DIR}/deepin-diskmanager.desktop")
set(APP_QRC "${APP_RES_DIR}/icons.qrc")
set(APP_ICONPATH "${APP_RES_DIR}/icons/deepin-diskmanager.svg")


#Generate the qm file
execute_process(COMMAND bash ${APP_TRANS_SCRIPT}
                    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/")

file(GLOB ALL_SOURCES "*.cpp" "./partedproxy/*.cpp" "./widgets/*.cpp" "./widgets/customcontrol/*.cpp")
file(GLOB ALL_HEADERS "*.h" "./partedproxy/*.h" "./widgets/*.h" "./widgets/customcontrol/*.h")
#资源文件
set (DTNG_QRC_FILES
    assets/resources.qrc
    assets/icons.qrc
)
include_directories(../basestruct)
link_directories(${PROJECT_BINARY_DIR})
set(ALL_SRCS   ${ALL_SOURCES} ${ALL_HEADERS})

# 生成执行文件
add_executable(${PROJECT_NAME} ${ALL_SRCS} ${APP_QRC})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${DtkWidget_INCLUDE_DIRS}
    ${Qt5Gui_PRIVATE_INCLUDE_DIRS}
    ${PROJECT_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME} ${LINK_LIBS} basestruct)

set(CMAKE_INSTALL_PREFIX /usr)

# Install files
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
file(GLOB APP_QM_FILES "translations/*.qm")
install(FILES ${APP_QM_FILES} DESTINATION share/${PROJECT_NAME}/translations)
install(FILES ${APP_ICONPATH} DESTINATION share/icons/hicolor/scalable/apps)
install(FILES ${APP_DESKTOP} DESTINATION share/applications)




