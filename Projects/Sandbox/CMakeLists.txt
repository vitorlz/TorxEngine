project(Sandbox)

add_executable(Sandbox "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp" )

# copy dlls into executable directory
add_custom_command(TARGET Sandbox POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/Torx/vendor/dlls/" "${CMAKE_CURRENT_BINARY_DIR}"
)

# copy res into executable directory

if (NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/res")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/res" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
endif()

file(COPY "${CMAKE_SOURCE_DIR}/TorxEditor/res/shaders" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/res")

target_link_libraries(Sandbox PRIVATE Torx)