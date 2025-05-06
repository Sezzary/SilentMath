cmake_minimum_required(VERSION 3.31.6)

# Define project.
project(Math)

# Specify C and C++ standards.
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)
#set(CMAKE_CXX_SCAN_FOR_MODULES ON)
set(CMAKE_CXX_MODULES ON)

# Add executable.
file(GLOB_RECURSE SOURCES "Source/*.cpp")
add_executable(Math ${SOURCES})
set_target_properties(Math PROPERTIES OUTPUT_NAME "Math" DEBUG_POSTFIX "_d")

# Set Debug build definitions.
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
add_definitions(-D_DEBUG)
endif()

# Set compiler options for Debug and Release modes.
target_compile_options(Math PRIVATE
    #-include "${CMAKE_CURRENT_LIST_DIR}/Source/Framework.h"
    $<$<CONFIG:DEBUG>:-DDEBUG_MODE>
    $<$<CONFIG:RELEASE>:-O3>)

# Set compiler warning flags.
if(MSVC)
    # MSVC warning flags.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
else()
    # GCC/Clang warning flags.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

# Link libraries statically.
set(BUILD_SHARED_LIBS OFF)

# Add project header paths.
target_include_directories(Math PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/Source
    ${CMAKE_CURRENT_LIST_DIR}/..)

# Add library header paths.
target_include_directories(Math SYSTEM PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/glm
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/imgui
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/json/include
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/SDL/include
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/spdlog)

# Add library directories.
add_subdirectory(Libraries/glm)
add_subdirectory(Libraries/json)
add_subdirectory(Libraries/SDL)
add_subdirectory(Libraries/spdlog)

# Force-find Vulkan SDK path.
set(Vulkan_INCLUDE_DIR "$ENV{VK_SDK_PATH}/Include")
set(Vulkan_LIBRARY "$ENV{VK_SDK_PATH}/Lib/vulkan-1.lib")

# Find packages.
find_package(Vulkan REQUIRED)

# Add libraries.
target_link_libraries(Math PRIVATE
    glm
    SDL3-static
    spdlog
    Vulkan::Vulkan)
