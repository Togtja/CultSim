-- Defines custom data collectors
data_collectors = {
    velocity_collector = DataCollector.new("Position X", {
        accum = 0.0,
        execute = function(self)
            local view = cultsim.view({component.position})
            local sum_x = 0.0

            view:each(function(e)
                local boid = cultsim.get_component(e, component.position)
                sum_x = sum_x + boid.position.x
            end)

            return sum_x / view:size()
        end
    })
}
