cmake_minimum_required(VERSION 3.16)
project(app_log_collector)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Network Sql)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/project_all)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/project_all)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/project_all)

add_executable(app_log_collector
    ${CMAKE_CURRENT_SOURCE_DIR}/src/abstract_service.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_content_format.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_content_struct.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_file_reader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_saver.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_scan_processor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_scanner.h

    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/abstract_service.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_content_format.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_file_reader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_saver.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_scan_processor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log_scanner.cpp
)

get_property(ALL_PLUGIN_COMPONENTS GLOBAL PROPERTY GLOBAL_SUB_PLUGIN_COMPONENTS_STR)
add_compile_definitions(
    __SOURCE_CODE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"
)

target_link_libraries(app_log_collector
PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Sql
    Qt6::Gui
    hengren_cdsem::base_core
    hengren_cdsem::data_common
    hengren_global_defination
)

set(DLL_PATH "${LIB_ROOT}/libs/${_CFG}/$<TARGET_FILE_NAME:app_log_collector>")
add_custom_command(TARGET app_log_collector POST_BUILD
    # 拷贝 DLL (或可执行文件)
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:app_log_collector>"
    ${DLL_PATH}
    VERBATIM
)
