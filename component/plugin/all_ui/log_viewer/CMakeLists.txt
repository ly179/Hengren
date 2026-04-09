cmake_minimum_required(VERSION 3.16)
set(TARGET_NAME log_viewer)
project(${TARGET_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

add_compile_definitions(
    LOG_VIEW_GLOBAL_LIB
    __LOG_VIEWER__TARGET=${TARGET_NAME}
)
if (LOG_VIEWER_VERSION)
    add_compile_definitions(
        _LOG_VIEWER_VERSION_="${LOG_VIEWER_VERSION}"
    )
endif()

set(LOG_VIEWER_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/log_view_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/log_view_global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/log_viewer_signal_bridge.h

    ${CMAKE_CURRENT_SOURCE_DIR}/src/highlight_log_textarea.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_config_helper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_history_view_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_view_model.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_content_view_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cache_log_flusher.h
)
set(LOG_VIEWER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/highlight_log_textarea.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_config_helper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_history_view_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_view_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_view_model.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_content_view_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cache_log_flusher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_viewer_signal_bridge.cpp
)

set(LOG_VIEWER_RES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app_files.qrc
)

qt_add_library(${TARGET_NAME} SHARED
    ${LOG_VIEWER_HEADERS}
    ${LOG_VIEWER_SOURCES}
    ${LOG_VIEWER_RES}
)
if (COMMAND copy_target_lib)
    copy_target_lib(${TARGET_NAME} plugin ${IDP_VERSION})
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
    hengren_cdsem::data_common
)

target_include_directories(${TARGET_NAME}
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_includes
    ${CMAKE_CURRENT_SOURCE_DIR}/src
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

