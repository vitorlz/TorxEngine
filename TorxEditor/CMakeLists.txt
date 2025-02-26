project(TorxEditor)

add_executable(TorxEditor "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp" "src/Editor/Editor.cpp" "src/Editor/Editor.h" "src/Editor/EditorCamera.cpp" "src/Editor/EditorCamera.h" "src/UI/UI.cpp" "src/UI/UI.h")

# copy dlls into executable directory
add_custom_command(TARGET TorxEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/Torx/vendor/dlls/" "${CMAKE_CURRENT_BINARY_DIR}"
)

# copy res into executable directory
add_custom_command(TARGET TorxEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/res/" "${CMAKE_CURRENT_BINARY_DIR}/res"
)

add_custom_command(TARGET TorxEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/TorxEditor/res/shaders"
            "${CMAKE_CURRENT_BINARY_DIR}/../Projects/Sandbox/res/shaders"
    COMMENT "Copying shaders from TorxEditor to Sandbox"
)

add_custom_command(TARGET TorxEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/TorxEditor/res/shaders"
            "${CMAKE_SOURCE_DIR}/Projects/template/res/shaders"
    COMMENT "Copying shaders from TorxEditor to template"
)

file(COPY "${CMAKE_SOURCE_DIR}/TorxEditor/res/shaders" DESTINATION "${CMAKE_SOURCE_DIR}/Projects/template/res")

target_link_libraries(TorxEditor PRIVATE Torx)