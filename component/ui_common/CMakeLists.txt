cmake_minimum_required(VERSION 3.16)
project(ui_common)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)
set(CMAKE_DEBUG_POSTFIX "d")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Boost REQUIRED CONFIG)

add_compile_definitions(
    UI_COMMON_GLOBAL_LIB
)


set(UI_COMMON_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/auto_focus_offset_setting_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/auto_focus_setting_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/auto_focus_threshold_setting_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/cdsem_device_selector.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/class_list_selector_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/data_select_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/file_class_copy_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/file_idp_copy_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/file_idw_copy_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/file_recipe_copy_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/file_simple_copy_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/focus_auto_content.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/focus_stigma_content.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idp_ap_add_reg_helper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idp_list_selector_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idw_canvas_item_check_style.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idw_list_selector_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/ifem_setting_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_scan_setting_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/img_env_setting_content.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/list_view_data_query_helper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/list_view_main_data_model.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/mpip_template_select_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/multi_shot_chip_list_model.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/multi_shot_chip_select_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/pattern_recognition_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/pattern_recognition_param_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/pattern_reg_info_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/pre_dose_setting_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/template_selector_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/ui_common_global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/z_sensor_setting_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_view_measure_tool_draw_helper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/pattern_alg_itembox_helper.h
)

set(UI_COMMON_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/auto_focus_offset_setting_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/auto_focus_setting_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/auto_focus_threshold_setting_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cdsem_device_selector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/class_list_selector_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/data_select_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/file_class_copy_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/file_idp_copy_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/file_idw_copy_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/file_recipe_copy_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/file_simple_copy_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/focus_auto_content.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/focus_stigma_content.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/idp_ap_add_reg_helper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/idp_list_selector_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/idw_canvas_item_check_style.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/idw_list_selector_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ifem_setting_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/image_scan_setting_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/img_env_setting_content.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/list_view_data_query_helper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/list_view_main_data_model.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mpip_template_select_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/multi_shot_chip_list_model.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/multi_shot_chip_select_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pattern_recognition_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pattern_recognition_param_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pattern_reg_info_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pre_dose_setting_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/template_selector_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/z_sensor_setting_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/image_view_measure_tool_draw_helper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/pattern_alg_itembox_helper.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/ui_includes)

set(UI_COMMON_RES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app_files.qrc
)

qt_add_library(ui_common
SHARED
    ${UI_COMMON_HEADERS}
    ${UI_COMMON_SOURCES}
    ${UI_COMMON_RES}
)
add_library(hengren_cdsem::ui_common ALIAS ui_common)

if (COMMAND copy_target_lib)
    copy_target_lib(ui_common plugin_lib "NULL"
        ${CMAKE_CURRENT_SOURCE_DIR}/includes
    )
endif()

target_link_libraries(ui_common
PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    hengren_cdsem::base_core
    hengren_cdsem::data_common
    hengren_cdsem::core_brain
    hengren_cdsem::base_data_module
    hengren_cdsem::core_data_module
    hengren_global_defination
    hengren_plugin_component
    hengren_cdsem::ui_core
    Boost::boost
)

target_include_directories(ui_common
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
)
