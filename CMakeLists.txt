cmake_minimum_required(VERSION 3.28.3)

# @todo
# - Modules don't work.
# - Assimp doesn't work.

# ==============
# Project Setup
# ==============

# Define project.
project(SilentEngine)

# Specify C and C++ standards.
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)

# Set compiler options.
if(MSVC)
    add_compile_options(/std:c++20) # Enable modules.
    add_compile_options(/wd4018)    # Suppress sign comparison warnings.
    add_compile_options(/wd4244)    # Suppress narrowing/conversion warnings. Required for C struct compat.
    add_compile_options(/wd4267)    # Suppress type conversion warnings.
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-Mmodules)            # Enable modules.
    add_compile_options(-Wno-narrowing)       # Silence narrowing entirely. Required for C struct compat.
    add_compile_options(-Wno-error=narrowing) # Suppress conversion narrowing errors. Required for C struct compat.
    add_compile_options(-Wno-changes-meaning) # Suppress name shadowing errors.
    add_compile_options(-Wformat-extra-args)  # Suppress format string errors.
    add_compile_options(-Wno-sign-compare)    # Suppress sign comparison warnings.
    add_compile_options(-Wno-conversion)      # Suppress type conversion warnings.
endif()

# Set `GIT_REPO` definition.
execute_process(
    COMMAND git remote get-url origin
    OUTPUT_VARIABLE GIT_REPO
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_definitions(-DGIT_REPO="${GIT_REPO}")

# Set `GIT_BRANCH` definition.
execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_definitions(-DGIT_BRANCH="${GIT_BRANCH}")

# Set `GIT_COMMIT` definition.
execute_process(
    COMMAND git rev-parse HEAD
    OUTPUT_VARIABLE GIT_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_definitions(-DGIT_COMMIT="${GIT_COMMIT}")

# Add executable.
file(GLOB_RECURSE SOURCES
    CONFIGURE_DEPENDS "Source/*.cpp"
)
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "${CMAKE_PROJECT_NAME}"
    DEBUG_POSTFIX "_d"
)

# Set `Debug` build definitions.
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
add_definitions(-D_DEBUG)
endif()

# Set compiler options for `Debug` and `Release` modes.
if(MSVC)
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
        $<$<CONFIG:DEBUG>:-DDEBUG_MODE>
        $<$<CONFIG:RELEASE>:/O2 /Zi>
    )

    target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
        $<$<CONFIG:RELEASE>:/DEBUG>
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
        $<$<CONFIG:DEBUG>:-DDEBUG_MODE>
        $<$<CONFIG:RELEASE>:-O2 -g>
    )
endif()

# Set compiler warning level.
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

# =================
# Dependency Setup
# =================

# Add library sources.
file(GLOB LIB_SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/Libraries/imgui/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Libraries/imgui/backends/imgui_impl_sdl3.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Libraries/imgui/backends/imgui_impl_sdlgpu3.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Libraries/ImGuizmo/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/Libraries/smol-atlas/src/*.cpp"
)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${LIB_SOURCES})

# Link libraries statically.
set(BUILD_SHARED_LIBS OFF)

# Add project header paths.
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/Source
    ${CMAKE_CURRENT_LIST_DIR}/..
)

# Add library directories.
#add_subdirectory(Libraries/assimp)
add_subdirectory(Libraries/freetype)
add_subdirectory(Libraries/glm)
add_subdirectory(Libraries/json)
add_subdirectory(Libraries/psyz/psyz)
#add_subdirectory(Libraries/SDL) # @todo PSY-Z also uses SDL and keeping this line causes a conflict.
add_subdirectory(Libraries/sol2)
add_subdirectory(Libraries/spdlog)
add_subdirectory(Libraries/utfcpp)
add_subdirectory(Libraries/yalantinglibs)

# Add library header paths.
target_include_directories(${CMAKE_PROJECT_NAME} SYSTEM PRIVATE
    #${CMAKE_CURRENT_LIST_DIR}/Libraries/assimp/include
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/freetype/include
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/glm
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/imgui
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/imgui/backends
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/ImGuizmo
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/json/include
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/LuaJIT/src
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/pl_mpeg
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/psyz/psyz/include
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/SDL/include
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/smol-atlas/src
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/sol2/include
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/spdlog
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/stb
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/utfcpp/source
    ${CMAKE_CURRENT_LIST_DIR}/Libraries/yalantinglibs/include
)

# Create target for LuaJIT library.
add_library(luajit STATIC IMPORTED)
set_target_properties(luajit PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/Libraries/LuaJIT/src/libluajit.a")

# Set PSY-Z compile definition.
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE __psyz)

# Add libraries.
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    #assimp
    freetype
    glm
    luajit
    psyz
    sol2
    SDL3::SDL3-static
    spdlog
    yalantinglibs
)
