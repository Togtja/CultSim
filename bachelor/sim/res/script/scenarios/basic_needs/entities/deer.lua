entity = {
    PositionComponent = {
        position = Vec2:new(random:uniform(-scenario.bounds.x, scenario.bounds.x),
                            random:uniform(-scenario.bounds.y, scenario.bounds.y))
    },
    MovementComponent = {
        speed = 80.0
    },
    SpriteComponent = {
        color = Vec3:new(1.0, 0.50, 0.60),
        texture = "sprites/agent_c.png"
    },
    VisionComponent = {
        radius = 40.0,
        fov = 0 -- 0 means no FOV
    },
    TagComponent = {
        tags = ETag.Avoidable
    },
    NeedComponent = {
        -- Corresponds to entires in the needs.lua file
        needs = { needs.hunger, needs.thirst, needs.sleep, needs.reproduce }
    },
    ReproductionComponent = {
        sex = random:randint(0, 1),
        max_children = 10
    },
    StrategyComponent = {
        -- Corresponds to entries in the strategies.lua file
        strategies = { strategies.consume_self, strategies.find_water }
    },
    HealthComponent = {
        health = 100.0,
        tickdown_rate = 1.0,
        vital_needs = ETag.Food | ETag.Drink | ETag.Sleep
    }
}
