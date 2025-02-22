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
    "${CMAKE_SOURCE_DIR}/Torx/res/" "${CMAKE_CURRENT_BINARY_DIR}/res"
)

target_link_libraries(TorxEditor PRIVATE Torx)