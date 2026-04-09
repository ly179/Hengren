cmake_minimum_required(VERSION 3.16)
set(TARGET_NAME control_pane)
project(${TARGET_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

add_compile_definitions(
    UI_CONTROL_PANE_GLOBAL_LIB
    __CONTROL_PANE_TARGET=${TARGET_NAME}
)
if (CONTROL_PANE_VERSION)
    add_compile_definitions(
        _CONTROL_PANE_VERSION_=\"${CONTROL_PANE_VERSION}\"
    )
endif()

set(CONTROL_PANE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/control_pane_signals_bridge.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/control_pane_global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/main_controller_helper.h
)

set(CONTROL_PANE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/control_pane_signals_bridge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/main_controller_helper.cpp
)

set(CONTROL_PANE_RES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app_file.qrc
)

qt_add_library(${TARGET_NAME}
SHARED
    ${CONTROL_PANE_HEADERS}
    ${CONTROL_PANE_SOURCES}
    ${CONTROL_PANE_RES}
)
if (COMMAND copy_target_lib)
    copy_target_lib(${TARGET_NAME} plugin ${CONTROL_PANE_VERSION})
endif()

# set_target_properties(${TARGET_NAME} PROPERTIES
#     AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/src"
# )

target_link_libraries(${TARGET_NAME}
PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    hengren_cdsem::base_core
    hengren_plugin_component
    hengren_global_defination
    Boost::boost
    hengren_cdsem::core_data_module
    hengren_cdsem::data_common
    image_operation
)

target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_includes
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private
    ${CMAKE_CURRENT_SOURCE_DIR}/../image_operation/includes
    ${CMAKE_CURRENT_SOURCE_DIR}/../image_manager/includes
)
