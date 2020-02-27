
scenario = Scenario:new()
scenario:name = "Basic Needs"
scenario:description = "Observe as your agents walk around in a forest looking for food and water to survive. How many can be sustained?"
scenario:agent_count = 100
scenario:init = function() log.info("Starting scenario " .. scenario:name .. "!")
