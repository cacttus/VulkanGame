print("Lua: Creating Editor")

local win = Gu.getGraphicsApi():createWindow(GraphicsWindowCreateParameters("Main Window", 
100, 100, 800, 600, GraphicsWindowCreateParameters.Wintype_Desktop,
false, true, false,
nil
))
-- config.w, config.h, config.fs, true, config.ar, nil)
--local ui = Gu.getUiManager()


-- const string_t& title, int32_t x, int32_t y, int32_t width, int32_t height,
-- bool fullscreen, bool show, bool forceAspectRatio,
-- bool closeButton, bool minButton, max,  
--bool resizable, bool border,
-- std::shared_ptr<GraphicsWindow> parent = nullptr


local win_child = Gu.getGraphicsApi():createWindow(GraphicsWindowCreateParameters("Child Window",
100, 100, 200, 500,  GraphicsWindowCreateParameters.Wintype_Utility,
false, true, false,
win
))
local file = "nature_0.ogg"
--Gu.playSound(file, SoundPlayInfo(true, 0, 1))
print("Lua: Done")
