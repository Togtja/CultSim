entity = {
    PositionComponent = {
        position = Vec2:new(random:uniform(-scenario.bounds.x, scenario.bounds.x),
                            random:uniform(-scenario.bounds.y, scenario.bounds.y))
    },
    MovementComponent = {
        speed = 80.0
    },
    SpriteComponent = {
        color = Vec3:new(0.0, 1.0, 0.60),
        texture = "sprites/agent_c.png",
        normal = "sprites/agent_n.png"
    },
    VisionComponent = {
        radius = 60.0,
        fov = 0 -- 0 means no FOV
    },
    TagComponent = {
        tags = ETag.Avoidable
    },
    NeedComponent = {
        -- Corresponds to entires in the needs.lua file
        needs = { needs.hunger_carn, needs.thirst, needs.sleep, needs.reproduce }
    },
    ReproductionComponent = {
        sex = random:randint(0, 1),
    },
    StrategyComponent = {
        -- Corresponds to entries in the strategies.lua file
        strategies = { strategies.attack, strategies.hunt, strategies.find_water, strategies.sleep_on_ground, strategies.find_mate, strategies.hunt, strategies.attack }
    },
    HealthComponent = {
        health = 100.0,
        tickdown_rate = 1.0,
        vital_needs = ETag.Food | ETag.Drink | ETag.Sleep
    },
    MemoryComponent = {
        max_memories = 10,
        max_retention_time = 1000.0,
        allowed_memories = { ETag.Food | ETag.Location, ETag.Drink | ETag.Location },
    },
    AttackComponent = {
        damage = 20.0
    }
}
