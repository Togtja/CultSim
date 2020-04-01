-- Create the scenario that will be loaded
scenario = Scenario:new()

-- Give the scenario a name
scenario.name = "Basic Needs"

-- Describe the scenario with a sensible amount of text
scenario.description = "Observe as your agents walk around in a forest looking for food and water to survive. How many can be sustained?"

-- The bounds of the level on the X and Y in both directions based on the center 200,200 means the total world is 400,400 with 200 units on both sides of 0,0
scenario.bounds = Vec2:new(500.0, 500.0)

-- Declare all systems that should be active initially
scenario.systems = {
    "SensorSystem",
    "NeedSystem",
    "ReproductionSystem",
    "MitigationSystem",
    "ActionSystem",
    "RequirementSystem",
    "MemorySystem",
    "TimerSystem",
    "MovementSystem",
    "InventorySystem",
    "HealthSystem",
    "DeletionSystem"
}

-- Decides how many seconds between each data sample taken in this scenario. IE: (also accuracy / granularity of graphs and output)
scenario.sampling_rate = 1.0;

-- This function is called before starting the simulation, once
scenario.init = function()
    cultsim.connect("ScenarioLoaded", function(event) log.info("Scenario loaded.") end)

    -- Spawn 100 Deer
    for i=1,100 do
        local deer = cultsim.spawn("deer")

        -- -- Randomly distribute starting needs of agents around 75, with a std deviation of 25
        local need_comp = cultsim.get_component(deer, component.need)
        for i, need in ipairs(need_comp.required_needs) do
            need.status = random:normal(75.0, 25.0)
        end
    end

    -- Spawn 75 Food Sources and Water Sources
    for i=1,75 do
        cultsim.spawn("food_source")
        cultsim.spawn("water_source")
    end

    -- Create some food / water initially
    for i=1,50 do
        cultsim.spawn_at("water", Vec2:new(random:normal(0.0, 70.0), random:normal(0.0, 70.0)))
        cultsim.spawn_at("food", Vec2:new(random:normal(0.0, 70.0), random:normal(0.0, 70.0)))
    end
end

-- This function is called every frame of the simulation
scenario.update = function(dt) end

-- This function is called during the draw phase of the update
scenario.draw = function() end

-- This function is called before finishing the scenario
scenario.shutdown = function() end
