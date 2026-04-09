cmake_minimum_required(VERSION 3.16)
project(entity_common)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

add_compile_definitions(
    ENTITY_COMMON_GLOBAL_LIB
)

set(ENTITY_COMMON_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/amp_data_entity.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/cdsem_auth_entity.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/class_entity.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idp_amp_data_entity.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idp_data_entity.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idw_wafer_entity.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_relation_record_entity.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/log_file_scan_entity.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/log_record_entity.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/memory_condition_entity.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/msr_entity.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/recipe_data_entity.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/scan_mode_config_entity.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/system_default_config_entity.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/template_info_entity.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/entity_common_global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/stage_memory_entity.h
)

set(ENTITY_COMMON_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/idw_wafer_entity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_file_scan_entity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_record_entity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scan_mode_config_entity.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/system_default_config_entity.cpp
)

qt_add_library(entity_common OBJECT
    ${ENTITY_COMMON_HEADERS}
    ${ENTITY_COMMON_SOURCES}
)

target_link_libraries(entity_common PRIVATE
    Qt6::Core
    Qt6::Sql
    hengren_cdsem::base_core
    Boost::boost
    hengren_global_defination
)

target_include_directories(entity_common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
)
