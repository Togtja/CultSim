-- Defines custom systems
systems = {
    boid_system = {
        update = function(dt)            
            local view = cultsim.view({component.position, component.lua1})
            
            local avg_pos = Vec3.new(0, 0, 0)
            local avg_vel = Vec2.new(0, 0)
            local out_vel = Vec2.new(0, 0)

            -- Sum all boid velocities and positions
            view:each(function(e)
                avg_pos.x = avg_pos.x + cultsim.get_component(e, component.position).position.x
                avg_pos.y = avg_pos.y + cultsim.get_component(e, component.position).position.y

                avg_vel.x = avg_vel.x + cultsim.get_component(e, component.lua1).velocity.x
                avg_vel.y = avg_vel.y + cultsim.get_component(e, component.lua1).velocity.y
            end)

            -- Compute averages for positions and velocities
            avg_pos.x = avg_pos.x / view:size()
            avg_pos.y = avg_pos.y / view:size()

            avg_vel.x = avg_vel.x / view:size()
            avg_vel.y = avg_vel.y / view:size()

            -- Set velocity to average
            view:each(function(e)
                local boid_comp = cultsim.get_component(e, component.lua1)
                local position = cultsim.get_component(e, component.position)
                
                boid_comp.velocity.x = boid_comp.velocity.x + (dt * (avg_vel.x + (avg_pos.x - position.position.x)) / 2)
                boid_comp.velocity.y = boid_comp.velocity.y + (dt * (avg_vel.y + (avg_pos.y - position.position.y)) / 2)
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
