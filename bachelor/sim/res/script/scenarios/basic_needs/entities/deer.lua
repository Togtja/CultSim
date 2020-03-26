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
        texture = "sprites/agent_c.png",
        normal = "sprites/agent_n.png"
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
        required_needs = { needs.hunger, needs.thirst, needs.sleep, needs.reproduce },
        leisure_needs = { needs.stockpile }
    },
    ReproductionComponent = {
        sex = random:randint(0, 1),
        max_children = 10
    },
    StrategyComponent = {
        -- Corresponds to entries in the strategies.lua file
        strategies = { strategies.find_food, strategies.find_water, strategies.sleep_on_ground, strategies.find_mate, strategies.forage_for_food}
    },
    HealthComponent = {
        health = 100.0,
        tickdown_rate = 1.0,
        vital_needs = ETag.Food | ETag.Drink | ETag.Sleep
    },
    InventoryComponent = {
        max_size = 5
    },
    MemoryComponent = {
        max_memories = 10,
        max_retention_time = 1000.0,
        allowed_memories = { ETag.Food | ETag.Location, ETag.Drink | ETag.Location },
    }
}
