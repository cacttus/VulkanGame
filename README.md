# VulkanGame
A demo game engine which uses the SDL2 Vulkan API.

# Building and Running
### CMake & Visual Studio - Windows
* Open VulkanGame.sln with Visual Studio 2019.
* Run ./cmake-windows-gcc.sh via Cygwin.
### VSCode 
* Change configuration from Linux / Win32 at the bottom.
* Ctrl+Shift+P -> Cmake: Build -> Enter.
### CMake - Debian Linux
* Install the following libraries via apt.
* Note the following dependencies can now be installed in Debian Linux by running __deps.sh__ as root.
* build-essential
* cmake
* libsdl2-dev
* libsdl2-net-dev
* ** NOTE: For some reason libsdl2-net isn't configured on my system. Installing libsdl-net fixed that problem.
* libsdl-net1.2-dev
* libvulkan-dev
* libopengl-dev
* liblua5.3-dev
* libluabind-dev
* libglm-dev

# Design

## Folders

|  Folder | Description|
|---------|----------------------------------------------------------------------------------------------
|  base   | graphs, generic systems, memory, buffers, networking, events, OS.        |
|  ext    | External libraries.           |
|  gfx    | GPU, graphics, images, materials, lighting, framebuffers, shaders.    |
|  math   | vectors, matrices, boxes, geometry, hulls, algorithms.                    |
|  model  | meshes, animation, models, characters, skeletons, bones.                  |
|  world  | physics, scenegraph.                                     |

| Class     | Description                                                                                                                                                                                                                                                                                                                                                                                                                                             |
|-----------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|  Package  | Application and project state inforamtion.  Scenes, models, meshes textures.                    |
|  Window   | SDL window.            |
|  Context  | Rendering context.                  |
|  Scene    | Scenegraph & meshes.          |
|  Gu       | Global static class to access core/active context.         |
|  Ui       | 2D user interface |

## Hierarchy

* Gu
  * Texture Manager (1, GLOBAL)
  * Mesh Manager (1, GLOBAL)
  * ShaderManager (1, GLOBAL)
    * Package (1, GLOBAL)
  * GraphicsApi (1, GLOBAL)
    * Context (1+)
      * GraphicsWindow (1+)
        * GraphicsWindow (child)(1+)
        * FrameSync (1)
        * Delta (1)
        * RenderPipe (1)
          * Picker (1)
        * UiScreen (1)
        * PhysicsWorld (1)
        * Scene (1)
          * LightNode (1*)
            * :PointLight
            * :DirLight
          * CameraNode (1*, only 1 active at a time)
          * PhysicsNode (1*) 
          * MeshNode (1*)
          * ModelNode (1*)
          * SceneNode (1*)
            * Component (1*)
              * BottleScript (1)
                * World25 (1)
