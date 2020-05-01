-- Defines custom systems
systems = {
    boid_system = {
        update = function(dt)            
            local view = cultsim.view({component.position})
            view:each(function(e)
                local avg_pos = Vec3.new(0, 0, 0)
                local avg_vel = Vec2.new(0, 0)
                local out_vel = Vec2.new(0, 0)

                avg_pos.x = avg_pos.x + cultsim.get_component(e, component.position).position.x
                avg_pos.y = avg_pos.y + cultsim.get_component(e, component.position).position.y

                out_vel.x = avg_pos.x / view:size()
                out_vel.y = avg_pos.y / view:size();

                local boid = cultsim.get_component(e, component.lua1)
                boid.velocity = out_vel
            end)
        end
    },
    move_system = {
        update = function(dt)
            local components = {component.position, component.lua1}
            local view = cultsim.view(components)

            view:each(function(e)
                local position = cultsim.get_component(e, component.position)
                local boid = cultsim.get_component(e, component.lua1)

                position.position.x = position.position.x + (boid.speed * boid.velocity.x * dt)
                position.position.y = position.position.y + (boid.speed * boid.velocity.y * dt)
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
