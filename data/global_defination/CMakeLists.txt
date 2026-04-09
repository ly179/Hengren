cmake_minimum_required(VERSION 3.16)
project(hengren_global_defination)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GD_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/exception_code.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/signal_hub_plugin_ids.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/system_default_config_keys.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/cbox_global_keys.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/measurement_tool_defition.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/signal_hub_ids.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_type_defition.h
)

qt_add_library(hengren_global_defination INTERFACE
    ${GD_HEADERS}
)

target_sources(hengren_global_defination INTERFACE
    ${GD_HEADERS}
)

target_include_directories(hengren_global_defination INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
)

copy_global_def_code(hengren_global_defination ${CMAKE_CURRENT_SOURCE_DIR}/includes)
