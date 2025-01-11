cmake_minimum_required(VERSION 3.20)

project(ElementEngine VERSION 1.0 LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_CXX_STANDARD 23)
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

set(SDL_SHARED ON CACHE BOOL "Build SDL3 as a shared library" FORCE)
set(SDL_STATIC ON CACHE BOOL "Build SDL3 as a static library" FORCE)

add_subdirectory(Libraries/SDL)
add_subdirectory(ElementEngine)
