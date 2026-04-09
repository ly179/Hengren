cmake_minimum_required(VERSION 3.16)
project(application_cdsem)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Network Svg Xml Widgets)
find_package(Boost REQUIRED CONFIG)
find_package(postgre REQUIRED CONFIG)
find_package(mysql REQUIRED CONFIG)
find_package(opencv REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/project_all)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/project_all)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/project_all)

add_executable(application_cdsem WIN32
    ${CMAKE_CURRENT_SOURCE_DIR}/main_prepared.h
    ${CMAKE_CURRENT_SOURCE_DIR}/main_prepared.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/base_init_handler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/base_init_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/task_init_handler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/task_init_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/config_module_task.h
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/config_module_task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/task_module.h
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/hardware_module_task.h
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/hardware_module_task.cpp
)

get_property(ALL_PLUGIN_COMPONENTS GLOBAL PROPERTY GLOBAL_SUB_PLUGIN_COMPONENTS_STR)
add_compile_definitions(
    __PLUGIN_DIR=\"${LIB_ROOT}/plugin/${CMAKE_BUILD_TYPE}\"
    __PLUGIN_LIB_DIR=\"${LIB_ROOT}/plugin_libs/${CMAKE_BUILD_TYPE}\"
    __SOURCE_CODE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"
    _SUB_PLUGIN_COMPONENTS_=\"${ALL_PLUGIN_COMPONENTS}\"
    __LOG_COLLECTOR_BIN=\"${LIB_ROOT}/log_collector/app_log_collector.exe\"
    # 如果需要锁定某个插件版本编译，可打开该变量。
    # 格式 如：idw 0_2_0 idp 0_1_0
    # _LOCKED_PLUGIN_VERSION_=\"\"
)

target_link_libraries(application_cdsem
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
    postgre::postgre
    mysql::mysql
)
