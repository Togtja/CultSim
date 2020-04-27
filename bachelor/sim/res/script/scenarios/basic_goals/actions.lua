--This file holds all actions an entity can do. These are stored in action sequences. Not all actions need to be usable by every entity


actions = {
    find_and_eat = {
     name = "Forage and eat",
     flags = 0,
     required_time = 30,
     success_chance = 0.8,
     action = function(entity, error)
     end
     ,
     get_duration = function()
     end
     ,
     get_goal_change = function(goal)
     end
	}
}