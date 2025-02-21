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

option(GAME_BUILD "Define to build the game (removes editor code)" OFF)

if(GAME_BUILD)
    target_compile_definitions(Sandbox PRIVATE GAME_BUILD)
endif()

target_link_libraries(Sandbox PRIVATE Torx)