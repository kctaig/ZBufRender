cmake_minimum_required(VERSION 3.10)

project(
        ZBufRender
        VERSION 1.0
        LANGUAGES CXX
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set(CMAKE_CXX_STANDARD 17)

set(TARGET_NAME ${CMAKE_PROJECT_NAME})

add_subdirectory(source)
add_subdirectory(3rdparty)