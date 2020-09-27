io.write(string.format("Hello from %s\n", _VERSION))

--Error

--Call C++ function
avg, sum = average(10, 20, 30, 40, 50)
print("The average is ", avg)
print("The sum is ", sum)

--Our Framework
function onStart( data )
  print("OnStart called from LUA data='" .. data .. "'");
end
function onUpdate(data)
  print("OnUpdate called from LUA data='" .. data .. "'");
end
function onExit(data)
  print("OnExit called from LUA data='" .. data .. "'");
end