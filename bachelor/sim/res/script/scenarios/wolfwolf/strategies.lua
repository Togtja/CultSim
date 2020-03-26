-- Define strategies, composed of actions that aim to achieve a greater goal

strategies.consume_self = {
    name = "Consume Self",
    tags = ETag.Food,
    actions = { actions.consume_self }
}

strategies.find_water = {
    name = "Looking for Water",
    tags = ETag.Drink,
    actions = { actions.drink }
}

strategies.find_veggie = {
    name = "Looking for plant/based food",
    tags = ETag.Food | ETag.Veggie,
    actions = { actions.eat }
}

strategies.sleep_on_ground = {
    name = "Sleeping on Ground",
    tags = ETag.Sleep,
    actions = { actions.sleep }
}

strategies.find_mate = {
    name = "Looking for a Mate",
    tags = ETag.Reproduce,
    actions = { actions.reproduce }
}

strategies.attack = {
    name = "Looking to beat someone up",
    tags = ETag.Creature,
    actions = { actions.attack }
}

strategies.hunt = {
    name = "Hunting for some food",
    tags = ETag.Creature | ETag.Food | ETag.Meat | ETag.Herbivore,
    actions = { actions.hunt }
}

