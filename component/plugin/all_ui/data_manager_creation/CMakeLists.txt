cmake_minimum_required(VERSION 3.16)
set(TARGET_NAME data_manager_creation)
project(${TARGET_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

add_compile_definitions(
    UI_DATA_MANAGER_CREATION_GLOBAL_LIB
    __DATA_MANAGER_TARGET=${TARGET_NAME}
)
if (DATA_MANAGER_VERSION)
    add_compile_definitions(
        _DATA_MANAGER_VERSION_=\"${DATA_MANAGER_VERSION}\"
    )
endif()

set(DATA_MANAGER_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/data_manager_signal_bridge.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/file_manager_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_manager_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/msr_manager_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/template_manager_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/ui_data_manager_creation_global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/cdsem_class_editor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/common_data_loader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/data_manager_controller_helper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/history_view_controller.h
)
set(DATA_MANAGER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/data_manager_signal_bridge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/file_manager_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/image_manager_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/msr_manager_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/template_manager_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/cdsem_class_editor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/common_data_loader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/data_manager_controller_helper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/history_view_controller.cpp
)

set(DATA_MANAGER_RES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app_file.qrc
)

qt_add_library(${TARGET_NAME} SHARED
    ${DATA_MANAGER_HEADERS}
    ${DATA_MANAGER_SOURCES}
    ${DATA_MANAGER_RES}
)
if (COMMAND copy_target_lib)
    copy_target_lib(${TARGET_NAME} plugin ${DATA_MANAGER_VERSION})
endif()

target_link_libraries(${TARGET_NAME}
PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Svg
    Qt6::Xml
    hengren_cdsem::base_core
    hengren_cdsem::base_data_module
    hengren_cdsem::core_data_module
    hengren_cdsem::ui_common
    hengren_cdsem::data_common
    hengren_plugin_component
    hengren_global_defination
    Boost::boost
)

target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_includes
)
