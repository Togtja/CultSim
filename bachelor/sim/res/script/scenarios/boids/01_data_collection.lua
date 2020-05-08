-- Defines custom data collectors
data_collectors = {
    velocity_collector = DataCollector.new("Avg Velocity", {
        execute = function(dt)
            local view = cultsim.view({component.lua1})
            local sum_squared = 0.0

            view:each(function(e)
                local boid = cultsim.get_component(e, component.lua1)
                sum_squared = sum_squared + (boid.velocity.x * boid.velocity.x + boid.velocity.y + boid.velocity.y)
            end)

            sum_squared = math.sqrt(sum_squared)
            return sum_squared
        end
    })
}
