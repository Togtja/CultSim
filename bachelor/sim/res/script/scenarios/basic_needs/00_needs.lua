-- This file defines all needs available to agents. Not all agents will be using all needs
-- Likewise, not all needs are used in every scenario

needs = {
    hunger = {
        name = "Hunger",
        weight = 3.0,
        status = random:uniform(50.0, 100.0),
        threshhold = 50.0,
        decay_rate = 0.5,
        vitality = 0.5,
        tags = ETag.Food | ETag.Consume
    },
    thirst = {
        name = "Thirst",
        weight = 4.0,
        status = random:uniform(50.0, 100.0),
        threshhold = 50.0,
        decay_rate = 0.75,
        vitality = 1.0,
        tags = ETag.Drink | ETag.Consume
    },
    sleep = {
        name = "Sleep",
        weight = 1.0,
        status = random:uniform(50.0, 100.0),
        threshhold = 50.0,
        decay_rate = 0.25,
        vitality = 0.1,
        tags = ETag.Sleep
    },
    reproduce = {
        name = "Reproduction",
        weight = 0.1,
        status = random:uniform(50.0, 100.0),
        threshhold = 50.0,
        decay_rate = 0.05,
        vitality = 0.0,
        tags = ETag.Reproduce
    },
    stockpile = {
        name = "Stockpile supplies",
        weight = 0.0,
        status = 0.0,
        threshhold = 50.0,
        decay_rate = 0.0,
        vitality = 0.0,
        tags =  ETag.Gather
	},
    steal = {
        name = "Steal supplies",
        weight = 0.0,
        status = 0.0,
        threshhold = 50.0,
        decay_rate = 0.0,
        vitality = 0.0,
        tags = ETag.Crime | ETag.Gather | ETag.Hostile
	}
    
}