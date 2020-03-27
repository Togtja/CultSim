-- Define strategies, composed of actions that aim to achieve a greater goal

strategies.consume_self = {
    name = "Consume Self",
    tags = ETag.Food, ETag.Consume,
    target_tags = ETag.Food,
    actions = { actions.consume_self }
}

strategies.find_water = {
    name = "Looking for Water",
    tags = ETag.Drink, ETag.Consume,
    target_tags = ETag.Drink,
    actions = { actions.drink }
}

strategies.find_food = {
    name = "Looking for Food",
    tags = ETag.Food | ETag.Consume,
    target_tags = ETag.Food,
    actions = { actions.eat, actions.eat_from_inventory }
}

strategies.sleep_on_ground = {
    name = "Sleeping on Ground",
    tags = ETag.Sleep,
    target_tags = ETag.Sleep,
    actions = { actions.sleep }
}

strategies.find_mate = {
    name = "Looking for a Mate",
    tags = ETag.Reproduce,
    target_tags = ETag.Sleep,
    actions = { actions.reproduce }
}

strategies.forage_for_food = {
    name = "Foraging for Food",
    tags = ETag.Food | ETag.Gather,
    target_tags = ETag.Food,
    actions = {actions.gather_food}
}