-- Define strategies, composed of actions that aim to achieve a greater goal

strategies.consume_self = {
    name = "Consume Self",
    tags = ETag.Food,
    actions = { actions.consume_self }
}
