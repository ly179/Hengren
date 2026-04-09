cmake_minimum_required(VERSION 3.16)
project(hengren_data_common)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/project_all)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/project_all)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/project_all)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/project_all)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/project_all)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/project_all)
set(CMAKE_DEBUG_POSTFIX "d")

find_package(Qt6 REQUIRED COMPONENTS Core Sql)
find_package(Boost REQUIRED CONFIG)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/entity_common)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/module_common)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/entity_module)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/dao)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/service)

qt_add_library(hengren_data_common
SHARED
    $<TARGET_OBJECTS:entity_common>
    $<TARGET_OBJECTS:module_common>
    $<TARGET_OBJECTS:entity_module>
    $<TARGET_OBJECTS:database_dao>
    $<TARGET_OBJECTS:database_service>
)
add_library(hengren_cdsem::data_common ALIAS hengren_data_common)

if (COMMAND copy_target_lib)
    copy_target_lib(hengren_data_common lib "NULL"
        ${CMAKE_CURRENT_SOURCE_DIR}/entity_common/includes
        ${CMAKE_CURRENT_SOURCE_DIR}/entity_module/includes
        ${CMAKE_CURRENT_SOURCE_DIR}/module_common/includes
        ${CMAKE_CURRENT_SOURCE_DIR}/dao/includes
        ${CMAKE_CURRENT_SOURCE_DIR}/service/includes
    )
endif()

target_link_libraries(hengren_data_common
PRIVATE
    Qt6::Core
    Qt6::Sql
    hengren_cdsem::base_core
PUBLIC
    entity_common
    module_common
    entity_module
    database_dao
    database_service
)
