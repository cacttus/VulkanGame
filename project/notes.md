# Tasks
Math & Base - shared library =>  vg_bm.so
VG => vg.so
 
Add /vulkan, /opengl
Move Gu from Math & Base 
Move Gu to the lowest level (world)
Change all Gu singletons to their own classes to decouple math & base
No more global singleton.
* Replacing Gu with Utils for /base static globals.

Instead of a single Gu class, each system layer will have the following
  * A static utility class named by the layer **.cpp & **.h
  * A header named by the layer **Header where enums, class definitions, typedefs and small classes

Base Layer (no dependencies)
  Math, Base, App::checkErrors

SDL will go in the API layer
SDl, Window, Vulkan, OpenGL
SDL : App
  SDL::checkErrors

Top Layer
Meshes, Scene, World, etc..

Base
Math
System
  SDL
  OpenGLClasses
  VulkanClasses
  AppRunner





# GWindow integration
  1. Move src to src_stage and move files over 1-by-1
  1. System decoupling.
    * Gu is a problem. It makes it difficult to modularize the engine.
    * base, math will be independent .so now and have no external dependencies besides /ext
    * graphics, model, world depend on base & math
      * this will help us write the utility applications without duplicating code
        * shaderc_poll..
  1. Global coupled classes
    * Depend on multiple libraries.
    * AppRunner 
    * Gu
  1. Renderer decoupling.
    * OpenGL & dependent classes move to /opengl
      * buffers, models, VAOData & etc
      * RenderPipe
    * Vulkan classes in /vulkan
  1. Remove old systems -> component systems.
    * Spec system, NodeData &c
    * World

### Important
* DestroyWindow is not wired up correctly (SDL_DestroyWindow is not gbeing called)
* Fix the exit in GraphicsApi if user closes child window.
* Get Windows build to work (VS).
* Fix UI to show debug text
* The width/height of the window is messed up somewhere in the engineconfig props.

### Enhancments
* Make RenderUtils be a class instance and hold the instantiated renders that we add to it.
  * Later - Add them to scene instead of using RenderUtils.
* Import Mobmaker into the app itself. We can compile it with mono or just convert the code.
* Simplified Engineconfig Properties
* Removing Default Width/Height - use percentage of screen.
* also add default X/Y
* Show window in center of screen (windowed mode)
* Test this build on Windows to make sure it still works.
* Fix the "unrecognized parameter" in the console.
* Turn Random into an instance.

### Backlog
* Change props from preprocessor to "debuggable" props.


# Goals
1. _Multirender Scene_ Draw the same scene on 2 windows.
  * https://community.khronos.org/t/sharing-contexts/73563/3
2. _Double Scene_ Draw 2 scenes on 2 different windows.
  * https://community.khronos.org/t/sharing-contexts/73563/3
2. View skelton + a UI menu in main window. With a UI Button in another window. Do with LUA.
3. Our goal should probably be the Mario game and not the 3D world that we are thinkin bout.
4. GENREN - generic renderer that works for everything
  * Init configurable - specifies which capabilities it has.
  * This reduces buffer footprints
  * Render configurable - Specify which pipe bits to set.
  3
Multithreaded Design
  Async scene  - scnees (physics &c) update async.
  Window -> Updates async
  Render Thread - 1 per context
  Audio - async
  Input - Async.
  5
 -- Good generic WPF reference
 -- https://www.wpf-tutorial.com/panels/dockpanel/
 
# Q's
How to specify camera?  
  
# Tasks
1. Multiple Windows
  A. _Task: UiCentrism_
    * __GUI__
      * Features
        * Purpose of UICentrism is to be able to use a single UI for in-game UI, and game editor UI.
          * The biggest feature is that it allows us to have a desktop UI 
            * comboboxes don't get cutoff.
            * we can move windows around the desktop.
        * Ability to render a single object, or, a scene.
        * Ability to tune the rendering pipeline
          * Turn shadows off. 
          * Turn PP and DOF off.
        * Lua driven
        * UI driven
        * Loose coupling of scene and its rendering
        
      * Considerations
        * Making the UI centric makes it impossible to put UI elements in 3D space, right?
        * We are bifurcating Renderpipe. Reuse renderpipe for UI + Scene?
          * Generic Renderpipe.
        
      * Tests
        * Create GElement in LUA
          * There needs to be lazy initialization with GWindow
          * LUA calls GElement constructor
            * Ok without a window where is the GManager?
            * Lazy call - We will create it in script. That's ok.
            * It only works if its added to a GWindow maybe ?
            
          * initializes new window if one isn't found.
            * if first window, initialize graphics system
            * create the context.
            * create the renderpipe for the context.
            * create the GManager for the window
            * show the window
            
          * shows the window
            *under the hood
            * Initializes new window
            
      * Tasks
        * Phase1 - Adding Phase
          * New system - don't remove stuff yet.
          * Create GManager on Gu. GWindow, GElement, GTitlebar
          * GManager holds the events that UI elements communicate with.
          
        * Phase2 - Breaking Phase
          * Move Ui out of RenderPipe
          * Move renderpipe to GSurface.
          * Change rendering algorithm to GPP (infinite updates -> render)
          
        
        * New G* classes
          * The OPENGL Context owns GManager.
          * GDesktop -- Global UI manager for the editor
            * GWindow
                GStackPanel
                  GTitlebar
                    ::height
                    ::HeightMode : Fixed, Fill (fill will average the heights of all filled items.
                  GRenderTarget
                    ::GCamera
                    ::GRenderer
                      this is RenderPipe
                    ::GScene
              * GDesignSize - hmm for windows we want a native size, but we want the UI to scale properly.
              * GTab - Tabbed windows.
              * GSplitter (rows x cols) -- Ceneric grid container allowing 1, 2, & 4 panes. A grid will work in any way
                * > icon - click+drag 32 pixels away to split window.
                  * right/left - split v, up/down splith
                * 1/2 editor , 1/2 code, etc.
              * GRegion
                * Must be contained in a GSplitter
              * GSurface : GElement -- What gets drawn to as the result of render output
                * Camera* -- REQUIRED - Pointer to the camera
                  * Other cams selected in the GRegion title bar.** How? GRegion may not own a GSurface.
                  * Default: a single flying camera - 1 per scene
                * Scene* -- REQUIRED - pointer to the scene or node to render.
                * RenderPipe -- REQUIRED - the pipeline that renders here.
                * Input -- ?
      * UI texture Classes
        * Invidiaully textured UI elements (ex. GSurface)
        * Color textured Elements (single pixel).
        * Compiled Megatexture UI elements that reference megatex.
        
      * Async UI
        * New Rendering Algorithm.
          * Using the GPP rendering algorithm - infinite world updates until render is done.
          * Update Managers( Physics, Audio)
          * Update the UI
          * Render
            * For each GManager->GSurface
              * Async --> Gather Objects, Cull Shadows.
              * Fence all scenes.
              * Draw UI
              
      * **UI DESIGN**
        * Overview
          * Essentially the goal here is to define everything but the base UiElement, and GSurface, in LUA.
            This gives us fast control over tweaking the user interface
            * editor.lua -- Runs at start to initialize the windowing system
            * ui.lua -- User interface library
          * Note: For speed considerations we should probably keep most Ui Primitives c++.
          * I want the UI to have aniamtions DAMMIT
            * Pop, Slide, Bounce, Fade          
          * We should have onStart, onUpdate and onEnd for all main scripts, even for Editor.lua
        * C++ - GManager.h/cpp
          * Most of this can go into the CPP why do we need it in the engine?
          * class GManager {} -- Manages all window screens per context.
              <GElement> - the windows managed by GManager are really just root UI elements.
          * class GElement {
            int_fast32_t _width = 32; // ignored whe stackpanel v/h
            int_fast32_t _height = 32;               
          } -- Basic building block for elements.
              * 3 modes: 
                * Docked - Docked within a parent GSplitter or other layout container
                * Floating - (position:absolute) floats within the game screen, x,y, width, height are used
                * Window - An OS floating window. It is its own window.
          * class GScreen {} -- The screen container for a window. 
                -- This is the main item that handles the UI for an individual window. This is not relevant to LUA.
          * class GMenu {} -- The List of a GComboBox Dropdown or GMenu * BORDERLESS OS WINDOW * 
              GList is a system level Element because we create a new window in order to display it. Without a new window it would get cut off.
              int maxVisibleItems. Create window, then create UI in the window. When window loses focus - destroy window.
          * class GButton {} -- basic button class. Toggle or Push.
          * class GTextbox {} -- Basic text box / label class. Can be editable or not.
          * class GSplitter {} -- organizes elements into a grid.
              GSplitterCell -- Contains --| a right and bottom partition. if cell is last x, or last y, don't show the partition on that side.
                GDivider - H, V or HV. Click + Drag to resize the parent container.
            <vec2, GRegion> - List of GRegion mapped by xy coordinate
            * Note GSplitter Doubles as a "grid layout" if we were to hide the GDividers
          * class GScene : PhysicsWorld {}
            <GObject> objects
            map<NodeId, GObject>
            * This will replace Scene and make it easier for us to manage objects.
            * No hierarchy. Just a list of objects like used by PhysicsWorld
            * It will suffice our goal of combining the PhysicsWorldwith the scene
          * class GObject {} -- 3D object inside of a GScene
              class GComponent

        * LUA
          GScreen
            GWindow
              GSplitter 
                GWindow -- Shows something (like in blender)
                  GDockPanel - docks to parent container
                  GStackPanel - Stacks the GTitlebar on top of the element below.
                    -- H stack panel ignores width of elements
                    -- V stack panel ignores height of elements

                    * When we RUN the game, we are going to hide the Title bar for our main game window.
                    [[Scene 0 - Camera 0]      ^ - x]  < GToolbar{ GDropdown, GButton, GButton }
                    |                               |   < ^ will pop this window out and create a "Window" window.
                    |         [ GSurface ]          |   < - collapses up (animated, see vs code)
                    |                               |   < x closes
                    ---------------------------------
                    * GWindow dropdwon is a list of the possible views based on the window type. This is similar to Blender.
          
          * GEditorToolbar
            [File > Exit      [>]              - x]
              GMenu           GButton           GButton
            * shit, why not make it borderless and use our own titlebar?
              * we would lose dock functionality.

        * editor.lua (test)
          function onStart(ob)
            --ob is nil here since this script is a system script.
            
            -- Here we are building the main game editor UI
            local win = GWindow(x,y,w,h, titlebar?, view?, parent)
              -- view - whether to show the view.
              -- titlebar - whether to show - we hide this when we create a GMenu
              -- without parent, GWindow becomes a new OS window.
              -- Internally calls GraphicsAPI:createWindow
              -- then initializes the GScreen in the window
              -- then it creates the UIElement that was requested by this function.
                ** The main UI element functions as window too. UGH so confusing.
            local panel = StackPanel() -- Layout containers should be required for non-container elements.
            win:addElement(panel) 
            local titlebar = GTitlebar()
              -- Titlebar should have a fixed height -, x, y, and width are ignored.
            local splitter = GSplitter(1, 2) -- rows x cols        
            panel:addElement(titlebar) -- Should stack on top, and fill
            panel:addElement(splitter)
            
            local scene = GScene() -- Shared Scene class. This is async.
            local sphere = ModelMaker.createSphere(1,10,10) --radius, slices, stacks
                -- create a GameObject
                -- add a MeshComponent
            scene.add(sphere)
            
            -- Some silly stuff. Adding a dynamic closure to an object we just made
            local script = Script(         
              function(ob) end, -- onStart
              function(dt, ob)            --onUpdate 
                ob:rotate(dt)
              end,
              function(ob) end --onExit
            )
            sphere:addScript(script)
            
            
            -- Testing camera angles.
            local left = splitter:get(0,0) -- left col
            local surf1 = GSurface() -- new rendering surface, creates a RenderPipe
            local cam1 = Camera(vec3(10,10,10), vec3(0,0,0), 45.0) -- pos, look, fov
            surf1.setScene(scene)
            surf1.setCamera(cam1)
            left.addElement(surf1)

            local right = splitter:get(1,0) -- right col
            local surf2 = GSurface() -- new rendering surface, creates a RenderPipe
            local cam2 = Camera(vec3(1,10,5), vec3(0,0,0), 45.0) -- pos, look, fov
            surf2.setScene(scene)
            surf2.setCamera(cam2)
            right.addElement(surf2)
          end -- end onStart
          function onUpdate(ob, dt) 
          end
          function onExit(ob) 
          end
          
           
    * __RenderPipe__                            
      * Essentially we're removing the forward step from RenderPipe.
      * Global Picker, or 2 pickers.
      Decouple   _pBlittedForward->init _pPick and pBlittedDeferred->init
        * Same pick buffer can't be used for both steps.
        * Picker uses Deferred Framebuffer.
        * Picker being used for picking in 3D. Also being used for picking in 2D.
        * Decoupling UI from RenderPipe will decouple picker.
        * Need: Global Picker
          * Picks ALL UI elements, 3D elements, etc.
      * Move Renderpipe out of GraphicsWindow, put on GSurface
        * 1 Renderpipe per GSurface (multiple RenderPipe's now)
      * Remove ScreenQuad from RenderPipe
      * Make RenderPipe final output be a Texture2D
      * Move _pForwardShader and endRenderAndBlit out of RenderPipe 
      * GraphcisWindow::updateWidthHeight
        * This will simply update the GScreen w/h then we update the UI dynamically.
          * ... GScreen::updateWidthHeight
            * _pRenderPipe->updateWidthHeight(GScreen::w, GScreen::h)
          
    * __Multiple Scenes__
      * Fix Scene::afterAttachedToWindow
        * Scene update - done async.
        GScene -> AsyncComponent
        AsyncGameComponent::create(name) = 0
          update()
        GScene::create(name) = 0
          [](){
            update()
          }
          
      

    * __Scripting__
      * Create MAIN script for engine configuration.
        * Editor.lua

        * SMW package 
          local scene = Scene()
          local skeledug = Gu.getObject("Skeledug") --get node/model etc.
          scene:add(Skeledug)

    * __Tests__
        1. Shared Contexts 
        1. Multiple Contexts
      a. Child Window 
        1. Shares context
        2. Shares Rendering Pipeline (GBuffer)
        3. Synchronous update.
      b. Parent Window
        1. Separate context, Pipeline.
        2. Asynchronous
      c. XcompleteX Lua performance test - script vec2 vs callback vec2.

  B. _Task: Fix the picker_.
    * - we can only fix this when we confirm that we can reuse the GBuffer for 2 windows. OR  something else.
  
  C. Asyc Update
    * Update is on a separate thread.
    * Render on main thread.
      * This is per window. Yet we need to figure out how to not update scene multiple times.
# Enhancements
* SaveScreenshot MRT buffers opens separate windows at runtime so we can see all of them.
  * Would be easy to do once this is all set up.
  * Move SaveScreenshot to  LUA
* Compile shaders and see errors engine.
  * print to stdout
* Compile LUA and see errors in engine.
* Fix Package Loading
  1. Package is on the .EXE
  2. Package location is supplied by the command line.
    * Package will be extracted to the default extract location (C:/TEMP, std::filesystem::temp_directory_path)
  3. Extracted Package directory supplied by the command line.
  
# Bugs
* Fix Path: Its a scene node so its context isn't fixed. The meshes use a context. TODO: make the meshes components.
* SkyBox: Change to SceneNode with Mesh /atlas componetns.
* Fix CHILD window closing so that it doesn't close the application.



