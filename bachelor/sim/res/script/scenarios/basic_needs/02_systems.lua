-- Defines custom systems
systems = {
    dummy_system = {
        -- initialize = function() log.error("Initialize Dummy!") end,
        -- deinitialize = function() log.info("De-Initialize Dummy!") end,
        -- update = function(dt) log.info("YO!") end
    }
}

-- Set system metatable for all systems
for k,v in pairs(systems) do
    setmetatable(v, {
        __index = cultlib.LuaSystem,
        __tostring = function(table) return k end
    })
end
