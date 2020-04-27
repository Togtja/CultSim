--This file holds all sequences of actions an entity can utilize. Not all Action sequences may be available to all agents.


action_sequences = {
	find_food = {
	name = "find_food",
    tags = ETag.Food | ETag.Consume,
    current_action = {},
    actions = {actions.find_and_eat},
    run_actions = function(entity, error)
    end
    ,
    get_duration = function()
    end
    ,
    get_goal_change = function(goal)
    end
	}
}