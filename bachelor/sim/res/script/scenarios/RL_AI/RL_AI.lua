py = require 'python'
py.execute("import sys; sys.path.append('D:/_Programming/NTNU_Bachelor/bachelor/bachelor/sim/res/script/scenarios/RL_AI/')")
ai = py.import 'ai_logic'
ai_move = ai.MoveAction()
ai_actions = ai.AIActions
ai_states = ai.AIStates

-- Create the scenario that will be loaded
scenario = Scenario:new()

-- Give the scenario a name
scenario.name = "Testbed for RL for our agents"

-- Describe the scenario with a sensible amount of text
scenario.description = "Look at flocking behavior in real time"

-- The bounds of the level on the X and Y in both directions based on the center 200,200 means the total world is 400,400 with 200 units on both sides of 0,0
scenario.bounds = Vec2:new(100.0, 100.0)

-- Declare all systems that should be active initially
scenario.systems = {
    "SensorSystem",
    "MovementSystem",
    "NeedSystem",
    "HealthSystem",
    "TimerSystem",
    lua_systems.move_system,
    "DeletionSystem",
}


--[[ Contains what data you would like to sample
scenario.data_collectors = {
    data_collectors.velocity_collector
} 
--]]

-- Decides how many seconds between each data sample taken in this scenario. IE: (also accuracy / granularity of graphs and output)
scenario.sampling_rate = 1.0;

-- This function is called before starting the simulation, once
scenario.init = function()
    -- local inspect = require 'inspect'

    log.debug(ai.hello())
    ai_move.set_bounds(100,100)
    local foods = {}
    local po = ai.ItemMeta(0, ai.Vec2(69,69), 420)


    table.insert(foods, ai.ItemMeta(69, ai.Vec2(69,69), 420))
    table.insert(foods, ai.ItemMeta(420, ai.Vec2(69,69), 421))
    table.insert(foods, ai.ItemMeta(34, ai.Vec2(69,69), 422))

    local a1 = ai.QBrain()
    local env = ai.Environment(ai.Vec2(69,69), 0,foods)
    if env == nil then
        log.critical("I am NIL envimorment")
    end

    log.critical("The env is (" .. env.my_loc.x .. ", " .. env.my_loc.y ..")")

    local a = a1.get_action(env) 
    if a ~= nil then
        log.debug("action = " .. a.action)
    end
    

    -- Spawn Boids
    for i=1,2 do
        local ai_peeps = cultsim.spawn("r_learners")      
    end
    
    for i=1,3 do
        cultsim.spawn("food_source")
        cultsim.spawn("water_source")
    end
end



-- This function is called every frame of the simulation
scenario.update = function(dt) end

-- This function is called during the draw phase of the update
scenario.draw = function() end

-- This function is called before finishing the scenario
scenario.shutdown = function() end
