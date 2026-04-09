cmake_minimum_required(VERSION 3.16)
project(module_common)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

add_compile_definitions(
    MODEL_COMMON_GLOBAL_LIB
)

set(MODULE_COMMON_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/cdsem_auth_model.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/recipe_data_model.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/template_info_model.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/file_copy_data_model.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idw_wafer_copy_model.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idp_data_copy_module.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_operation_data_model.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/module_common_global.h
)
set(MODULE_COMMON_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/image_operation_data_model.cpp
)

qt_add_library(module_common OBJECT
    ${MODULE_COMMON_HEADERS}
    ${MODULE_COMMON_SOURCES}
)

target_link_libraries(module_common PRIVATE
    Qt6::Core
    hengren_cdsem::base_core
    hengren_cdsem::core_data_module
    Boost::boost
    hengren_global_defination
    hengren_cdsem::base_data_module
)

target_include_directories(module_common
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
)
