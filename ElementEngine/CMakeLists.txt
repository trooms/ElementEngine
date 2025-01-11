file(GLOB PRIVATE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp")

add_executable(ElementEngine
    main.cpp
    ${PRIVATE_SOURCES}
)

target_include_directories(ElementEngine
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/SDL/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/Slang/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/cgltf
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(ElementEngine PRIVATE SDL3::SDL3-shared)
else()
    target_link_libraries(ElementEngine PRIVATE SDL3::SDL3-static)
endif()

set(CMAKE_PREFIX_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/../Libraries/Slang"
    ${CMAKE_PREFIX_PATH}
)
find_package(slang REQUIRED)

target_link_libraries(ElementEngine PRIVATE slang::slang)

file(GLOB_RECURSE ASSET_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/Assets/*"
)

add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory
            "${CMAKE_CURRENT_BINARY_DIR}/Assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/Assets"
            "${CMAKE_CURRENT_BINARY_DIR}/Assets"
    DEPENDS ${ASSET_FILES}
)

add_dependencies(ElementEngine copy_assets)

