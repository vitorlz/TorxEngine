cmake_minimum_required(VERSION 3.15)
project(TorxEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(Torx)    
add_subdirectory(Projects/Sandbox)  
add_subdirectory(TorxEditor)  

target_compile_definitions(TorxEditor PRIVATE PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}")






