entity = {
    PositionComponent = {
        position = Vec2:new(0.0, 0.0)
    },
    SpriteComponent = {
        color = Vec3:new(0.8, 0.10, 0.30),
        texture = "sprites/berry_c.png",
        normal = "sprites/berry_n.png",
        scale = 6
    },
    TagComponent = {
        tags = ETag.Food | ETag.Consume
    },
    NameComponent = {
        entity_type = "Food"
	}
}
