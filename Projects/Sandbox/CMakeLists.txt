project(Sandbox)

add_executable(Sandbox "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp" )

# copy dlls into executable directory
add_custom_command(TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/Torx/vendor/dlls/" "${CMAKE_CURRENT_BINARY_DIR}"
)

# copy res into executable directory
add_custom_command(TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/Torx/res/" "${CMAKE_CURRENT_BINARY_DIR}/res"
)

target_link_libraries(Sandbox PRIVATE Torx)