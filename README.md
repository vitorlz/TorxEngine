# TorxEngine
Custom game engine built from scratch using C++ and OpenGL.

https://github.com/user-attachments/assets/c94439a4-017c-409b-bb2d-52b3e5a0c8bd

https://github.com/user-attachments/assets/bd16ce13-ceeb-48e2-b0ed-964f2b4c2a72

-------------------------------------------------------------------------------
## Features:
<li>Voxel Cone Tracing Global Illumination</li> 
<li>Screen Space Reflections</li>
<li>Screen Space Ambient Occlusion</li> 
<li>ECS</li> 
<li>BulletPhysics integration</li> 
<li>Shadow mapping</li> 
<li>Normal mapping</li> 
<li>Percentage-Closer Filtering</li> 
<li>Gamma correction</li> 
<li>Tone mapping</li> 
<li>Skybox</li> 
<li>Bloom</li> 
<li>PBR materials support</li> 
<li>Point, directional, and spot lights</li> 
 <br/>

## Project Build Instructions

**Make sure you have CMake installed.** <br/>
**The program only supports Windows for now.**

Clone the repository
```
git clone git@github.com:vitorlz/TorxEngine.git
```
From the directory containing CMakeLists.txt:
```
mkdir build
```
```
cd build
```
```
cmake ..
```
```
cmake --build . --config Release
```
To run:
```
cd bin
```
```
./TorxEngine.exe
```
