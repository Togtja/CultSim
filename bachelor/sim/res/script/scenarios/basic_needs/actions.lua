-- Defines actions for the basic needs scenario
-- Actions can be spread into as many files as desired
-- Put all available actions in the actions table

actions.sleep = {
    name = "Sleep",
    requirements = ETag.None,
    time_to_complete = 10.0,
    success = function(owner, target, registry) end
}

actions.consume_self = {
    name = "Consume Self",
    requirements = ETag.None,
    time_to_complete = 7.5,
    success = function(owner, target, registry) log.info("Consuming self!") end
}
