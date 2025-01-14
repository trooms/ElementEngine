file(GLOB PRIVATE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp")

add_executable(ElementEngine
    main.cpp
    ${PRIVATE_SOURCES}
)

target_include_directories(ElementEngine
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/SDL/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/slang/slang
        ${CMAKE_CURRENT_SOURCE_DIR}/../external/glm/glm
        ${CMAKE_CURRENT_SOURCE_DIR}/Public
)

# Link SDL and Slang Libraries
target_link_libraries(ElementEngine
    PRIVATE
        SDL3::SDL3-static  # Links SDL dynamically
        slang              # Links Slang library
        glm                # Links GLM library
)

# Copy Assets Folder After Build
add_custom_command(TARGET ElementEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/ElementEngine/Assets"
        "$<TARGET_FILE_DIR:ElementEngine>/Assets"
)
