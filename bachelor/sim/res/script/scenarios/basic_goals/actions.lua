--This file holds all actions an entity can do. These are stored in action sequences. Not all actions need to be usable by every entity


actions = {
    find_and_eat = {
     name = "Forage and eat",
     flags = 0,
     required_time = 30,
     success_chance = 0.8,
     action = function(entity, error)
        
        --We are not by the desired type of entity
        
        if !(flags & 1) then
            for i,target in ipairs(cultsim.get_component(entity,component.vision).seen) do
                --if we find our target
                if cultsim.get_component(target,component.tags).tags & ETag.Food then
                    local t_pos = cultsim.get_component(target,component.position).position
                    local e_pos = cultsim.get_component(target,component.position).position
                   if(cultsim.distance(t_pos,e_pos,10.0))then
                        flags = (flags | 1)
                        return false
                   end
                   if !cultsim.get_component(entity,component.movement).desired_position then
                        cultsim.goto(entity,pos)
                        return false
                   end
                end
            end

            --We cannot see any appropriate entity

            local x = random.uniform(-scenario.bounds.x,scenario.bounds.x)
            local y = random.uniform(-scenario.bounds.y,scenario.bounds.y)
            local z = 0.f
            cultsim.goto(entity,Vec3:new(x,y,z))
            return
        end

        --We are by the desired type of enity

        for i,target in ipairs(cultsim.get_component(entity,component.vision).seen)do
            if cultsim.get_component(target,component.tags).tags & ETag.Food then
                
                for i,goal in ipairs(cultsim.get_component(entity,component.goal).goals)do
                    if goal.tags & ETag.Food then
                    goals[i].weight -= 50.0
                end

                cultsim.kill(target)
                return true
            end
		end

        --The Entity dissapeared after we went to eat it

        error = "We lost our target after finding it"
        return false
     end
     ,
     get_duration = function(entity)
        --If we can already see our target, we will complete the action near instantly
        for i,target in pairs(cultsim.get_component(entity,component.vision).seen)do
            if(cultsim.get_component(target,component.tags).tags & ETag.Food) then
            return 1.0
        end
        --If not, it will take on average ((number of spaces to check)/(speed to check 1 space))/2 to find food (Probably wildly off, but this is just a demo anyway)
        return (((scenario.bounds.x*2) * (scenario.bounds.y*2)) / (cultsim.get_component(entity,component.movement).speed * cultsim.get_component(entity,component.movement).speed_multi))/2
     end
     ,
     get_goal_change = function(goal)
        if(goal.tags & ETag.Food)then
            return -50.0
        end
        return 0
     end
	}
}