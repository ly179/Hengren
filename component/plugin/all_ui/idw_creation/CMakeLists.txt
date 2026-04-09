cmake_minimum_required(VERSION 3.16)
set(TARGET_NAME idw_creation)
project(${TARGET_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

add_compile_definitions(
    UI_IDW_CREATION_GLOBAL_LIB
    __IDW__TARGET=${TARGET_NAME}
)
if (IDW_VERSION)
    add_compile_definitions(
        _IDW_VERSION_=\"${IDW_VERSION}\"
    )
endif()

set(IDW_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idw_signals_bridge.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idw_wafer_editor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/ui_idw_creation_global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idw_detail_data_model.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idw_sub_editor_canvas.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idw_wafer_sub_editor.h
)
set(IDW_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/idw_signals_bridge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/idw_wafer_editor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idw_detail_data_model.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idw_sub_editor_canvas.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idw_wafer_sub_editor.cpp
)

qt_add_library(${TARGET_NAME} SHARED
    ${IDW_HEADERS}
    ${IDW_SOURCES}
)
if (COMMAND copy_target_lib)
    copy_target_lib(${TARGET_NAME} plugin ${IDW_VERSION})
endif()

target_link_libraries(${TARGET_NAME}
PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    hengren_cdsem::base_core
    hengren_cdsem::ui_common
    hengren_cdsem::core_data_module
    hengren_cdsem::base_data_module
    hengren_cdsem::core_brain
    hengren_cdsem::ui_core
    hengren_cdsem::data_common
    hengren_plugin_component
    hengren_global_defination
    Boost::boost
)

target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
