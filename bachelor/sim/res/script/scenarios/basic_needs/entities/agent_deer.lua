entity.deer = {
    PositionComponent = {
        position = Vec3:new(random.uniform(-scenario.bounds.x, scenario.bounds.x),
                            random.uniform(-scenario.bounds.y, scenario.bounds.y),
                            0.0)
    },
    MovementComponent = {
        speed = 80.0
    },
    SpriteComponent = {
        color = Vec3:new(1.0, 1.0, 1.0),
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
        -- Corresponds to keys in the needs.lua file
        needs = { "hunger", "thirst", "sleep", "reproduce" }
    },
    ReproductionComponent = {
        sex = random.randint(0, 1),
        max_children = 10
    },
    StrategyComponent = {
        strategies = { "consume_self", "sleep" }
    },
    HealthComponent = {
        health = 100.0
        tickdown_rate = 1.0
        vital_needs = ETag.Food | ETag.Drink | ETag.Sleep
    }
}
