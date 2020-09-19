# VulkanGame
This is a game demo utilizing the SDL2 Vulkan API.

# Building 
## Windows
### VSCode 
* In the bottom right on status bar change configuration (click button) from Linux To Win32 if not changed.
* Ctrl+Shift+P -> Type Cmake: Build -> hit enter
### Cygwin
* Install Cygwin if not installed
* Navigate to the project directory
* Run ./cmake-windows-gcc.sh
* You can build the project using Visual Studio in the /build directory.
## Linux
### VSCode
* Build with VSCode

# Tasks
1. Multiple Windows.
  * Get a separate camera to render to a separate window.
  * Requires creating multiple render pipelines.
1. GUI
1. LUA Scripting

# TODO: Important
* Get Windows build to work (VS).
* Fix UI to show debug text
* The width/height of the window is messed up somewhere in the engineconfig props.

# TODO: Enhancments.
* Import Mobmaker into the app itself. We can compile it with mono or just convert the code.
* Simplified Engineconfig Properties
* Removing Default Width/Height - use percentage of screen.
* also add default X/Y
* Show window in center of screen (windowed mode)
* Test this build on Windows to make sure it still works.
* Fix the "unrecognized parameter" in the console.

# TODO: Backlog
* Change props from preprocessor to "debuggable" props.