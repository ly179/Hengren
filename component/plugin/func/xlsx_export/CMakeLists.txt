cmake_minimum_required(VERSION 3.16)
set(TARGET_NAME xlsx_export)
project(${TARGET_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

add_compile_definitions(
        CDSEM_PLUGIN_MSR_EXPORT_GLOBAL
        _XLSX_EXP_TARGET=${TARGET_NAME}
)
if (XLSX_EXP_VERSION)
    add_compile_definitions(
        _XLSX_EXP_VERSION_="${XLSX_EXP_VERSION}"
    )
endif ()

set(XLSX_EXPORT_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/cdsem_plugin_msr_export_global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/xlsx_export_signal_bridge.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/msr_export_image_drawer.h
)
set(XLSX_EXPORT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/msr_export_image_drawer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/xlsx_export_signal_bridge.cpp
)

qt_add_library(${TARGET_NAME} SHARED
    ${XLSX_EXPORT_HEADERS}
    ${XLSX_EXPORT_SOURCES}
)
if (COMMAND copy_target_lib)
    copy_target_lib(${TARGET_NAME} plugin ${XLSX_EXP_VERSION})
endif ()

target_link_libraries(${TARGET_NAME}
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    hengren_cdsem::base_core
    hengren_cdsem::base_data_module
    hengren_cdsem::core_data_module
    hengren_cdsem::ui_core
    hengren_cdsem::data_common
    hengren_global_defination
    hengren_cdsem::ui_common
    qxlsx::qxlsx
    Boost::boost
)

target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_includes
)
