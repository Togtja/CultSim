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
}