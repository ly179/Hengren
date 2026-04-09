cmake_minimum_required(VERSION 3.16)
project(entity_module)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

add_compile_definitions(
    ENTITY_MODULE_GLOBAL_LIB
)


set(ENTITY_MODULE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/axis_align_model_convert.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/cdsem_global_env_data_model_convert.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/entity_module_global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/env_pattern_param_convert.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idp_data_model_convert.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idw_model_data_convert.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/memory_condition_model_convert.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/msr_data_convert.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/recipe_data_convert.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/scan_model_data_convert.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/template_data_model_convert.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/template_info_model_convert.h
)

set(ENTITY_MODULE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/axis_align_model_convert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cdsem_global_env_data_model_convert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/env_pattern_param_convert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/idp_data_model_convert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/idw_model_data_convert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/memory_condition_model_convert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/msr_data_convert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/recipe_data_convert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scan_model_data_convert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/template_data_model_convert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/template_info_model_convert.cpp
)

qt_add_library(entity_module OBJECT
    ${ENTITY_MODULE_HEADERS}
    ${ENTITY_MODULE_SOURCES}
)

target_link_libraries(entity_module PRIVATE
    Qt6::Core
    hengren_cdsem::base_core
    entity_common
    module_common
    hengren_global_defination
    hengren_cdsem::core_data_module
    hengren_cdsem::base_data_module
    Boost::boost
)

target_include_directories(entity_module PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
)
