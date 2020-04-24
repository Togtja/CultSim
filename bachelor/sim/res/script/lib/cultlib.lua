local cultlib = {
    -- System prototype for implementing custom Lua Systems
    System = {
        initialize = function() end,
        deinitialize = function() end,
        update = function(dt) end,
        name = "LuaSystem"
    },

    SystemMetatable = {
        __index = function(_, key) log.warn("System access") return System.name end,
        __tostring = function(table) return table.name end
    },

    -- Create a new system
    create_system = function(name, data)
        data[name] = name
        setmetatable(data, SystemMetatable)
        return data
    end

}

return cultlib
