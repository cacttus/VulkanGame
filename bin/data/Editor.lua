--local config = Gu.getEngineConfig()

print("Lua: Running Engine.lua")

local params;
local gapi = Gu.getGraphicsApi()
params = GraphicsWindowCreateParameters("Main Window", 800, 600, false, true, false)
local win = gapi:createWindow(params)
 -- config.w, config.h, config.fs, true, config.ar, nil)
-- Note the system gets initialized in window now, no need for "dummy" window.
--local ui = Gu.getUiManager()
params = GraphicsWindowCreateParameters("Child Window", 200, 500, false, true, false, win)
local win_child = gapi:createWindow(params)
print("Lua: Done")
