cmake_minimum_required(VERSION 3.16)
project(application_widget_test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Network Svg Xml Widgets PrintSupport)
find_package(Boost REQUIRED CONFIG)
find_package(opencv REQUIRED)
find_package(jkqtplotter REQUIRED)
find_package(sentinel_ldk REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/project_all)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/project_all)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/project_all)

add_executable(application_widget_test
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main_prepared.h
    ${CMAKE_CURRENT_SOURCE_DIR}/main_prepared.cpp
)

get_property(ALL_PLUGIN_COMPONENTS GLOBAL PROPERTY GLOBAL_SUB_PLUGIN_COMPONENTS_STR)
add_compile_definitions(
    __PLUGIN_DIR=\"${LIB_ROOT}/plugin/${CMAKE_BUILD_TYPE}\"
    __PLUGIN_LIB_DIR=\"${LIB_ROOT}/plugin_libs/${CMAKE_BUILD_TYPE}\"
    __SOURCE_CODE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"
    __LIB_PATH=\"${LIB_ROOT}\"
    _SUB_PLUGIN_COMPONENTS_=\"${ALL_PLUGIN_COMPONENTS}\"
    # 如果需要锁定某个插件版本编译，可打开该变量。
    # 格式 如：idw 0_2_0 idp 0_1_0
    # _LOCKED_PLUGIN_VERSION_=\"\"
)

target_link_libraries(application_widget_test
PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Network
    Qt6::Svg
    Qt6::Xml
    Qt6::Widgets
    hengren_cdsem::base_core
    hengren_cdsem::data_common
    hengren_cdsem::core_data_module
    hengren_cdsem::base_data_module
    hengren_plugin_component
    hengren_global_defination
    hengren_cdsem::ui_common
    hengren_cdsem::ui_core
    hengren_cdsem::core_brain
    opencv::opencv
    Boost::boost
    jkqtplotter::JKQTPlotter6
    Qt6::PrintSupport
    Thales::sentinel_ldk
)
