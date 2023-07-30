local atl = require "assimp_to_lua" -- make sure the shared library is in the same folder as this file
local util = require "util"

function love.load()
    print("AssimpToLua functions:")
    util.printRecursive(atl)
    
    local fd = love.filesystem.newFileData("animation_with_skeleton.fbx")

    local scene, errmsg = atl.import(fd, {"target_realtime_quality", "validate_data"})
    print()
	if errmsg then
		print(errmsg)
	else
        print("Scene nodes:")
        util.printRecursive(scene.nodes)
		-- print("Imported scene:")
		-- local seen = {}
		-- for i, v in ipairs(scene.nodes) do
		-- 	if i > 1 then
		-- 		seen[v] = true
		-- 	end
		-- end
		-- util.printRecursive(scene, seen)
	end

    love.event.push("quit")
end

