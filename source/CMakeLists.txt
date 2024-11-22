aux_source_directory(src SOURCE_FILES)
file(GLOB_RECURSE HEADER_FILES "include/*.hpp")
file(GLOB_RECURSE SHADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*")

# set(ASSERTS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/assets/*.obj")

# ≤È’“OpenGLø‚
find_package(OpenGL REQUIRED)

source_group(
    TREE "${CMAKE_SOURCE_DIR}" 
    FILES ${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES} main.cpp 
    )

add_executable(${TARGET_NAME} main.cpp ${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

set(THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/3rdparty")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SHADER_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(GLM_INCLUDE "${THIRD_PARTY_DIR}/glm")
set(GLFW_ROOT "${THIRD_PARTY_DIR}/GLFW-VS2022")
set(LOADER_INCLUDE "${THIRD_PARTY_DIR}/tinyobjloader")

target_link_libraries(${TARGET_NAME} PUBLIC "${GLFW_ROOT}/lib/glfw3.lib" glm ${OPENGL_LIBRARIES})

target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${INCLUDE_DIR}>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${SHADER_INCLUDE}>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${GLFW_ROOT}/include>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${GLM_INCLUDE}>)
target_include_directories(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${LOADER_INCLUDE}>)

