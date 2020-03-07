-- Create the scenario that will be loaded
scenario = Scenario:new()

-- Give the scenario a name
scenario.name = "Basic Needs"

-- Describe the scenario with a sensible amount of text
scenario.description = "Observe as your agents walk around in a forest looking for food and water to survive. How many can be sustained?"

-- Decide how many agents should be used
scenario.agent_count = 100

-- The bounds of the level on the X and Y in both directions based on the center 200,200 means the total world is 400,400 with 200 units on both sides of 0,0
scenario.bounds = Vec2:new(500.0, 500.0)

-- Declare all systems that should be active initially
scenario.systems = {
    "NeedSystem",
    "HealthSystem",
    "ReproductionSystem",
    "MitigationSystem",
    "ActionSystem",
    "RequirementSystem",
    "TimerSystem",
    "AiSystem",
    "MovementSystem",
    "RenderingSystem"
}

-- Decides how many seconds between each data sample taken in this scenario. IE: (also accuracy / granularity of graphs and output)
scenario.sampling_rate = 1.0;

-- This function is called before starting the simulation, once
scenario.init = function() log.info("Starting scenario " .. scenario.name .. "!") end

-- This function is called every frame of the simulation
scenario.update = function(dt) end

-- This function is called during the draw phase of the update
scenario.draw = function() end

-- This function is called before finishing the scenario
scenario.shutdown = function() end
