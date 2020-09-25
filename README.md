# VulkanGame
This is a game demo utilizing the SDL2 Vulkan API.

# Changes
Moved OpenGLUtils to GLContext per the multiple contexts.
HappySky -> Skybox

# Building 
## Windows
### Visual Studio
* Build with the Visual Studio Solution.
### VSCode 
* In the bottom right on status bar change configuration (click button) from Linux To Win32 if not changed.
* Ctrl+Shift+P -> Type Cmake: Build -> hit enter
### CMake
* (Cygwin if not installed)
* Navigate to the project directory
* Run ./cmake-windows-gcc.sh
* You can build the project using Visual Studio in the /build directory
## Linux
### VSCode
* sudo apt-get update
* sudo apt-get upgrade
* Install (apt install)
  * libsdl2-dev
  * libsdl2-net-dev
  * libvulkan-dev
  * libopengl-dev
  * cmake
* Build with Cmake + GCC, CLANG

# Tasks
1. Multiple Windows. 
  * Notes:
  * Inspect SDL_GL_MakeCurrent to see if we can simply make another window current, same context.
  * Path: Its a scene node so its context isn't fixed. The meshes use a context. TODO: make the meshes components.
  * SkyBox: Change to SceneNode with Mesh /atlas componetns.
    
    * How to render the same scene from 2 windows.
      Can't get context from scene as scene is being drawn from 2 separate widnows so it shares a context.
      Window = Viewport + FBOs + Camera(s)
        // Graphics: Multiple contexts -> 1 thread
        // Other Threads: Multiple threads -> queue graphics Commands

    * Problem: Many things in Scene require the specific window, for instance the UI (window viewport, input, window context)
      * Solution:
        * Multiple UI Screen's per window.
        * UI Screen's communicate with each other via events & databind.
          * ex: Click button on screen #2, -> write a script to send click event to screen #1

    I want to check for errors in WorldGrid
      Runs from Script
      Runs from ScriptComponent
      Runs from SceneNode
      from Scene
      From Window -> Context
    Multiple windows with multiple contexts running at the same time.


  1. Goal is to draw the same scene on 2 different windows.
  1. __Synchrous windows (child windows) - shared context.__
    * https://community.khronos.org/t/sharing-contexts/73563/3
    * Not possible. Must use separate contexts.
    * Unshared GL Objects
      * VAO, FBO, 
    1. SDL_GL_makecurrent SDL_GL_SwapWindow
  1. Asynchronous (separate GL context)
    * Requires creating multiple render pipelines.
    * Replace Gu::checkErrors with getContext()->checkerrors
  1. Get a separate camera to render to a separate window.
1. GUI
1. LUA Scripting

# TODO: Important
* Get Windows build to work (VS).
* Fix UI to show debug text
* The width/height of the window is messed up somewhere in the engineconfig props.

# TODO: Enhancments.
* Make RenderUtils be a class instance and hold the instantiated renders that we add to it.
  * Later - Add them to scene instead of using RenderUtils.
* Import Mobmaker into the app itself. We can compile it with mono or just convert the code.
* Simplified Engineconfig Properties
* Removing Default Width/Height - use percentage of screen.
* also add default X/Y
* Show window in center of screen (windowed mode)
* Test this build on Windows to make sure it still works.
* Fix the "unrecognized parameter" in the console.

# TODO: Backlog
* Change props from preprocessor to "debuggable" props.