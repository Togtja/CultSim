-- Defines actions for the basic needs scenario
-- Actions can be spread into as many files as desired
-- Put all available actions in the actions table

actions.eat = {
    name = "Eat",
    requirements = ETag.Find,
    time_to_complete = 5.0,
    success_chance = 0.9,
    success = function(owner, target)
        cultsim.modify_need(owner, ETag.Food, 60.0)
        cultsim.kill(target)
    end,
    failure = function(owner, target)
        cultsim.modify_need(owner, ETag.Food, 10.0)
    end,
    abort = function(owner, target)
        local tag_component = cultsim.get_component(owner, component.tag)
    end
}
