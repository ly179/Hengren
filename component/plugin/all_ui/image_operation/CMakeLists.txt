cmake_minimum_required(VERSION 3.16)
set(TARGET_NAME image_operation)
project(${TARGET_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

add_compile_definitions(
        UI_IMAGE_OPERATION_GLOBAL_LIB
        _IMAGE_OPERATION_TARGET=${TARGET_NAME}
)
if (IMG_OPERATION_VERSION)
    add_compile_definitions(
        _IMG_OPERATION_VERSION_="${IMG_OPERATION_VERSION}"
    )
endif ()

set(IMG_OPERATION_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_operation_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_operation_signal_bridge.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/ui_image_operation_global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/image_operation_custom_setter_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/image_operation_view_setter_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/cursor_handler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/diameter_hole_cursor_handler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/edge_roughness_cursor_handler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/ellipse_cursor_handler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/image_operation_controller_data_helper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/image_operation_controller_ui_helper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/width_cursor_handler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/width_roughness_cursor_handler.h
)
set(IMG_OPERATION_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/image_operation_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/image_operation_signal_bridge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/image_operation_custom_setter_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/image_operation_view_setter_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/cursor_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/diameter_hole_cursor_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/edge_roughness_cursor_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/ellipse_cursor_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/image_operation_controller_data_helper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/image_operation_controller_ui_helper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/width_cursor_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/width_roughness_cursor_handler.cpp
)

set(IMG_OPERATION_RES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app_file.qrc
)

qt_add_library(${TARGET_NAME} SHARED
    ${IMG_OPERATION_HEADERS}
    ${IMG_OPERATION_SOURCES}
    ${IMG_OPERATION_RES}
)
if (COMMAND copy_target_lib)
    copy_target_lib(${TARGET_NAME} plugin ${IMG_OPERATION_VERSION})
endif ()

target_link_libraries(${TARGET_NAME}
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    hengren_cdsem::base_core
    hengren_cdsem::ui_common
    hengren_cdsem::ui_core
    hengren_plugin_component
    hengren_cdsem::base_data_module
    hengren_cdsem::core_data_module
    hengren_cdsem::core_brain
    hengren_cdsem::data_common
    hengren_global_defination
    Boost::boost
)

target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_includes
)
