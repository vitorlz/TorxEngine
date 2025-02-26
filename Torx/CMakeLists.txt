include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/assimp/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/bulletphysics/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/freetype/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/GLFW/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/nlohmann/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb_image"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/imguizmo"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

link_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/assimp/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/bulletphysics/lib/Debug"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/bulletphysics/lib/Release"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/freetype/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/GLFW/lib"
)

file(GLOB_RECURSE TORX_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/src/glad.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/imguizmo/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb_image/stb_img_implementation.cpp"
)

add_library(Torx STATIC ${TORX_SOURCES})


target_include_directories(Torx PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/assimp/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/bulletphysics/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/freetype/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/GLFW/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/nlohmann/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb_image"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/imguizmo"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/IconFontCppHeaders"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"    
)

target_link_directories(Torx PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/assimp/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/bulletphysics/lib/Debug"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/bulletphysics/lib/Release"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/freetype/lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/GLFW/lib"
)

target_link_libraries(Torx PRIVATE
    BulletCollision
    BulletDynamics
    LinearMath
    assimp-vc143-mt
    freetype
    glfw3
    opengl32
)

if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif()
