-- Defines actions for the basic needs scenario
-- Actions can be spread into as many files as desired
-- Put all available actions in the actions table

actions.sleep = {
    name = "Sleep",
    requirements = Tag.None,
    tags = Tag.None,
    time_to_complete = 10.0,
    success = function(owner, target, registry) end,
    failure = function(target, registry) end
}

actions.consume_self = {
    name = "Consume Self",
    requirements = Tag.None,
    tags = Tag.None,
    time_to_complete = 7.5,
    success = function(owner, target, registry)
        modify_need(owner, Tag.Food, 40.0)
        modify_health(owner, random:uniform(-25.0, 0.0))
    end,
    failure = function(target, registry)
        modify_health(target, random:uniform(-40.0, -25.0))
    end
}
