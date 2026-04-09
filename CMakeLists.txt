cmake_minimum_required(VERSION 3.16)
project(Hengren_I_SEM)
get_filename_component(LIB_ROOT "${CMAKE_CURRENT_LIST_DIR}/../common_library" ABSOLUTE)

include(${CMAKE_CURRENT_LIST_DIR}/component/module.cmake)
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/conan_path.cmake")
    include(${CMAKE_CURRENT_LIST_DIR}/conan_path.cmake)
endif()

set(CONAN_BIN $ENV{CONAN_HOME}/../conan.exe)
add_compile_definitions(
    QT_MESSAGELOGCONTEXT
)

set(CONAN_GEN_DIR "${CMAKE_BINARY_DIR}/conan_deps") # 定义存放目录
execute_process(
        COMMAND ${CONAN_BIN} install ${CMAKE_CURRENT_SOURCE_DIR}
        --output-folder=${CONAN_GEN_DIR}  # 关键：指定输出到该文件夹
        --update --build=missing
        --settings=build_type=${CMAKE_BUILD_TYPE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE conan_ret
)

set(CONAN_CONFIG_PATH "${CONAN_GEN_DIR}/build/generators")
# 修正点 2：将精确路径加入搜索列表
list(PREPEND CMAKE_PREFIX_PATH "${CONAN_CONFIG_PATH}")
find_package(hengren_cdsem_base REQUIRED CONFIG)
find_package(hengren_cdsem_brain REQUIRED CONFIG)
import_sub_module(${CMAKE_CURRENT_LIST_DIR}/data
    hengren_cdsem_data
)
add_subdirectory(component/ui_common)
add_subdirectory(component/plugin)
add_subdirectory(component/app)