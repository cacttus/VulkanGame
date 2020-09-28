
vec2 = { x=0, y=0 }
function vec2:new (dx, dy)
  local o = {}
  setmetatable(o, self)
  self.__index = self
  self.__add = function(a, b)
    z = vec2:new(nil,0,0)
    z.x = a.x + b.x
    z.y = a.y + b.y
    return z
  end
  self.__sub = function(a, b)
    z = vec2:new(nil,0,0)
    z.x = a.x - b.x
    z.y = a.y - b.y
    return z
  end
  self.__mul = function(a, b)
    z = vec2:new(nil,0,0)
    z.x = a.x * b.x
    z.y = a.y * b.y
    return z
  end
  self.__div = function(a, b)
    z = vec2:new(nil,0,0)
    z.x = a.x / b.x
    z.y = a.y / b.y
    return z
  end

  o.x = dx or 0;
  o.y = dy or 0;

  return o
end
function vec2:dot(rhs)
  local r = self.x*rhs.x + self.y*rhs.y
  return r
end
function vec2:length()
  local r = math.sqrt(self:dot(self))
  return r
end
function vec2:distanceTo(rhs)
  local r = (rhs - self):length()
  return r
end
function vec2:toString()
  return "(" .. self.x .."," .. self.y .. ")"
end

local v0 = vec2:new(1,2)
local v2 = vec2:new(3,4)
print("v0 = " .. v0:toString())
print("v2 = " .. v2:toString())

print("v + v2 = " .. (v0 + v2):toString())
print("v * v2 = " .. (v0 * v2):toString())
print("v . v2 = " .. (v0:dot(v2)))
print("distance = " .. (v0:distanceTo(v2)))

local v3 = vec2:new(5,0)
local v4 = vec2:new(10,0)
print("v0 = " .. v3:toString())
print("v2 = " .. v4:toString())
print("distance = " .. (v3:distanceTo(v4)) .. " should be 5")



--v3 = v + v2

--result = v:toString()

--print("result=" .. result)

-- io.write(string.format("Hello from %s\n", _VERSION))
-- 
-- --Error
-- 
-- --Call C++ function
-- avg, sum = average(10, 20, 30, 40, 50)
-- print("The average is ", avg)
-- print("The sum is ", sum)
-- 
-- --Our Framework
function onStart( data )
  print("OnStart called from LUA data='" .. data .. "'");
end
function onUpdate(data)
  print("OnUpdate called from LUA data='" .. data .. "'");
end
function onExit(data)
  print("OnExit called from LUA data='" .. data .. "'");
end