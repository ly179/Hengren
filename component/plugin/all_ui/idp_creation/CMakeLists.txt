cmake_minimum_required(VERSION 3.16)
set(TARGET_NAME idp_creation)
project(${TARGET_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

add_compile_definitions(
    UI_IDP_CREATION_GLOBAL_LIB
    __IDP__TARGET=${TARGET_NAME}
)
if (IDP_VERSION)
    add_compile_definitions(
        _IDP_VERSION_="${IDP_VERSION}"
    )
endif()

set(IDP_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idp_signals_bridge.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/idp_data_editor_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/includes/ui_idp_creation_global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/axis_align_detail_content.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/axis_align_wafer_selector_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/axis_alignment_wafer_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/chip_map_helper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/d_no_modify_input_validator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_ap_detail_editor_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_ap_wafer_selector_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_chip_selector_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_common_util.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_d_no_measurement_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_d_no_p_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_init_stage_different_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_measurement_arrange_setting_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_message_box_helper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_mp_coordinate_setting_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_mp_data_table_model.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_mp_detail_editor_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_mp_wafer_selector_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_opened_stage_different_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_p_measure_times_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_p_no_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_shift_coordinate_setting_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/image_environment_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/template_handler_helper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/template_save_box_controller.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/wizard/wizard_component.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/wizard/wizard_list_data.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/wizard/ap/ap_wizard_components.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/wizard/idp_wizard_window.h
)
set(IDP_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/idp_signals_bridge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/idp_data_editor_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/axis_align_detail_content.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/axis_align_wafer_selector_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/axis_alignment_wafer_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/chip_map_helper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/d_no_modify_input_validator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_ap_detail_editor_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_ap_wafer_selector_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_chip_selector_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_common_util.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_d_no_measurement_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_d_no_p_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_init_stage_different_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_measurement_arrange_setting_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_message_box_helper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_mp_coordinate_setting_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_mp_data_table_model.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_mp_detail_editor_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_mp_wafer_selector_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_opened_stage_different_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_p_measure_times_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_p_no_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/idp_shift_coordinate_setting_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/image_environment_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/template_save_box_controller.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/wizard/wizard_component.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/wizard/ap/ap_wizard_components.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/wizard/idp_wizard_window.cpp
)

set(IDP_RES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app_file.qrc
)

qt_add_library(${TARGET_NAME} SHARED
    ${IDP_HEADERS}
    ${IDP_SOURCES}
    ${IDP_RES}
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
    hengren_cdsem::ui_common
    hengren_cdsem::ui_core
    hengren_cdsem::core_data_module
    hengren_cdsem::core_brain
    hengren_cdsem::base_data_module
    hengren_cdsem::data_common
    hengren_plugin_component
    hengren_global_defination
    Boost::boost
)

target_include_directories(${TARGET_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui_includes
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private
    ${CMAKE_CURRENT_SOURCE_DIR}/src/private/wizard
)
