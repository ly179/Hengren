cmake_minimum_required(VERSION 3.16)
set(TARGET_NAME amp_operation)
project(${TARGET_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

add_compile_definitions(
    UI_AMP_OPERATION_GLOBAL_LIB
    _AMP_TARGET=${TARGET_NAME}
)
if (AMP_VERSION)
    add_compile_definitions(
        _AMP_VERSION_=\"${AMP_VERSION}\"
    )
endif()

set(AMP_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/amp_edit_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/ui_amp_operation_global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/amp_signal_bridge.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/amp_corner_edit_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/amp_data_select_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/amp_dense_setting_edit_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/amp_diameter_hole_edit_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/amp_edge_roughness_edit_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/amp_ellipse_edit_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/amp_gap_edit_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/amp_lwr_analysis_edit_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/amp_mppc_edit_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/amp_open_close_hole_edit_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/amp_width_edit_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/amp_width_roughness_edit_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/detection_common_setting_controller.h
)
set(AMP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/amp_signal_bridge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/amp_edit_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/amp_corner_edit_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/amp_data_select_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/amp_dense_setting_edit_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/amp_diameter_hole_edit_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/amp_edge_roughness_edit_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/amp_ellipse_edit_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/amp_gap_edit_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/amp_lwr_analysis_edit_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/amp_mppc_edit_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/amp_open_close_hole_edit_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/amp_width_edit_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/amp_width_roughness_edit_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/detection_common_setting_controller.cpp
)

set(AMP_RES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/app_file.qrc
)

qt_add_library(${TARGET_NAME} SHARED
    ${AMP_HEADERS}
    ${AMP_SOURCES}
    ${AMP_RES}
)
if (COMMAND copy_target_lib)
    copy_target_lib(${TARGET_NAME} plugin ${AMP_VERSION})
endif()

target_link_libraries(${TARGET_NAME}
PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    hengren_cdsem::base_core
    hengren_cdsem::ui_common
    hengren_plugin_component
    hengren_global_defination
    hengren_cdsem::data_common
    hengren_cdsem::core_data_module
    hengren_cdsem::ui_core
    Boost::boost
)

target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/ui_includes
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
