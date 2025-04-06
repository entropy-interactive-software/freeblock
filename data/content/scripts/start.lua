game:GetService("RunService"):Start()

game.Workspace.InfinitePlane = true
local samples = 25
local phi = math.pi * (math.sqrt(5) - 1)
for i = 1, samples do
   local y = 1 - (i / (samples - 1)) * 2
   radius = math.sqrt(1 - y * y)
   theta = phi * i
   local x = math.cos(theta) * radius
   local z = math.sin(theta) * radius
   
   local block = Instance.new("BlockInstance", game.Workspace)
   block.Position = (Vector3.new(x, y, z) * 10) + Vector3.new(0, 15, 0)
   block.Size = Vector3.new(1)
   block.Anchored = false
end
   
print("Created parts")
