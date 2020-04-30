-- Defines custom systems
systems = {
    dummy_system = {}
}

-- Set system metatable for all systems
for k,v in pairs(systems) do
    setmetatable(v, {
        __index = cultlib.LuaSystem,
        __tostring = function(table) return k end
    })
end
