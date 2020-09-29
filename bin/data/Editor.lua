print("Lua: Creating Editor")

local win = Gu.getGraphicsApi():createWindow(GraphicsWindowCreateParameters("Main Window", 
100, 100, 800, 600, GraphicsWindowCreateParameters.Wintype_Desktop,
false, true, false,
nil
))
local win_child = Gu.getGraphicsApi():createWindow(GraphicsWindowCreateParameters("Child Window",
100, 100, 200, 500,  GraphicsWindowCreateParameters.Wintype_Utility,
false, true, false,
win
))
local file = "nature_0.ogg"
--This is causing "bad arg" errors although it is correct.
local sound = Gu.playSound(file, SoundPlayInfo(true, 0, 1))
print("Lua: Done")
