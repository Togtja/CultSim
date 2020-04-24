entity = {
    PositionComponent = {
        position = Vec2:new(random:uniform(-scenario.bounds.x, scenario.bounds.x),
                            random:uniform(-scenario.bounds.y, scenario.bounds.y))
    },
    SpriteComponent = {
        color = Vec3:new(1.0, 1.0, 1.0),
        texture = "sprites/circle_c.png",
        normal = "sprites/circle_n.png"
    },
    TagComponent = {
        tags = ETag.Avoidable
    },
    ReproductionComponent = {
        --Just a egg, so we only need this
        average_gestation = 90,
        gestation_deviation = 5
    }
    --[[
        Could need heat, that the incubator needs to provide
        HealthComponent = {
            health = 100.0,
            tickdown_rate = 1.0,
            vital_needs = ETag.Food | ETag.Drink | ETag.Sleep
        },
    --]]
}
