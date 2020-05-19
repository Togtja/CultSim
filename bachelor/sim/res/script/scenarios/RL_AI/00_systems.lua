-- Defines custom systems
local function move_to(rl_ai, position, dt)
    -- Update direction
    rl_ai.direction.x = rl_ai.action.loc.x - position.x
    rl_ai.direction.y = rl_ai.action.loc.y - position.y

    -- Normalize direction
    local mag = math.sqrt( rl_ai.direction.x * rl_ai.direction.x + rl_ai.direction.y * rl_ai.direction.y)
    if mag >= 0 then
        rl_ai.direction.x = rl_ai.direction.x/mag
        rl_ai.direction.y = rl_ai.direction.y/mag
    else
        log.critical("(" .. position.x .. "," .. position.y ..") within (" .. rl_ai.action.loc.x .. "," .. rl_ai.action.loc.y .. ")?")
    end

    position.x = position.x + (rl_ai.speed * rl_ai.direction.x * dt)
    position.y = position.y + (rl_ai.speed * rl_ai.direction.y * dt)

end

local function close_enough(pos1, pos2, tresh )
    local in_x = pos1.x <= pos2.x + tresh and pos1.x >= pos2.x - tresh
    local in_y = pos1.y <= pos2.y + tresh and pos1.y >= pos2.y - tresh

    if  in_x and in_y then
        return true
    end
    return false
end

lua_systems = {
    move_system = {
        update = function(dt)
            local components = {component.position, component.vision, component.lua1}
            local view = cultsim.view(components)

            view:each(function(e)
                local position = cultsim.get_component(e, component.position).position
                local rl_ai = cultsim.get_component(e, component.lua1)

                local reward = 0
                
                -- local curr_action = cultsim.get_component(e, component.lua2).action

                if rl_ai.action.ac == ai_actions.search.value or 
                rl_ai.action.ac == ai_actions.move_to_food.value or
                rl_ai.action.ac == ai_actions.move_to_water.value then

                    if rl_ai.action.ac == ai_actions.search.value then
                        rl_ai.state = ai_states.searching.value
                        reward = 0.1
                    elseif rl_ai.action.ac == ai_actions.move_to_food.value then
                        rl_ai.state = ai_states.searching.value
                        reward = 1
                    elseif rl_ai.action.ac == ai_actions.move_to_water.value then
                        rl_ai.state = ai_states.searching.value
                        reward = 2

                    end
                    move_to(rl_ai, position, dt)
                    if(close_enough(position, rl_ai.action.loc, 2)) then
                        log.warn("I have reached my goal")
                        rl_ai.prev_state = rl_ai.state
                        rl_ai.state =  ai_states.nothing.value
                    end

                elseif rl_ai.action.ac == ai_actions.eat.value then

                    local in_x = position.x <= rl_ai.action.loc.x + 2 and position.x >= rl_ai.action.loc.x - 2
                    local in_y = position.y <= rl_ai.action.loc.y + 2 and position.y >= rl_ai.action.loc.y - 2
                    -- Check if the food is withon 2 "spaces"
                    if  in_x and in_y and rl_ai.action.type == ETag.Food then
                        cultsim.modify_need(e, ETag.Food, 30)
                        cultsim.kill(rl_ai.action.entity)
                        reward = 3
                    else
                        reward = -1
                    end

                    rl_ai.prev_state = rl_ai.state
                    rl_ai.state =  ai_states.nothing.value

                elseif rl_ai.action.ac == ai_actions.drink.value then
                    
                    local in_x = position.x <= rl_ai.action.loc.x + 2 and position.x >= rl_ai.action.loc.x - 2
                    local in_y = position.y <= rl_ai.action.loc.y + 2 and position.y >= rl_ai.action.loc.y - 2
                    -- Check if the drink is withon 2 "spaces"
                    if  in_x and in_y and rl_ai.action.type == ETag.Drink then
                        cultsim.modify_need(e, ETag.Drink, 30)
                        cultsim.kill(rl_ai.action.entity)
                        reward = 4
                    else
                        reward = -1
                    end
                    
                    rl_ai.prev_state = rl_ai.state
                    rl_ai.state =  ai_states.nothing.value
                end


                local vision = cultsim.get_component(e, component.vision)
                --Finds food to eat
                local items = {}
                for i, e2 in ipairs(vision.seen) do
                    local tag_component = cultsim.get_component(e2, component.tag).tags
                        
                    if tag_component ~= nil and tag_component & (ETag.Consume) ~= 0 then
                        local e2_position = cultsim.get_component(e2, component.position).position
                        local py_pos = ai.Vec2(e2_position.x, e2_position.y)
                        local food
                        if tag_component & ETag.Food ~= 0 then
                            local food_id = tag_component & ETag.Food
                            food = ai.ItemMeta(ETag.Food, py_pos, e2)
                        end
                        if tag_component & ETag.Drink ~= 0 then
                            food = ai.ItemMeta(ETag.Drink, py_pos, e2)
                        end
                        table.insert(items, item)
                    end
                end

                if rl_ai.state == ai_states.nothing.value then
                    
                    local env = ai.Environment(ai.Vec2(position.x, position.y), rl_ai.prev_state,  items)
                    local py_action = rl_ai.q_brain.get_action(env)

                    if py_action ~= nil then
                        rl_ai.action.ac = py_action.action
                        rl_ai.action.loc = py_action.loc
                        rl_ai.action.entity = py_action.e
                        rl_ai.action.type = py_action.type

                        --print action

                        if  py_action.action == ai_actions.nothing.value then
                            log.critical("Got told to to nothing")
                        elseif py_action.action == ai_actions.search.value then
                            log.debug("Got told to to search")
                        elseif py_action.action == ai_actions.eat.value then
                            log.critical("Got told to eat")
                        elseif py_action.action == ai_actions.drink.value then
                            log.critical("Got told to drinking")
                        end
                    end
                end

                
            end)
        end
    }
}

-- Set system metatable for all systems
for k,v in pairs(lua_systems) do
    setmetatable(v, {
        __index = cultlib.LuaSystem,
        __tostring = function(table) return k end
    })
end
