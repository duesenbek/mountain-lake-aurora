cmake_minimum_required(VERSION 3.20)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
elseif(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND EXISTS "C:/Program Files/Microsoft Visual Studio/18/Community/VC/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "C:/Program Files/Microsoft Visual Studio/18/Community/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
elseif(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND EXISTS "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

project(mountain-lake-aurora LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Reuse pre-installed vcpkg packages from assignment4 or assignment3 if present
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../assignment4/vcpkg_installed/x64-windows")
    list(PREPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../assignment4/vcpkg_installed/x64-windows")
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../assignment3/vcpkg_installed/x64-windows")
    list(PREPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../assignment3/vcpkg_installed/x64-windows")
endif()

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm CONFIG REQUIRED)

set(SOURCES
    src/main.cpp
    src/Window/Window.cpp
    src/Window/Window.h
    src/Shader/Shader.cpp
    src/Shader/Shader.h
    src/Mesh/Mesh.cpp
    src/Mesh/Mesh.h
)

add_executable(mountain-lake-aurora ${SOURCES})

target_include_directories(mountain-lake-aurora PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(mountain-lake-aurora PRIVATE
    OpenGL::GL
    glfw
    GLEW::GLEW
    glm::glm
)
