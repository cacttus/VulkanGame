print("Lua: Creating Editor")

--If no parent is supplied this creates a new OS window when shown.
-- local e = GWindow(100,100,100,100)
--   GDesk.add(e)
-- local e2 = GElement(200,200,100,100)
--   GDesk.add(e2)
-- local e3 = GSurface(300,300,100,100)
--   GDesk.add(e3)
-- local scene = GScene()
-- local sphere = ModelMaker.createSphere(1,10,10) --radius, slices, stacks
-- scene.add(sphere)
-- e3.setScene(scene)

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
