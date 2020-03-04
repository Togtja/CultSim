-- Defines actions for the basic needs scenario
-- Actions can be spread into as many files as desired
-- Put all available actions in the actions table

actions.sleep = Action:new()

actions.sleep.name = "Sleep"

actions.sleep.requirements = Tag.None

actions.sleep.time_to_complete = random.uniform(9.0, 11.0)

actions.sleep.success = function(owner, target, registry)

end
