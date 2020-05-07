--This file holds all sequences of actions an entity can utilize. Not all Action sequences may be available to all agents.


action_sequences = {
	find_food = {
	name = "find_food",
    tags = ETag.Food | ETag.Consume,
    actions = {actions.find_and_eat}
	}
}