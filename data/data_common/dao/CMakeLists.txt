cmake_minimum_required(VERSION 3.16)
project(database_dao)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

add_compile_definitions(
    DATABASE_DAO_GLOBAL_LIB
)

set(DAO_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/amp_data_dao.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/class_dao.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/dao_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/general_database_dao_global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/global_connection_factory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idp_data_dao.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idw_wafer_dao.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_relation_record_dao.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/log_record_dao.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/memory_condition_dao.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/msr_dao.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/recipe_data_dao.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/sem_page_search_param.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/sem_scan_mode_config_dao.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/system_default_config_dao.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/template_info_dao.h
        ${CMAKE_CURRENT_SOURCE_DIR}/includes/stage_memory_dao.h
)

set(DAO_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/amp_data_dao.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/class_dao.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/dao_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/global_connection_factory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/idp_data_dao.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/idw_wafer_dao.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/image_relation_record_dao.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_record_dao.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/memory_condition_dao.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/msr_dao.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/recipe_data_dao.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/sem_scan_mode_config_dao.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/system_default_config_dao.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/template_info_dao.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/stage_memory_dao.cpp
)

qt_add_library(database_dao OBJECT
    ${DAO_HEADERS}
    ${DAO_SOURCES}
)

target_link_libraries(database_dao PRIVATE
    Qt6::Sql
    Qt6::Core
    hengren_cdsem::base_core
    entity_common
    module_common
    Boost::boost
    hengren_global_defination
    hengren_cdsem::core_data_module
    hengren_cdsem::base_data_module
)

target_include_directories(database_dao PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
)
