# TorxEngine
Custom game engine built from scratch using C++ and OpenGL.

![bistroDay](https://github.com/user-attachments/assets/ed6f0fec-b238-4b61-a846-dc06dfa78f00)

![bistroNight](https://github.com/user-attachments/assets/a08f6897-2df1-4fc3-9165-486a109112a4)

## Features:
<li>Voxel Cone Tracing Global Illumination</li> 
<li>Screen Space Reflections</li>
<li>Screen Space Ambient Occlusion</li> 
<li>Skeletal animations</li>
<li>Native scripting</li>
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
From the TorxEngine directory:
```
mkdir build
cd build
cmake ..
```
**After that, run the TorxEditor project in release mode.**

## How to use
After booting up TorxEditor, you can take a look at an example scene by going **File** → **Load Project** → **Sandbox**. <br/>
You can also create a new empty project. However, this is not advised yet as new project creation is a work in progress.

## Controls
The editor camera's controls are very similar to Blender's:
<table>
  <tr>
    <th>Control</th>
    <th>Action</th>
  </tr>
  <tr>
    <td>Middle Mouse Click</td>
    <td>Pan the camera</td>
  </tr>
  <tr>
    <td>Middle Mouse Click + Shift</td>
    <td>Move up, down, right, and left</td>
  </tr>
  <tr>
    <td>Scroll</td>
    <td>Move backwards and forwards</td>
  </tr>
 <tr>
    <td>Right click</td>
    <td>Select an entity.</td>
  </tr>
</table>
Mouse picking will only work if the entity has a rigidbody component. <br/>
If you hit play in the Sandbox project, you will be able to control the player with WASD and the 
mouse cursor will be hidden. Hit ESCAPE to see your mouse again and click the stop button to go back to the editor camera.

## To do  
<li>Implement one more GI option</li> 
<li>Python scripting</li> 
<li>Improve performance</li> 
<li>Improve UI and user experience</li> 




