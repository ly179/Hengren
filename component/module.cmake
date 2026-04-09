set(GIT_EXECUTABLE git)
set(_CFG "${CMAKE_BUILD_TYPE}")
if(NOT _CFG)
    set(_CFG "Debug")
endif()
string(TOUPPER ${_CFG} _CFG_UPPER)

function(copy_all_files TARGET)
    # 遍历除第一个参数(TARGET)之外的所有输入路径
    foreach(SOURCE ${ARGN})
        # 排除空路径
        if("${SOURCE}" STREQUAL "")
            continue()
        endif()

        # 1. 如果是目录：拷贝其内部所有内容，不拷贝目录本身
        if(IS_DIRECTORY "${SOURCE}")
            file(GLOB CONTENTS "${SOURCE}/*")
            if(CONTENTS)
                file(COPY ${CONTENTS} DESTINATION "${TARGET}")
                message(STATUS "SmartCopy: Copied contents of [DIR] ${SOURCE} -> ${TARGET}")
            endif()

            # 2. 如果是文件：直接拷贝
        elseif(EXISTS "${SOURCE}")
            file(COPY "${SOURCE}" DESTINATION "${TARGET}")
            message(STATUS "SmartCopy: Copied [FILE] ${SOURCE} -> ${TARGET}")

        else()
            message(WARNING "SmartCopy: Source '${SOURCE}' not found, skipping.")
        endif()
    endforeach()
endfunction()

#拷贝函数，分lib/plugin_lib和plugin三种target_type
function(copy_target_lib target_name target_type version_val)
    # 根据类型决定基础路径
    if (target_type STREQUAL "lib")
        set(BASE_PATH "libs")
    elseif (target_type STREQUAL "plugin_lib")
        set(BASE_PATH "plugin_libs")
    elseif (target_type STREQUAL "plugin")
        set(BASE_PATH "plugin")
    else()
        message(FATAL_ERROR "target type: ${target_type} is not supported.")
    endif()

    # 1. 拷贝二进制产物 (自动处理后缀 'd')
    if (version_val STREQUAL "NULL")
        set(DLL_PATH "${LIB_ROOT}/${BASE_PATH}/${_CFG}/$<TARGET_FILE_NAME:${target_name}>")
        set(LIB_PATH "${LIB_ROOT}/${BASE_PATH}/${_CFG}/$<TARGET_LINKER_FILE_NAME:${target_name}>")
        set(HEAD_PATH "${LIB_ROOT}/includes/${BASE_PATH}/${target_name}")
    else()
        set(DLL_PATH "${LIB_ROOT}/${BASE_PATH}/${_CFG}/${version_val}/$<TARGET_FILE_NAME:${target_name}>")
        set(LIB_PATH "${LIB_ROOT}/${BASE_PATH}/${_CFG}/${version_val}/$<TARGET_LINKER_FILE_NAME:${target_name}>")
        set(HEAD_PATH "${LIB_ROOT}/includes/${BASE_PATH}/${version_val}/${target_name}")
    endif()
    add_custom_command(TARGET ${target_name} POST_BUILD
        # 拷贝 DLL (或可执行文件)
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:${target_name}>"
        ${DLL_PATH}
        VERBATIM
    )

    # 2. 如果是库，还需要拷贝 .lib 文件
    if (target_type MATCHES "lib")
        add_custom_command(TARGET ${target_name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_LINKER_FILE:${target_name}>"
                ${LIB_PATH}
                VERBATIM
        )

        # 3. 拷贝头文件
        foreach(include_dir ${ARGN})
            add_custom_command(TARGET ${target_name} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    "${include_dir}"
                    ${HEAD_PATH}
                    VERBATIM
            )
        endforeach()
    endif()
endfunction()

function(import_sub_module sub_module)
    if (EXISTS "${sub_module}/CMakeLists.txt")
        add_subdirectory(${sub_module})
    else()
        set(ARGS_LIST ${ARGN})
        list(LENGTH ARGS_LIST ARGS_COUNT)
        math(EXPR STOP_INDEX "${ARGS_COUNT} - 1")
        foreach(INDEX RANGE 0 ${STOP_INDEX} 1)
            list(GET ARGS_LIST ${INDEX} find_name)
            find_package(${find_name} REQUIRED CONFIG)
        endforeach()
    endif()
endfunction()

#子模块导入函数，分lib/plugin_lib和interface三种类型
function(add_sub_module sub_module lib_type)
    if (EXISTS "${sub_module}/CMakeLists.txt")
        add_subdirectory(${sub_module})
    else()
        if (lib_type STREQUAL "lib")
            set(lib_path "libs")
        elseif(lib_type STREQUAL "plugin_lib")
            set(lib_path "plugin_libs")
        elseif(lib_type STREQUAL "interface")
            set(lib_path "plugin")
        else()
            return()
        endif()
        # 获取参数列表及其长度
        set(ARGS_LIST ${ARGN})
        list(LENGTH ARGS_LIST ARGS_COUNT)
        # 以步长 2 遍历参数 (Name, Version)
        math(EXPR STOP_INDEX "${ARGS_COUNT} - 1")
        foreach(INDEX RANGE 0 ${STOP_INDEX} 2)
            # 提取当前步长的 库名 和 版本号
            list(GET ARGS_LIST ${INDEX} lib_name)
            math(EXPR VERSION_INDEX "${INDEX} + 1")
            list(GET ARGS_LIST ${VERSION_INDEX} lib_version)
            if (lib_type STREQUAL "interface")
                add_library(${lib_name} INTERFACE IMPORTED GLOBAL)
                set_target_properties(${lib_name} PROPERTIES
                    INTERFACE_INCLUDE_DIRECTORIES "${LIB_ROOT}/includes/${lib_path}/${lib_name}/${lib_version}"
                )
            else()
                add_library(${lib_name} SHARED IMPORTED GLOBAL)
                set_target_properties(${lib_name} PROPERTIES
                    INTERFACE_INCLUDE_DIRECTORIES "${LIB_ROOT}/includes/${lib_path}/${lib_version}/${lib_name}"
                )
            endif()
            if (NOT lib_type STREQUAL "interface")
                set_target_properties(${lib_name} PROPERTIES
                    IMPORTED_IMPLIB_${_CFG_UPPER}   "${LIB_ROOT}/${lib_path}/${_CFG}/${lib_version}/${lib_name}.lib"
                    IMPORTED_LOCATION_${_CFG_UPPER} "${LIB_ROOT}/${lib_path}/${_CFG}/${lib_version}/${lib_name}.dll"
                )
            endif()
        endforeach()
    endif()
endfunction()

function(import_library lib_name lib_include)
    if(TARGET ${lib_name})
        return()
    endif()
    #检查所有的路径是否存在，不存在则不做任何导入操作。
    if (NOT lib_include STREQUAL "NULL" AND NOT EXISTS "${lib_include}")
        message(STATUS "include path not exits: ${lib_include}")
        return()
    endif()
    set(ARGS_LIST ${ARGN})
    list(LENGTH ARGS_LIST ARGS_COUNT)
    if (ARGS_COUNT EQUAL 0)
        add_library(${lib_name} INTERFACE IMPORTED GLOBAL)
        set_target_properties(${lib_name} PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${lib_include}"
        )
        return()
    endif()
    math(EXPR STOP_INDEX "${ARGS_COUNT} - 1")
    foreach(INDEX RANGE 0 ${STOP_INDEX} 2)
        list(GET ARGS_LIST ${INDEX} lib_path)
        if (NOT EXISTS "${lib_path}")
            message(STATUS "lib path not exits: ${lib_path}")
            return()
        endif()
    endforeach()
    add_library(${lib_name} SHARED IMPORTED GLOBAL)
    if (NOT lib_include STREQUAL "NULL")
        set_target_properties(${lib_name} PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${lib_include}"
        )
    endif()
    foreach(INDEX RANGE 0 ${STOP_INDEX} 2)
        list(GET ARGS_LIST ${INDEX} lib_path)
        math(EXPR VERSION_INDEX "${INDEX} + 1")
        list(GET ARGS_LIST ${VERSION_INDEX} lib_type)
        if (lib_type STREQUAL "dynamic")
            message("import library: ${lib_path}")
            set_target_properties(${lib_name} PROPERTIES
                IMPORTED_LOCATION_${_CFG_UPPER} "${lib_path}"
            )
        elseif (lib_type STREQUAL "static")
            message("import library: ${lib_path}")
            set_target_properties(${lib_name} PROPERTIES
                IMPORTED_IMPLIB_${_CFG_UPPER} "${lib_path}"
            )
        endif()
    endforeach()
endfunction()

#plugin_relation头文件拷贝
function(copy_interface_headers target_name)
    # ARGN 包含了 target_name 之后的所有参数
    list(LENGTH ARGN list_len)

    # 检查参数是否成对出现（路径 + 版本）
    math(EXPR is_even "${list_len} % 2")
    if(NOT ${is_even} EQUAL 0)
        message(FATAL_ERROR "copy_interface_headers 调用的参数必须成对出现 (路径 版本)")
    endif()

    # 创建一个自定义目标，汇总所有拷贝任务
    # 使用 ALL 确保该目标会被默认构建
    if(NOT TARGET copy_${target_name}_headers)
        add_custom_target(copy_${target_name}_headers ALL
            COMMENT "Starting header copy for ${target_name}..."
        )
    endif()

    # 步长为 2 遍历参数列表
    math(EXPR last_idx "${list_len} - 1")
    foreach(idx RANGE 0 ${last_idx} 2)
        list(GET ARGN ${idx} header_dir)      # 获取路径
        math(EXPR ver_idx "${idx} + 1")
        list(GET ARGN ${ver_idx} header_ver)  # 获取该路径对应的版本号

        # 获取文件夹名称（例如 control_pane），用于保持目录结构清晰
        get_filename_component(dir_name "${header_dir}" NAME)

        # 定义最终存放路径：.../includes/target_name/文件夹名/版本号
        set(SPECIFIC_DEST "${LIB_ROOT}/includes/${target_name}/${dir_name}/${header_ver}")

        add_custom_command(TARGET copy_${target_name}_headers POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${SPECIFIC_DEST}"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${header_dir}" "${SPECIFIC_DEST}"
            COMMENT "Copying ${dir_name} (v${header_ver}) to SDK..."
            VERBATIM
        )
    endforeach()
endfunction()

function(copy_global_def_code target_name source_path)
    add_custom_target(copy_${target_name}_headers ALL
        COMMENT "Starting header copy for ${target_name}..."
    )
    add_custom_command(TARGET copy_${target_name}_headers POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${source_path}"
                "${LIB_ROOT}/includes/${target_name}"
        VERBATIM
    )
endfunction()

function (copy_all_libs_to_runtime)
    message ("Build Type For SmartCopy: ${_CFG_UPPER}")
    copy_all_files("${CMAKE_BINARY_DIR}/project_all" "${LIB_ROOT}/msvc/postgre")
    if (_CFG_UPPER STREQUAL "DEBUG")
        copy_all_files("${CMAKE_BINARY_DIR}/project_all" "${LIB_ROOT}/msvc/opencv/opencv_world4100d.dll")
    else()
        copy_all_files("${CMAKE_BINARY_DIR}/project_all" "${LIB_ROOT}/msvc/opencv/opencv_world4100.dll")
    endif()
endfunction()