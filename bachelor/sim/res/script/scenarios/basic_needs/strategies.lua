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

strategies.find_food = {
    name = "Looking for Food",
    tags = ETag.Food,
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
