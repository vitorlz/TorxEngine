cmake_minimum_required(VERSION 3.15)
project(TorxEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()

include_directories(
    "${CMAKE_SOURCE_DIR}/vendor/assimp/include"
    "${CMAKE_SOURCE_DIR}/vendor/bulletphysics/include"
    "${CMAKE_SOURCE_DIR}/vendor/freetype/include"
    "${CMAKE_SOURCE_DIR}/vendor/glad/include"
    "${CMAKE_SOURCE_DIR}/vendor/GLFW/include"
    "${CMAKE_SOURCE_DIR}/vendor/glm/include"
    "${CMAKE_SOURCE_DIR}/vendor/nlohmann/include"
    "${CMAKE_SOURCE_DIR}/vendor/stb_image"
    "${CMAKE_SOURCE_DIR}/vendor/imgui"
    "${CMAKE_SOURCE_DIR}/vendor/imguizmo"
    "${CMAKE_SOURCE_DIR}/include"
)

link_directories(
    "${CMAKE_SOURCE_DIR}/vendor/assimp/lib"
    "${CMAKE_SOURCE_DIR}/vendor/bulletphysics/lib/Debug"
    "${CMAKE_SOURCE_DIR}/vendor/bulletphysics/lib/Release"
    "${CMAKE_SOURCE_DIR}/vendor/freetype/lib"
    "${CMAKE_SOURCE_DIR}/vendor/GLFW/lib"
)

add_library(glad STATIC
    "${CMAKE_SOURCE_DIR}/vendor/glad/src/glad.c"
)
target_include_directories(glad PUBLIC "${CMAKE_SOURCE_DIR}/vendor/glad/include")

file(GLOB IMGUI_SOURCES
    "${CMAKE_SOURCE_DIR}/vendor/imgui/*.cpp"
    "${CMAKE_SOURCE_DIR}/vendor/imgui/*.h"
)

file(GLOB IMGUIZMO_SOURCES
    "${CMAKE_SOURCE_DIR}/vendor/imguizmo/*.cpp"
    "${CMAKE_SOURCE_DIR}/vendor/imguizmo/*.h"
)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC "${CMAKE_SOURCE_DIR}/vendor/imgui")

add_library(imguizmo STATIC ${IMGUIZMO_SOURCES})
target_include_directories(imguizmo PUBLIC "${CMAKE_SOURCE_DIR}/vendor/imguizmo")

add_library(stb_image STATIC
    "${CMAKE_SOURCE_DIR}/vendor/stb_image/stb_img_implementation.cpp"
)
target_include_directories(stb_image PUBLIC "${CMAKE_SOURCE_DIR}/vendor/stb_image")

file(GLOB_RECURSE TORXENGINE_SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.h"
)

add_executable(TorxEngine ${TORXENGINE_SOURCES})

target_link_libraries(TorxEngine PRIVATE
    glad
    imgui
    imguizmo
    stb_image
    $<$<CONFIG:Debug>:BulletCollision_Debug>
    $<$<CONFIG:Debug>:BulletDynamics_Debug>
    $<$<CONFIG:Debug>:LinearMath_Debug>
    $<$<CONFIG:Release>:BulletCollision>
    $<$<CONFIG:Release>:BulletDynamics>
    $<$<CONFIG:Release>:LinearMath>
    $<$<CONFIG:Debug>:assimp-vc143-mtd>
    $<$<CONFIG:Release>:assimp-vc143-mt>
    freetype
    glfw3
    opengl32  
)

add_custom_command(TARGET TorxEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/vendor/dlls"
        $<TARGET_FILE_DIR:TorxEngine>
)

add_custom_command(TARGET TorxEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/res"
        "$<TARGET_FILE_DIR:TorxEngine>/res"
)

set_target_properties(TorxEngine PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>")

if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif()
