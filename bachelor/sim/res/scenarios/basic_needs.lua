-- Create the scenario that will be loaded
scenario = Scenario:new()

-- Give the scenario a name
scenario.name = "Basic Needs"

-- Describe the scenario with a sensible amount of text
scenario.description = "Observe as your agents walk around in a forest looking for food and water to survive. How many can be sustained?"

-- Decide how many agents should be used
scenario.agent_count = 1

-- Declare all systems that should be active initially
scenario.systems = { "ActionSystem", "MovementSystem" }

-- This function is called before starting the simulation, once
scenario.init = function() log.info("Starting scenario " .. scenario.name .. "!") end

-- This function is called every frame of the simulation
scenario.update = function(dt) end

-- This function is called before finishing the scenario
scenario.shutdown = function() end
