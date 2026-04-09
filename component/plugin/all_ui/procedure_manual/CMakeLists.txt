cmake_minimum_required(VERSION 3.16)
set(TARGET_NAME procedure_manual)
project(${TARGET_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

add_compile_definitions(
    UI_PROCEDURE_MANUAL_LIB
    __UI_PROCEDURE_MANUAL_TARGET=${TARGET_NAME}
)
if (PROCEDURE_MANUAL_VERSION)
    add_compile_definitions(
        _UI_PROCEDURE_MANUAL_VERSION_="${PROCEDURE_MANUAL_VERSION}"
    )
endif()

set(PRO_M_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/procedure_manual_normal_window.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/procedure_manual_signal_bridge.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/procedure_manual_global.h
)

set(PRO_M_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/procedure_manual_normal_window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/procedure_manual_signal_bridge.cpp
)

set(PRO_M_RES
   ${CMAKE_CURRENT_SOURCE_DIR}/src/app_file.qrc
)

qt_add_library(${TARGET_NAME} SHARED
    ${PRO_M_HEADERS}
    ${PRO_M_SOURCES}
    ${PRO_M_RES}
)
if (COMMAND copy_target_lib)
    copy_target_lib(${TARGET_NAME} plugin ${PROCEDURE_MANUAL_VERSION})
endif()

target_link_libraries(${TARGET_NAME}
PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    hengren_cdsem::base_core
    hengren_plugin_component
    hengren_global_defination
    Boost::boost
)

target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_includes
)
