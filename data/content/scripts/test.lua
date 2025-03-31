local functions = {
   function()
      print("Sanity test")

      if (1 + 1) == 2 then
	 return true
      end

      return false
   end,
   function()
      print("IsA test")

      print("testing IsA ScriptInstance")
      if not script:IsA("ScriptInstance") then
	 return false
      end

      print("testing IsA Instance")
      if not script:IsA("Instance") then
	 return false
      end

      print("testing IsA ModelInstance")
      if script:IsA("ModelInstance") then
	 return false
      end

      return true
   end,
   function()
      print("FindFirstChild test")

      print("testing game.Workspace")
      if not game:FindFirstChild("Workspace") then
	 return false
      end

      print("testing non existent instance")
      if game:FindFirstChild("COFFEE") then
	 return false
      end

      return true
   end,
   function()
      print("Vector3 test")

      print("testing Vector3.new")
      print("testing no arguments")
      local v = Vector3.new()
      print(v)
      if not (v.x == 0.0 and v.y == 0.0 and v.z == 0.0) then
	 return false
      end
      print("testing 1 argument")
      v = Vector3.new(1.0)
      print(v)
      if not (v.x == 1.0 and v.y == 1.0 and v.z == 1.0) then
	 return false
      end
      print("testing 3 arguments")
      v = Vector3.new(1.0, 2.0, 3.0)
      print(v)
      if not (v.x == 1.0 and v.y == 2.0 and v.z == 3.0) then
	 return false
      end
      
      return true
   end,
}

local testsDone = 0
local testsPassed = 0
for i = 1, #functions do
   if not functions[i]() then
      print("FAILED TEST " .. i)
   else 
      print("PASSED TEST " .. i)
      testsPassed = testsPassed + 1
   end
   testsDone = testsDone + 1
end
print("Did " .. testsDone .. " tests, passed " .. testsPassed .. " (" .. (testsPassed / testsDone) * 100 .. "%)")
