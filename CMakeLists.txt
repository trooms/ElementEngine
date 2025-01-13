cmake_minimum_required(VERSION 3.20)

project(ElementEngine VERSION 1.0 LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(
    -Wall
    -Werror=return-type
    -Werror=unused-result
    -Werror=return-stack-address
    -ftrivial-auto-var-init=pattern
)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

# SDL3 Configuration
set(SDL_SHARED OFF CACHE BOOL "Disable SDL3 as a shared library" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build SDL3 static library" FORCE)
add_subdirectory(external/SDL)

# Slang Configuration
set(SLANG_ENABLE_GFX OFF CACHE BOOL "Disable GFX targets for Slang")
set(SLANG_ENABLE_SLANGC OFF CACHE BOOL "Enable standalone compiler target for shader compilation")
set(SLANG_ENABLE_SLANGRT OFF CACHE BOOL "Disable runtime target")
set(SLANG_ENABLE_SLANGD OFF CACHE BOOL "Disable language server target")
set(SLANG_ENABLE_TESTS OFF CACHE BOOL "Disable test targets")
set(SLANG_ENABLE_EXAMPLES OFF CACHE BOOL "Disable example targets")
set(SLANG_ENABLE_SLANG_RHI OFF CACHE BOOL "Disable slang-rhi module")
set(SLANG_ENABLE_SLANG_GLSLANG OFF CACHE BOOL "Disable glslang dependency")
add_subdirectory(external/slang)

# Add ElementEngine Source
add_subdirectory(ElementEngine)


