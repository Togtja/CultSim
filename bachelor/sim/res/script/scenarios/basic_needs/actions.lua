-- Defines actions for the basic needs scenario
-- Actions can be spread into as many files as desired
-- Put all available actions in the actions table

actions.sleep = {
    name = "Sleep",
    requirements = ETag.None,
    time_to_complete = 10.0,
    success = function(owner, target, registry)
        cultsim.modify_need(owner, ETag.Sleep, 69.0)
    end,
    failure = function(owner, target, registry)
        cultsim.modify_need(owner, ETag.Sleep, 5.0)
    end
}

actions.consume_self = {
    name = "Consume Self",
    requirements = ETag.None,
    time_to_complete = 7.5,
    -- You manage to eat yourself without too much damage caused
    success = function(owner, target, registry)
        -- Roll for how much we ate
        local chunk_eaten = random:uniform(5.0, 25.0)
        cultsim.modify_need(owner, ETag.Food, chunk_eaten)

        -- Negatively impact health because we are eating ourselves
        local health = cultsim.get_component(owner, component.health)
        health.health = health.health - chunk_eaten / 2.0
    end,
    -- Take some damage when you fail to swallow yourself
    failure = function(owner, target, registry)
        local health = cultsim.get_component(owner, component.health)
        health = health - 2.0
    end
}

actions.drink = {
    name = "Drink",
    requirements = ETag.Find,
    time_to_complete = 2.0,
    success = function(owner, target, registry)
        cultsim.modify_need(owner, ETag.Drink, 85.0)
    end,
    failure = function(owner, target, registry)
        cultsim.modify_need(owner, ETag.Drink, 5.0)
    end
}
