-- Defines actions for the basic needs scenario
-- Actions can be spread into as many files as desired
-- Put all available actions in the actions table

actions.sleep = {
    name = "Sleep",
    requirements = ETag.None,
    time_to_complete = 10.0,
    success_chance = 0.9,
    targets_self = true,
    success = function(owner, target)
        cultsim.modify_need(owner, ETag.Sleep, 69.0)
    end,
    failure = function(owner, target)
        cultsim.modify_need(owner, ETag.Sleep, 5.0)
    end
}

actions.consume_self = {
    name = "Consume Self",
    requirements = ETag.None,
    time_to_complete = 7.5,
    -- You manage to eat yourself without too much damage caused
    success_chance = 0.9,
    targets_self = true,
    success = function(owner, target)
        -- Roll for how much we ate
        local chunk_eaten = random:uniform(15.0, 45.0)
        cultsim.modify_need(owner, ETag.Food, chunk_eaten)

        -- Negatively impact health because we are eating ourselves
        cultsim.apply_basic_damage(owner, chunk_eaten / 2.0)
    end,
    -- Take some damage when you fail to swallow yourself
    failure = function(owner, target)
        cultsim.apply_basic_damage(owner, 3.0)
    end

}

actions.drink = {
    name = "Drink",
    requirements = ETag.Find,
    time_to_complete = 2.0,
    success_chance = 0.9,
    success = function(owner, target)
        cultsim.modify_need(owner, ETag.Drink, 85.0)
        cultsim.kill(target)
    end,
    failure = function(owner, target)
        cultsim.modify_need(owner, ETag.Drink, 5.0)
    end,
    abort = function(owner, target)
        local tag_component = cultsim.get_component(owner, component.tag)
    end
}

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

actions.gather_food = {
    name = "Forage",
    requirements = ETag.Find,
    time_to_complete = 2.0,
    success_chance = 0.9,
    success = function(owner,target)
    
    if cultsim.get_component(target, component.tag).tags & ETag.Inventory ~= 0  then
      log.info("I (" .. target .. ") with tags " ..cultsim.get_component(target, component.tag).tags .." could not be picked up.")
        return
        end   

    log.info("I (" .. owner .. ") picked up " .. target .. ".")
    cultsim.add_to_inventory(owner,target)
    cultsim.remove_component(target, component.position)

    end,
    failure = function(owner, target)
    log.info("I (" .. owner .. ") failed to pick up " .. target .. ".")
    end,
    abort = function(owner, target)
    end
}

actions.eat_from_inventory = {
    name = "Eat from inventory",
    requirements = ETag.Inventory,
    time_to_complete = 5.0,
    success_chance = 0.9,
    success = function(owner,target)
        log.info("I (" .. owner .. ") ate ".. target .." from my backpack.")
        cultsim.modify_need(owner, ETag.Food, 60.0)
        cultsim.remove_from_inventory(owner,target)
        cultsim.kill(target)
    end,
    failure = function(owner,target)
    end,
    abort = function(owner,target)
    end
}

actions.reproduce = {
    name = "Reproducing",
    requirements = ETag.Find | ETag.Tag,
    time_to_complete = 5.0,
    success_chance = 0.9,
    success = function(owner, target)
        log.warn(owner.." succeeded to have sex with " .. target)

        if not (cultsim.is_valid(owner) and cultsim.is_valid(target)) then 
            return
        end

        local my_reproduction = cultsim.get_component(owner, component.reproduction)
        local their_reproduction = cultsim.get_component(target, component.reproduction)

        local tag_component = cultsim.get_component(owner, component.tag)
        local their_tags = cultsim.get_component(target,component.tag)
        tag_component.tags = tag_component.tags & ~(ETag.Reproduce)
        their_tags.tags = their_tags.tags & ~(ETag.Reproduce)

        -- Increase need
        cultsim.modify_need(owner, ETag.Reproduce, 99.0)
        cultsim.modify_need(target, ETag.Reproduce,99.0)



        -- If we have different sexes and we're the female, then we're impregnated (currently 100% chance)
        if my_reproduction.sex == ESex.Female and their_reproduction.sex == ESex.Male then
            cultsim.impregnate(target, owner)
        end
    end,
    failure = function(owner, target)
         log.info(owner .. " failed to have sex with " .. target)
         local tag_component = cultsim.get_component(owner, component.tag)
    end,
    abort = function(owner, target)
        local tag_component = cultsim.get_component(owner, component.tag)
    end
}
