cmake_minimum_required(VERSION 3.16)
set(TARGET_NAME worksheet_creation)
project(${TARGET_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

add_compile_definitions(
    UI_WORKSHEET_GLOBAL_LIB
    __WORKSHEET_TARGET=${TARGET_NAME}
)
if (WORKSHEET_VERSION)
    add_compile_definitions(
        _WORKSHEET_VERSION_=\"${WORKSHEET_VERSION}\"
    )
endif()

set(WORKSHEET_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/ui_worksheet_creation_global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/worksheet_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/worksheet_signal_bridge.h
)
set(WORKSHEET_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/worksheet_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/worksheet_signal_bridge.cpp
)

set(WORKSHEET_RES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app_file.qrc
)

qt_add_library(${TARGET_NAME} SHARED
    ${WORKSHEET_HEADERS}
    ${WORKSHEET_SOURCES}
    ${WORKSHEET_RES}
)
if (COMMAND copy_target_lib)
    copy_target_lib(${TARGET_NAME} plugin ${WORKSHEET_VERSION})
endif()

target_link_libraries(${TARGET_NAME}
PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    hengren_cdsem::base_core
    hengren_plugin_component
    hengren_global_defination
    hengren_cdsem::data_common
    hengren_cdsem::base_data_module
    hengren_cdsem::core_data_module
    hengren_cdsem::ui_core
    Boost::boost
)

target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_includes
)
