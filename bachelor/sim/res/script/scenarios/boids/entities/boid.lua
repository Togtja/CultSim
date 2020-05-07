entity = {
    PositionComponent = {
        position = Vec2:new(random:uniform(-scenario.bounds.x, scenario.bounds.x),
                            random:uniform(-scenario.bounds.y, scenario.bounds.y))
    },
    SpriteComponent = {
        color = Vec3:new(1.0, 1.0, 1.0),
        texture = "sprites/arrow_c.png",
        normal = "sprites/arrow_n.png",
        scale = 10
    },
    VisionComponent = {
        radius = 50.0,
        fov = 0 -- 0 means no FOV
    },
    TagComponent = {
        tags = ETag.Avoidable | ETag.Creature
    },
    LuaComponent = {
        lua_id = 1,
        speed = 80.0,
        velocity = Vec2.new(0, 0)
    }
}
