-- Defines custom systems
systems = {
    boid_system = {
        update = function(dt)
            local avg_pos = Vec3.new(0, 0, 0)
            local avg_vel = Vec2.new(0, 0, 0)

            local components = {component.position, component.lua1}
            local view = cultsim.view(components)


        end
    },
    move_system = {
        update = function(dt)
            local components = {component.position, component.lua1}
            local view = cultsim.view(components)

            view:each(function(e)
                local position = cultsim.get_component(e, component.position)
                local boid = cultsim.get_component(e, component.lua1)

                position.position.x = position.position.x + boid.speed * dt
            end)
        end
    }
}

-- Set system metatable for all systems
for k,v in pairs(systems) do
    setmetatable(v, {
        __index = cultlib.LuaSystem,
        __tostring = function(table) return k end
    })
end
