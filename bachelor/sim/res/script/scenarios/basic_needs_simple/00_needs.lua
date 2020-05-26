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
    }  
}