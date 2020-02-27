
scenario = Scenario:new()
scenario.name = "Basic Needs"
scenario.description = "Observe as your agents walk around in a forest looking for food and water to survive. How many can be sustained?"
scenario.agent_count = 1
scenario.init = function() log.info("Starting scenario " .. scenario.name .. "!") end
scenario.update = function(dt) end
scenario.draw = function() end
scenario.shutdown = function() end
