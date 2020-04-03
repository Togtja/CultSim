-- Define strategies, composed of actions that aim to achieve a greater goal

strategies.consume_self = {
    name = "Consume Self",
    tags = ETag.Food | ETag.Consume,
    target_tags = ETag.Food,
    actions = { actions.consume_self }
}

strategies.find_water = {
    name = "Looking for Water",
    tags = ETag.Drink | ETag.Consume,
    target_tags = ETag.Drink | ETag.Consume,
    actions = { actions.drink }
}

strategies.find_food = {
    name = "Looking for Food",
    tags = ETag.Food | ETag.Consume,
    target_tags = ETag.Food | ETag.Consume,
    actions = { actions.eat}
}

strategies.loot_food = {
    name = "Looting crates for food",
    tags = ETag.Food | ETag.Gather,
    target_tags = ETag.Food | ETag.Inventory,
    actions = {actions.take_food_from_inventory}
}

strategies.loot_water = {
    name = "Looting crates for water",
    tags = ETag.Drink | ETag.Gather,
    target_tags = ETag.Drink | ETag.Inventory,
    actions = {actions.take_water_from_inventory}
}

strategies.steal_food = {
    name = "Stealing food",
    tags = ETag.Food | ETag.Gather | ETag.Crime | ETag.Hostile,
    target_tags = ETag.Inventory | ETag.Creature,
    actions = {actions.steal_food_from_entity}
}

strategies.consume_from_backpack = {
    name = "Checking backpack for food",
    tags = ETag.Food |ETag.Consume,
    target_tags = ETag.Food | ETag.Consume,
    actions = {actions.eat_from_inventory}
}

strategies.drink_from_backpack = {
    name = "Checking backpack for drink",
    tags = ETag.Drink | ETag.Consume,
    target_tags = ETag.Drink | ETag.Consume,
    actions = {actions.drink_from_inventory}
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
    target_tags = ETag.Reproduce,
    actions = { actions.reproduce }
}

strategies.forage_for_food = {
    name = "Foraging for Food",
    tags = ETag.Food | ETag.Gather,
    target_tags = ETag.Food | ETag.Consume,
    actions = {actions.gather_food}
}