cmake_minimum_required(VERSION 3.16)
project(application_image_test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui)
find_package(Boost REQUIRED CONFIG)
find_package(opencv REQUIRED)

add_compile_definitions(
    __SOURCE_CODE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"
)


add_executable(application_image_test
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_link_libraries(application_image_test
PRIVATE
    Qt6::Core
    Qt6::Gui
    hengren_cdsem::base_core
    hengren_cdsem::core_data_module
    hengren_cdsem::base_data_module
    hengren_cdsem::core_brain
    hengren_cdsem::data_common
    hengren_global_defination
    Boost::boost
    opencv::opencv
)
