-- Define strategies, composed of actions that aim to achieve a greater goal


strategies.find_food = {
    name = "Looking for Food",
    tags = ETag.Food | ETag.Consume,
    target_tags = ETag.Food | ETag.Consume,
    actions = { actions.eat}
}