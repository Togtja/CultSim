-- Create the scenario that will be loaded
scenario = Scenario:new()

-- Give the scenario a name
scenario.name = "Boids"

-- Describe the scenario with a sensible amount of text
scenario.description = "Look at flocking behavior in real time"

-- The bounds of the level on the X and Y in both directions based on the center 200,200 means the total world is 400,400 with 200 units on both sides of 0,0
scenario.bounds = Vec2:new(100.0, 100.0)

-- Declare all systems that should be active initially
scenario.systems = {
    "SensorSystem",
    systems.boid_system,
    systems.move_system,
    "DeletionSystem",
}

-- Decides how many seconds between each data sample taken in this scenario. IE: (also accuracy / granularity of graphs and output)
scenario.sampling_rate = 5.0;

-- This function is called before starting the simulation, once
scenario.init = function()
    -- Spawn Boids
    for i=1,10 do
        local deer = cultsim.spawn("boid")        
    end
end

-- This function is called every frame of the simulation
scenario.update = function(dt) end

-- This function is called during the draw phase of the update
scenario.draw = function() end

-- This function is called before finishing the scenario
scenario.shutdown = function() end
