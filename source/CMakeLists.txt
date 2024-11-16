set(TARGET_NAME ${CMAKE_PROJECT_NAME})

aux_source_directory(src SOURCE_FILES)
file(GLOB_RECURSE HEADER_FILES "include/*.hpp")

source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}" 
    FILES ${SOURCE_FILES} ${HEADER_FILES} main.cpp
    )

add_executable(${TARGET_NAME} main.cpp ${SOURCE_FILES} ${HEADER_FILES})


set(THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/3rdparty")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(GLFW_INCLUDE ${THIRD_PARTY_DIR}/glfw-3.4/include)

target_link_libraries(${TARGET_NAME} PUBLIC glfw)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${INCLUDE_DIR}>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${GLFW_INCLUDE}>)

