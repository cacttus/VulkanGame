# VulkanGame
This is a game demo utilizing the SDL2 Vulkan API.

# Tasks
1. Multiple Windows.
  * Get a separate camera to render to a separate window.
  * Requires creating multiple render pipelines.
1. GUI
1. LUA Scripting

# TODO
* Fix UI to show debug text
* The problem is in the debugPrintTextureInfo - this was causing all errors. Some kind of string thing.
* The width/height of hte window is messed up somewhere in the engineconfig props
** TODO: Change props from preprocessor to "debuggable" props.

# TODO: Quick Fixes
* Import Mobmaker into the app itself. We can compile it with mono or just convert the code.
* Simplified Engineconfig Properties
* Removing Default Width/Height - use percentage of screen.
* also add default X/Y
* Show window in center of screen (windowed mode)
* Test this build on Windows to make sure it still works.
* Fix the "unrecognized parameter" in the console.

