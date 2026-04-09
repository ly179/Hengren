cmake_minimum_required(VERSION 3.16)
project(database_service)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

add_compile_definitions(
    DATABASE_SERVICE_GLOBAL_LIB
)


set(SERVICE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/amp_data_service.h
        ${CMAKE_CURRENT_SOURCE_DIR}/includes/class_service.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/general_database_service_global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idp_data_service.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idw_wafer_service.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_relation_record_service.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/log_record_service.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/memory_condition_service.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/msr_service.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/recipe_data_service.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/sem_scan_mode_config_service.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/service_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/system_default_config_service.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/template_info_service.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/transaction_manager_service.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/stage_memory_service.h
)

set(SERVICE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/amp_data_service.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/class_service.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/idp_data_service.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/idw_wafer_service.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/image_relation_record_service.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_record_service.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/memory_condition_service.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/msr_service.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/recipe_data_service.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sem_scan_mode_config_service.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/service_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/system_default_config_service.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/template_info_service.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/transaction_manager_service.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stage_memory_service.cpp
)

qt_add_library(database_service OBJECT
    ${SERVICE_HEADERS}
    ${SERVICE_SOURCES}
)

target_link_libraries(database_service PRIVATE
    Qt6::Core
    Qt6::Sql
    hengren_cdsem::base_core
    entity_common
    module_common
    entity_module
    database_dao
    Boost::boost
    hengren_global_defination
    hengren_cdsem::core_data_module
    hengren_cdsem::base_data_module
)

target_include_directories(database_service PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
)
