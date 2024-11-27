aux_source_directory(src SOURCE_FILES)
file(GLOB_RECURSE HEADER_FILES "include/*.hpp")
file(GLOB_RECURSE SHADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*")

source_group(
        TREE "${CMAKE_SOURCE_DIR}"
        FILES ${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES} main.cpp
)

add_executable(${TARGET_NAME} main.cpp ${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES}
        include/Camera.hpp)

set(THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/3rdparty")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SHADER_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(GLM_INCLUDE "${THIRD_PARTY_DIR}/glm")
#set(GLFW_ROOT "${THIRD_PARTY_DIR}/GLFW-VS2022")
set(GLFW_INCLUDE "${THIRD_PARTY_DIR}/glfw/include")
set(LOADER_INCLUDE "${THIRD_PARTY_DIR}/tinyobjloader")

find_package(OpenGL REQUIRED)
find_package(OpenMP REQUIRED)

target_link_libraries(${TARGET_NAME}
        PUBLIC
        glfw
        glm
        glad
        ${OPENGL_LIBRARIES}
        OpenMP::OpenMP_CXX
)

#target_include_directories(${TARGET_NAME} PUBLIC ${THIRD_PARTY_DIR}/glad/include)
target_include_directories(${TARGET_NAME} PUBLIC ${THIRD_PARTY_DIR})
target_include_directories(${TARGET_NAME} PUBLIC ${INCLUDE_DIR})
target_include_directories(${TARGET_NAME} PUBLIC ${SHADER_INCLUDE})
target_include_directories(${TARGET_NAME} PUBLIC ${GLFW_INCLUDE})
target_include_directories(${TARGET_NAME} PUBLIC ${GLM_INCLUDE})
target_include_directories(${TARGET_NAME} PUBLIC ${LOADER_INCLUDE})


