-- Create the scenario that will be loaded
scenario = Scenario:new()

-- Give the scenario a name
scenario.name = "Basic Goals"

-- Describe the scenario with a sensible amount of text
scenario.description = "Observe as your agents walk around in a forest looking for food. Now with extra GOB"

-- The bounds of the level on the X and Y in both directions based on the center 200,200 means the total world is 400,400 with 200 units on both sides of 0,0
scenario.bounds = Vec2:new(500.0, 500.0)

-- Declare all systems that should be active initially
scenario.systems = {
    "SensorSystem",
    "GoalSystem",
    "GOBSystem",
    "GOBBASystem",
    "TimerSystem",
    "MovementSystem",
    "DeletionSystem"
}

-- Decides how many seconds between each data sample taken in this scenario. IE: (also accuracy / granularity of graphs and output)
scenario.sampling_rate = 1.0;

-- This function is called before starting the simulation, once
scenario.init = function()

    -- Spawn 100 Deer
    for i=1,100 do
        local deer = cultsim.spawn("deer")

        -- -- Randomly distribute starting needs of agents around 75, with a std deviation of 25
       
        local name = cultsim.get_component(deer, component.name)
        local genname = cultsim.generate_name("germanic", true)
        name.name = string.format("%s %s", genname.first, genname.last)
    end

    -- Spawn 75 Food Sources and Water Sources
    for i=1,75 do
        cultsim.spawn("food_source")
    end

    -- Create some food / water initially
    for i=1,50 do
        cultsim.spawn_at("food", Vec2:new(random:normal(0.0, 70.0), random:normal(0.0, 70.0)))
    end

end

-- This function is called every frame of the simulation
scenario.update = function(dt) end

-- This function is called during the draw phase of the update
scenario.draw = function() end

-- This function is called before finishing the scenario
scenario.shutdown = function() end
