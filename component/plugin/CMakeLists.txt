cmake_minimum_required(VERSION 3.16)
project(hengren_plugin_component)

set_property(GLOBAL PROPERTY GLOBAL_SUB_PLUGIN_COMPONENTS_STR "")
set(CMAKE_DEBUG_POSTFIX "d")
function(add_plugin_components dir_path)
    add_subdirectory(${dir_path})
    get_filename_component(FOLDER_NAME "${dir_path}" NAME)
    get_property(CURRENT_STR GLOBAL PROPERTY GLOBAL_SUB_PLUGIN_COMPONENTS_STR)
    if(CURRENT_STR STREQUAL "")
        set(UPDATED_STR "${FOLDER_NAME}")
    else()
        set(UPDATED_STR "${CURRENT_STR} ${FOLDER_NAME}")
    endif()
    set_property(GLOBAL PROPERTY GLOBAL_SUB_PLUGIN_COMPONENTS_STR "${UPDATED_STR}")
endfunction()


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(RELATION_VERSION "0_1_0")
set(CONTROL_PANE_VERSION "0_1_0")
set(WORKSHEET_VERSION "0_1_0")
set(DATA_MANAGER_VERSION "0_1_0")
set(IMAGE_MANAGER_VERSION "0_1_0")
set(TEMPLATE_VERSION "0_1_0")
set(IDW_VERSION "0_1_0")
set(IDP_VERSION "0_1_0")
set(AMP_VERSION "0_1_0")
set(IMG_OPERATION_VERSION "0_1_0")
set(IMG_OPTICS_VERSION "0_1_0")
set(XLSX_EXP_VERSION "0_1_0")
set(LOG_VIEWER_VERSION "0_1_0")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Svg Xml SerialPort Network)
find_package(Boost REQUIRED CONFIG)
find_package(qxlsx REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC OFF)
set(PLUGIN_COMPONENT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set(PLUGIN_INTERFACE_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idw/relation_idw_wafer_editor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idw/idw_wafer_relation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/data_manager/relation_image_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/data_manager/relation_msr_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/data_manager/relation_template_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/data_manager/data_manager_relation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_manager/relation_image_list_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_manager/relation_image_view_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_manager/image_manager_relation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/relation_helper/relation_cde90_export_helper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/relation_helper/relation_cde90_window_export.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/relation_helper/relation_cde90_frame_export.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/control_pane/relation_main_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/control_pane/main_control_relation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/data_manager/relation_file_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/worksheet/relation_worksheet_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/worksheet/worksheet_relation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idp_template/relation_ap_template_edit_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idp_template/relation_mp_template_edit_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idp_template/relation_idp_ap_template_edit_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idp_template/relation_idp_multi_ms_template_edit_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idp_template/relation_idp_mp_template_edit_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idp_template/relation_mpip_template_edit_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idp_template/relation_ms_multi_template_edit_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idp_template/template_relation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idp/relation_idp_data_edit_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idp/idp_edit_relation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/amp/relation_amp_edit_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/amp/amp_editor_relation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_optics/optics_editor_relation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_optics/relation_optics_edit_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_operation/relation_image_operation_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_operation/image_operation_relation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/xlsx_export/xlsx_export_relation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/global_hub/image_operation_data_relation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/log_viewer/log_viewer_relation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/log_viewer/relation_log_viewer_controller.h
        all_ui/recipe_creation/fem_measurement_canvas.cpp
)

qt_add_library(hengren_plugin_component INTERFACE
    ${PLUGIN_INTERFACE_HEADERS}
)

target_sources(hengren_plugin_component INTERFACE
    ${PLUGIN_INTERFACE_HEADERS}
)

set_target_properties(hengren_plugin_component PROPERTIES
    INTERFACE_AUTOMOC ON
)

target_link_libraries(hengren_plugin_component INTERFACE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    hengren_cdsem::base_core
)

target_include_directories(hengren_plugin_component INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/control_pane
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/data_manager
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idw
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idp
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/amp
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_manager
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/relation_helper
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/worksheet
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/xlsx_export
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idp_template
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_operation
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_optics
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/global_hub
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/log_viewer
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/recipe_creation
)
copy_interface_headers(plugin ${CMAKE_CURRENT_SOURCE_DIR}/includes/control_pane ${CONTROL_PANE_VERSION})
copy_interface_headers(plugin ${CMAKE_CURRENT_SOURCE_DIR}/includes/data_manager ${DATA_MANAGER_VERSION})
copy_interface_headers(plugin ${CMAKE_CURRENT_SOURCE_DIR}/includes/idw ${IDW_VERSION})
copy_interface_headers(plugin ${CMAKE_CURRENT_SOURCE_DIR}/includes/idp ${IDP_VERSION})
copy_interface_headers(plugin ${CMAKE_CURRENT_SOURCE_DIR}/includes/amp ${AMP_VERSION})
copy_interface_headers(plugin ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_manager ${IMAGE_MANAGER_VERSION})
copy_interface_headers(plugin ${CMAKE_CURRENT_SOURCE_DIR}/includes/relation_helper ${RELATION_VERSION})
copy_interface_headers(plugin ${CMAKE_CURRENT_SOURCE_DIR}/includes/worksheet ${WORKSHEET_VERSION})
copy_interface_headers(plugin ${CMAKE_CURRENT_SOURCE_DIR}/includes/xlsx_export ${XLSX_EXP_VERSION})
copy_interface_headers(plugin ${CMAKE_CURRENT_SOURCE_DIR}/includes/idp_template ${TEMPLATE_VERSION})
copy_interface_headers(plugin ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_operation ${IMG_OPERATION_VERSION})
copy_interface_headers(plugin ${CMAKE_CURRENT_SOURCE_DIR}/includes/image_optics ${IMG_OPTICS_VERSION})

add_plugin_components(${CMAKE_CURRENT_SOURCE_DIR}/all_ui/control_pane)
add_plugin_components(${CMAKE_CURRENT_SOURCE_DIR}/all_ui/worksheet_creation)
add_plugin_components(${CMAKE_CURRENT_SOURCE_DIR}/all_ui/data_manager_creation)
add_plugin_components(${CMAKE_CURRENT_SOURCE_DIR}/all_ui/image_manager)
add_plugin_components(${CMAKE_CURRENT_SOURCE_DIR}/all_ui/idw_creation)
add_plugin_components(${CMAKE_CURRENT_SOURCE_DIR}/all_ui/idp_creation)
add_plugin_components(${CMAKE_CURRENT_SOURCE_DIR}/all_ui/amp_operation)
add_plugin_components(${CMAKE_CURRENT_SOURCE_DIR}/all_ui/image_operation)
add_plugin_components(${CMAKE_CURRENT_SOURCE_DIR}/all_ui/log_viewer)

add_plugin_components(${CMAKE_CURRENT_SOURCE_DIR}/func/xlsx_export)
