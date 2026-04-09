cmake_minimum_required(VERSION 3.16)
set(TARGET_NAME image_manager)
project(${TARGET_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

add_compile_definitions(
    UI_IMG_MANAGER_GLOBAL_LIB
    __IMAGE_MANAGER_TARGET=${TARGET_NAME}
)
if (IMAGE_MANAGER_VERSION)
    add_compile_definitions(
        _IMAGE_MANAGER_VERSION_=\"${IMAGE_MANAGER_VERSION}\"
    )
endif()

set(IMG_MANAGER_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_list_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_manager_signal_bridge.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_view_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/ui_image_manager_global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/image_info_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/image_overlay_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/overlay_control_model.h
)
set(IMG_MANAGER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/image_list_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/image_manager_signal_bridge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/image_view_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/image_info_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/image_overlay_controller.cpp
)

set(IMG_MANAGER_RES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app_file.qrc
)

qt_add_library(${TARGET_NAME} SHARED
    ${IMG_MANAGER_HEADERS}
    ${IMG_MANAGER_SOURCES}
    ${IMG_MANAGER_RES}
)
if (COMMAND copy_target_lib)
    copy_target_lib(${TARGET_NAME} plugin ${IMAGE_MANAGER_VERSION})
endif()

target_link_libraries(${TARGET_NAME}
PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    hengren_cdsem::base_core
    hengren_plugin_component
    hengren_global_defination
    hengren_cdsem::core_data_module
    hengren_cdsem::base_data_module
    hengren_cdsem::data_common
    hengren_cdsem::ui_common
    hengren_cdsem::ui_core
    Boost::boost
)

target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_includes
)
