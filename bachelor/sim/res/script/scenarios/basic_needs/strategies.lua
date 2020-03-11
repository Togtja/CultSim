-- Define strategies, composed of actions that aim to achieve a greater goal

strategies.consume_self = {
    name = "Consume Self",
    tags = ETag.Food,
    actions = { "consume_self" } -- Correspond to keys in actions
}
