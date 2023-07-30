local util = {}

---Writes a table and its contents recursively, ignoring repeated tables.
---@param file any any data structure with a write(self, ...) function
---@param t any
---@param seen table?
---@param tabs number?
function util.writeRecursive(file, t, seen, tabs)
	if not seen then
		seen = {}
	end
	if not tabs then
		tabs = 0
	end
	file:write(tostring(t), "\n")
	if type(t) == "table" and not seen[t] then
		seen[t] = true
		for k, v in pairs(t) do
			for i=0, tabs do file:write("|  ") end
			file:write(tostring(k), ": ")
			util.writeRecursive(file, v, seen, tabs+1)
		end
	end
end

---Prints a table recursively
---@param t table
---@param seen table?
---@param tabs number?
function util.printRecursive(t, seen, tabs)
	util.writeRecursive(io.stdout, t, seen, tabs)
end

---Create a data structure which acts like a file
---but saves a string instead.
function util.newStringFile()
	local sb = {}
	function sb:write(...)
		for i = 1, select("#", ...) do
			sb[#sb+1] = select(i, ...)
		end
	end
	function sb:close()
		if not self._str then
			self._str = table.concat(self)
			for i = #self, 1, -1 do
				self[i] = nil
			end
		end
		return self._str
	end

	return sb
end

return util