----------------------------------------------
-- @file test_math.lua
-- @purpose Speed test scripted Lua math classes vs C++ callbacks.
--          This test sought having a "lua library" may prove faster than calling back into c++.
-- @result (GCC) Br2::vec3 50% faster for simple arithmetic in release mode.
----------------------------------------------
function getType(b)
  local type = type(b)
  if type == "table" then
    --Our builtin system uses table length shortcut for class names
    return #b
  else
    return type
  end
end
lvec4 = {x = 0, y = 0, z = 0, w = 0}
function lvec4:new(dx, dy, dz, dw)
  local o = {}
  setmetatable(o, self)
  self.__index = self
  self.__len = function()
    --hack # to return name
    return "lvec4"
  end
  o.x = dx or 0
  o.y = dy or 0
  o.z = dz or 0
  o.w = dw or 0
  return o
end
lvec2 = {x = 0, y = 0}
function lvec2:new(dx, dy)
  local o = {}
  setmetatable(o, self)
  self.__index = self
  self.__len = function()
    --hack # to return name
    return "lvec2"
  end
  o.x = dx or 0
  o.y = dy or 0
  return o
end
lvec3 = {x = 0, y = 0, z = 0}
function lvec3:new(dx, dy, dz)
  local o = {}
  setmetatable(o, self)
  self.__index = self
  self.__len = function()
    --hack # to return name
    return "lvec3"
  end
  self.__add = function(a, b)
    r = lvec3:new(0, 0, 0)
    if type(b) == "number" then
      r.x = a.x + b
      r.y = a.y + b
      r.z = a.z + b
    elseif type(b) == "table" and #b == #a then
      r.x = a.x + b.x
      r.y = a.y + b.y
      r.z = a.z + b.z
    else
      error("Type mismatch for " .. #a .. ".__add types = " .. #a .. "," .. getType(b) .. "\n" .. debug.traceback())
    end
    return r
  end
  self.__sub = function(a, b)
    r = lvec3:new(0, 0, 0)
    if type(b) == "number" then
      r.x = a.x - b
      r.y = a.y - b
      r.z = a.z - b
    elseif type(b) == "table" and #b == #a then
      r.x = a.x - b.x
      r.y = a.y - b.y
      r.z = a.z - b.z
    else
      error("Type mismatch for " .. #a .. ".__sub types = " .. #a .. "," .. getType(b) .. "\n" .. debug.traceback())
    end
    return r
  end
  self.__mul = function(a, b)
    r = lvec3:new(0, 0, 0)
    if type(b) == "number" then
      r.x = a.x * b
      r.y = a.y * b
      r.z = a.z * b
    elseif type(b) == "table" and #b == #a then
      r.x = a.x * b.x
      r.y = a.y * b.y
      r.z = a.z * b.z
    else
      error("Type mismatch for " .. #a .. ".__mul types = " .. #a .. "," .. getType(b) .. "\n" .. debug.traceback())
    end
    return r
  end
  self.__div = function(a, b)
    r = lvec3:new(0, 0, 0)
    if type(b) == "number" then
      r.x = a.x / b
      r.y = a.y / b
      r.z = a.z / b
    elseif type(b) == "table" and #b == #a then
      r.x = a.x / b.x
      r.y = a.y / b.y
      r.z = a.z / b.z
    else
      error("Type mismatch for " .. #a .. ".__div types = " .. #a .. "," .. getType(b) .. "\n" .. debug.traceback())
    end
    return r
  end

  o.x = dx or 0
  o.y = dy or 0
  o.z = dz or 0

  return o
end
function lvec3:dot(rhs)
  local r = (self.x * rhs.x) + (self.y * rhs.y) + (self.z * rhs.z)
  return r
end
function lvec3:length()
  local r = math.sqrt(self:dot(self))
  return r
end
function lvec3:distanceTo(rhs)
  local r = (rhs - self):length()
  return r
end
function lvec3:toString()
  return self.x .. "," .. self.y .. "," .. self.z
end
function lvec3:cross(rhs)
  local r = lvec3:new()
  r.x = (self.y * rhs.z) - (rhs.y * self.z)
  r.y = (self.z * rhs.x) - (rhs.z * self.x)
  r.z = (self.x * rhs.y) - (rhs.x * self.y)
  return r
end

print("Lua (lvec3) and BR2 (vec3) impl test.")
local v0 = lvec3:new(1, 2, 3)
local v1 = lvec3:new(4, 5, 6)
local v4_wrong = lvec4:new(7, 8, 9, 10)
print("lvec3 v0 toString = " .. v0:toString())
print("lvec3 v1 toString = " .. v1:toString())
print("v0 + v1 = " .. (v0 + v1):toString())
print("v0 - v1 = " .. (v0 - v1):toString())
--print("v0 + func = " .. (v0 / function() end):toString()) -- Failure case - fails correctly.
print("v0 * v1 = " .. (v0 * v1):toString())
print("v0 / v1 = " .. (v0 / v1):toString())
print("v0 . v1 = " .. (v0:dot(v1)))
print("v0 + 10 = " .. (v0 + 10.0):toString())
print("v0 - 10 = " .. (v0 - 10.0):toString())
print("v0 * 10 = " .. (v0 * 10.0):toString())
print("v0 / 10 = " .. (v0 / 10.0):toString())
print("distance = " .. (v0:distanceTo(v1)))
local v5 = vec3(1, 2, 3)
local v6 = vec3(4, 5, 6)
print("vec3 v5 toString() = " .. v5:toString())
print("vec3 v6 toString() = " .. v6:toString())
print("v5 + v6 = " .. (v5 + v6):toString())
print("v5 - v6 = " .. (v5 - v6):toString())
print("v5 * v6 = " .. (v5 * v6):toString())
print("v5 / v6 = " .. (v5 / v6):toString())
print("v5 . v6 = " .. (v5:dot(v6)))
print("v5 + 10 = " .. (v5:adds(10)):toString())
print("v5 - 10 = " .. (v5:subs(10)):toString())
print("v5 * 10 = " .. (v5:muls(10)):toString())
print("v5 / 10 = " .. (v5:divs(10)):toString())
print("distance = " .. (v5:distanceTo(v6)))

---

local count = 100000
local avgDist
local sw
local fixedSeed = 1206905993
local rando = Random(fixedSeed)
local a_t, b_t

---

print("Addition test")
avgDist = 0
sw = Stopwatch()
sw:start()
rando:setSeed(fixedSeed)
local lua_v3 = lvec3:new(0, 0, 0)
for i = 0, count, 1 do
  local v0 = lvec3:new(rando:nextFloat(-10, 10), rando:nextFloat(-10, 10), rando:nextFloat(-10, 10))
  local v1 = lvec3:new(rando:nextFloat(-10, 10), rando:nextFloat(-10, 10), rando:nextFloat(-10, 10))
  local v2 = v0 + v1
  lua_v3 = (lua_v3 + v2) / 2
end
sw:stop()
-- Make sure the results are the same for both tests
a_t = sw:deltaMilliseconds() / 1000
print("Lua vec3 add = " .. a_t .. "s --> value = " .. lua_v3:toString() .. " --> dist = " .. avgDist)
avgDist = 0
sw = Stopwatch()
sw:start()
rando:setSeed(fixedSeed)
local br_v3 = vec3(0, 0, 0)
for i = 0, count, 1 do
  local v0 = vec3(rando:nextFloat(-10, 10), rando:nextFloat(-10, 10), rando:nextFloat(-10, 10))
  local v1 = vec3(rando:nextFloat(-10, 10), rando:nextFloat(-10, 10), rando:nextFloat(-10, 10))
  local v2 = v0 + v1
  br_v3 = (br_v3 + v2):divs(2)
end
sw:stop()
-- Make sure the results are the same for both tests
b_t = sw:deltaMilliseconds() / 1000
print("BR2 vec3 add = " .. b_t .. "s --> value = " .. br_v3:toString(14) .. " --> dist = " .. avgDist)
print("" .. (a_t - b_t) / a_t * 100 .. "% faster")

--
print("Cross Product test")
avgDist = 0
sw = Stopwatch()
sw:start()
rando:setSeed(fixedSeed)
local lua_v3 = lvec3:new(0, 0, 0)
for i = 0, count, 1 do
  local v0 = lvec3:new(rando:nextFloat(-10, 10), rando:nextFloat(-10, 10), rando:nextFloat(-10, 10))
  local v1 = lvec3:new(rando:nextFloat(-10, 10), rando:nextFloat(-10, 10), rando:nextFloat(-10, 10))
  local v2 = v0:cross(v1)
  lua_v3 = (lua_v3 + v2) / (2)
end
sw:stop()
-- Make sure the results are the same for both tests
a_t = sw:deltaMilliseconds() / 1000
print("Lua vec3 cross = " .. a_t .. "s  --> value = " .. lua_v3:toString() .. " --> dist = " .. avgDist)
avgDist = 0
sw = Stopwatch()
sw:start()
rando:setSeed(fixedSeed)
local br_v3 = vec3(0, 0, 0)
for i = 0, count, 1 do
  local v0 = vec3(rando:nextFloat(-10, 10), rando:nextFloat(-10, 10), rando:nextFloat(-10, 10))
  local v1 = vec3(rando:nextFloat(-10, 10), rando:nextFloat(-10, 10), rando:nextFloat(-10, 10))
  local v2 = v0:cross(v1)
  br_v3 = (br_v3 + v2):divs(2)
end
sw:stop()
-- Make sure the results are the same for both tests
b_t = sw:deltaMilliseconds() / 1000
print("BR2 vec3 cross = " .. b_t .. "s --> value = " .. br_v3:toString(14) .. " --> dist = " .. avgDist)
print("" .. (a_t - b_t) / a_t * 100 .. "% faster")

print("done..")