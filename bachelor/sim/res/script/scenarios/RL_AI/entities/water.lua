
entity = {
    PositionComponent = {
        position = Vec2:new(0.0, 0.0)
    },
    SpriteComponent = {
        color = Vec3:new(0.10, 0.70, 1.00),
        texture = "sprites/liquid_c.png",
        normal = "sprites/liquid_n.png",
        scale = 6
    },
    TagComponent = {
        tags = ETag.Drink | ETag.Consume
    },
        NameComponent = {
        entity_type = "Water"
	}
}
