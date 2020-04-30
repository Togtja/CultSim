local cultlib = {}

-- System prototype for implementing custom Lua Systems
cultlib.LuaSystem = {
    initialize = function() end,
    deinitialize = function() end,
    update = function(dt) end
}

return cultlib
